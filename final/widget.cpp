#include "widget.h"
#include "ui_widget.h"

#include <QColorDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>
#include <algorithm>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->treeWidget->setHeaderLabel("當日行程");
    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->treeWidget, &QTreeWidget::itemChanged,
            this, &Widget::on_itemChanged);

    ui->comboBoxCategory->setEditable(true);
    ui->comboBoxCategory->addItems({"課業", "工作", "生活", "其他"});

    currentDate = QDate::currentDate();
    ui->calendarWidget->setSelectedDate(currentDate);

    loadFromJson();
    addNationalHolidays();   // ⭐ 自動加入國定假日
    rebuildTree();


    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout,
            this, &Widget::checkReminder);
    reminderTimer->start(30000); // 30 秒檢查一次
}

Widget::~Widget()
{
    saveToJson();
    delete ui;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    int size = qMax(10, width() / 60);
    QFont f = font();
    f.setPointSize(size);
    setFont(f);
    QWidget::resizeEvent(event);
}

void Widget::on_calendarWidget_selectionChanged()
{
    currentDate = ui->calendarWidget->selectedDate();
    rebuildTree();
}

void Widget::on_addButton_clicked()
{
    if (ui->lineEdit->text().trimmed().isEmpty()) return;

    QColor color = QColorDialog::getColor(Qt::black, this);
    if (!color.isValid()) return;

    QString baseTitle = ui->lineEdit->text().trimmed();
    QString groupId = QUuid::createUuid().toString();

    int days = ui->checkBoxDeadline->isChecked()
                   ? ui->spinBoxDays->value()
                   : 1;

    for (int i = 0; i < days; ++i) {
        Task t;
        t.title = baseTitle + (days > 1 ? QString("（Day %1）").arg(i + 1) : "");
        t.date = currentDate.addDays(i);
        t.time = ui->timeEdit->time();
        t.category = ui->comboBoxCategory->currentText();
        t.color = color.name();

        t.completed = false;
        t.reminded  = false;     // ⭐ 初始化
        t.priority  = ui->checkBoxPriority->isChecked();

        t.groupId = (days > 1) ? groupId : "";
        t.stepIndex = i;
        t.totalSteps = days;

        tasks.append(t);
    }

    ui->lineEdit->clear();
    rebuildTree();
}

void Widget::on_deleteButton_clicked()
{
    auto items = ui->treeWidget->selectedItems();
    if (items.isEmpty()) return;

    for (QTreeWidgetItem *item : items) {
        if (!item->parent()) continue;

        if (item->parent() == priorityRoot) {
            if (QMessageBox::question(
                    this, "確認刪除",
                    "你確定要刪除這個嗎？\n（此任務具有優先順序）")
                != QMessageBox::Yes)
                continue;
        }

        QString label = item->text(0);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                                   [&](const Task &t){
                                       return label.contains(t.title) && t.date == currentDate;
                                   }), tasks.end());
    }

    rebuildTree();
}

bool Widget::canCheckTask(const Task &task, QString &blockName) const
{
    if (task.groupId.isEmpty()) return true;

    for (const Task &t : tasks) {
        if (t.groupId == task.groupId &&
            t.stepIndex < task.stepIndex &&
            !t.completed) {
            blockName = t.title;
            return false;
        }
    }
    return true;
}

void Widget::on_itemChanged(QTreeWidgetItem *item, int)
{
    if (!item->parent()) return;

    QString label = item->text(0);

    for (Task &t : tasks) {
        if (label.contains(t.title) && t.date == currentDate) {
            if (item->checkState(0) == Qt::Checked) {
                QString block;
                if (!canCheckTask(t, block)) {
                    QMessageBox::warning(
                        this, "順序限制",
                        "前面的還沒有做完，請先完成：\n" + block);
                    item->setCheckState(0, Qt::Unchecked);
                    return;
                }
                t.completed = true;
            } else {
                t.completed = false;
            }
            break;
        }
    }
}

void Widget::on_lineEditSearch_textChanged(const QString &text)
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        auto root = ui->treeWidget->topLevelItem(i);
        bool rootVisible = false;

        for (int j = 0; j < root->childCount(); ++j) {
            auto child = root->child(j);
            bool match = child->text(0).contains(text, Qt::CaseInsensitive);
            child->setHidden(!match);
            if (match) rootVisible = true;
        }
        root->setHidden(!rootVisible && !text.isEmpty());
    }
}

/* ===== 提醒（安全版，不改完成狀態） ===== */
void Widget::checkReminder()
{
    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();

    for (Task &t : tasks) {
        if (t.reminded)  continue;
        if (t.completed) continue;
        if (t.date != today) continue;

        if (t.time <= now) {
            QMessageBox::information(
                this, "行程提醒",
                QString("提醒事項：\n%1\n時間：%2")
                    .arg(t.title)
                    .arg(t.time.toString("HH:mm"))
                );
            t.reminded = true; // ⭐ 只標記已提醒
        }
    }
}

void Widget::rebuildTree()
{
    ui->treeWidget->clear();

    priorityRoot = new QTreeWidgetItem(ui->treeWidget, {"📂 優先任務"});
    normalRoot   = new QTreeWidgetItem(ui->treeWidget, {"一般任務"});
    priorityRoot->setExpanded(true);
    normalRoot->setExpanded(true);

    for (const Task &t : tasks) {
        if (t.date != currentDate) continue;

        QString text = QString("%1 [%2] %3")
                           .arg(t.time.toString("HH:mm"))
                           .arg(t.category)
                           .arg(t.title);

        QTreeWidgetItem *item =
            new QTreeWidgetItem(
                t.priority ? priorityRoot : normalRoot,
                {text});

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, t.completed ? Qt::Checked : Qt::Unchecked);
        item->setForeground(0, QColor(t.color));
    }
}

void Widget::saveToJson()
{
    QJsonArray arr;
    for (const Task &t : tasks) {
        QJsonObject o;
        o["title"] = t.title;
        o["date"] = t.date.toString(Qt::ISODate);
        o["time"] = t.time.toString("HH:mm");
        o["category"] = t.category;
        o["color"] = t.color;
        o["completed"] = t.completed;
        o["reminded"] = t.reminded;
        o["priority"] = t.priority;
        o["groupId"] = t.groupId;
        o["stepIndex"] = t.stepIndex;
        o["totalSteps"] = t.totalSteps;
        arr.append(o);
    }

    QFile f("tasks.json");
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(QJsonDocument(arr).toJson());
}

void Widget::loadFromJson()
{
    QFile f("tasks.json");
    if (!f.open(QIODevice::ReadOnly)) return;

    QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
    tasks.clear();

    for (const auto &v : arr) {
        QJsonObject o = v.toObject();
        Task t;
        t.title = o["title"].toString();
        t.date = QDate::fromString(o["date"].toString(), Qt::ISODate);
        t.time = QTime::fromString(o["time"].toString(), "HH:mm");
        t.category = o["category"].toString();
        t.color = o["color"].toString("#000000");
        t.completed = o["completed"].toBool();
        t.reminded  = o["reminded"].toBool(false);
        t.priority  = o["priority"].toBool();
        t.groupId   = o["groupId"].toString();
        t.stepIndex = o["stepIndex"].toInt();
        t.totalSteps = o["totalSteps"].toInt();
        tasks.append(t);
    }
}

    void Widget::addNationalHolidays()
    {
        // === 台灣國定假日（範例：2026，可自行擴充） ===
        struct Holiday {
            int month;
            int day;
            QString name;
        };

        QVector<Holiday> holidays = {
            {1,  1,  "元旦"},
            {2, 28,  "和平紀念日"},
            {4,  4,  "兒童節"},
            {4,  5,  "清明節"},
            {5,  1,  "勞動節"},
            {10, 10, "國慶日"},
            {12, 25, "聖誕節"}
        };

        int year = QDate::currentDate().year();

        for (const Holiday &h : holidays) {
            QDate date(year, h.month, h.day);

            // 檢查是否已存在（避免重複加入）
            bool exists = false;
            for (const Task &t : tasks) {
                if (t.date == date && t.title == h.name) {
                    exists = true;
                    break;
                }
            }
            if (exists) continue;

            // 新增假日任務
            Task t;
            t.title = h.name;
            t.date = date;
            t.time = QTime(0, 0);              // 全天
            t.category = "國定假日";
            t.color = "#D32F2F";               // 紅色
            t.completed = false;
            t.reminded = false;
            t.priority = false;

            // 非 deadline 任務
            t.groupId = "";
            t.stepIndex = 0;
            t.totalSteps = 1;

            tasks.append(t);
        }
    }
