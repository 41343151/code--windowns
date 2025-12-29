#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QLabel *imageLabel;   // 顯示圖片
    QLabel *infoLabel;    // 顯示座標與灰階
    QImage img;
};

#endif // MAINWINDOW_H
