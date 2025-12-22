#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_openButton_clicked();   // 開啟圖片
    void on_grayButton_clicked();   // 灰階處理
    void on_saveButton_clicked();   // 存檔

private:
    Ui::Widget *ui;
    QImage currentImage;            // 目前正在編輯的圖片
};

#endif // WIDGET_H
