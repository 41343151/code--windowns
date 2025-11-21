#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void addRecord();
    void exportToTxt();

private:
    QTableWidget *table;
    QLineEdit *leStudentID;
    QLineEdit *leClass;
    QLineEdit *leName;
    QLineEdit *lePhone;
};

#endif // WIDGET_H
