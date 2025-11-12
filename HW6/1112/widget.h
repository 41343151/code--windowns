#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class QLineEdit;

class Widget : public QWidget {
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    QLineEdit *display;
    void onButton(const QString &t);
    static double eval(const QString &expr);
};

#endif // WIDGET_H
