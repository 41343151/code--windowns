#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QDate>
#include <QTime>
#include <QTreeWidgetItem>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct Task {
    QString title;
    QDate date;
    QTime time;

    QString category;
    QString color;

    bool completed;
    bool reminded;      // ⭐ 已提醒（避免重複提醒）
    bool priority;

    // Deadline / 拓樸排序
    QString groupId;
    int stepIndex;
    int totalSteps;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_calendarWidget_selectionChanged();
    void on_itemChanged(QTreeWidgetItem *item, int column);
    void on_lineEditSearch_textChanged(const QString &text);
    void checkReminder();   // ⭐ 提醒

private:
    Ui::Widget *ui;
    QVector<Task> tasks;
    QDate currentDate;

    QTreeWidgetItem *priorityRoot;
    QTreeWidgetItem *normalRoot;

    QTimer *reminderTimer;

    void rebuildTree();
    bool canCheckTask(const Task &task, QString &blockName) const;

    void saveToJson();
    void loadFromJson();

    void addNationalHolidays();
};

#endif // WIDGET_H
