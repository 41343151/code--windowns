#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    // 表格
    table = new QTableWidget(0, 4, this);
    QStringList headers = {"學號", "班級", "姓名", "電話"};
    table->setHorizontalHeaderLabels(headers);

    // 輸入框
    leStudentID = new QLineEdit();
    leClass = new QLineEdit();
    leName = new QLineEdit();
    lePhone = new QLineEdit();

    // 對齊你的圖片格式：每列一個 label + lineedit
    QHBoxLayout *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("學號："));
    row1->addWidget(leStudentID);

    QHBoxLayout *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("班級："));
    row2->addWidget(leClass);

    QHBoxLayout *row3 = new QHBoxLayout;
    row3->addWidget(new QLabel("姓名："));
    row3->addWidget(leName);

    QHBoxLayout *row4 = new QHBoxLayout;
    row4->addWidget(new QLabel("電話："));
    row4->addWidget(lePhone);

    // 按鈕
    QPushButton *btnAdd = new QPushButton("新增");
    QPushButton *btnExport = new QPushButton("匯出");

    connect(btnAdd, &QPushButton::clicked, this, &Widget::addRecord);
    connect(btnExport, &QPushButton::clicked, this, &Widget::exportToTxt);

    // 主 layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addLayout(row3);
    layout->addLayout(row4);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnExport);

    layout->addLayout(btnLayout);

    setLayout(layout);
    resize(600, 400);
}

Widget::~Widget() {}

void Widget::addRecord()
{
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(leStudentID->text()));
    table->setItem(row, 1, new QTableWidgetItem(leClass->text()));
    table->setItem(row, 2, new QTableWidgetItem(leName->text()));
    table->setItem(row, 3, new QTableWidgetItem(lePhone->text()));

    leStudentID->clear();
    leClass->clear();
    leName->clear();
    lePhone->clear();
}

void Widget::exportToTxt()
{
    QString filePath = QFileDialog::getSaveFileName(this, "匯出文字檔", "", "Text Files (*.txt)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text)) return;

    QTextStream out(&file);

    for (int i = 0; i < table->rowCount(); ++i)
    {
        QString line;
        for (int j = 0; j < table->columnCount(); ++j)
        {
            line += table->item(i, j)->text() + ", ";
        }
        out << line << "\n";
    }

    file.close();
}
