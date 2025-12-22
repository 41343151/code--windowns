#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QColor>

/* ===== 建構子 ===== */
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

/* ===== 解構子 ===== */
Widget::~Widget()
{
    delete ui;
}

/* ===== 開啟圖片 ===== */
void Widget::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "開啟圖片",
        "",
        "Images (*.png *.jpg *.bmp)"
        );

    if (fileName.isEmpty())
        return;

    if (!currentImage.load(fileName)) {
        QMessageBox::warning(this, "錯誤", "圖片載入失敗");
        return;
    }

    ui->label->setPixmap(QPixmap::fromImage(currentImage));
}

/* ===== 灰階處理 ===== */
void Widget::on_grayButton_clicked()
{
    if (currentImage.isNull())
        return;

    for (int y = 0; y < currentImage.height(); ++y) {
        for (int x = 0; x < currentImage.width(); ++x) {
            QColor c = currentImage.pixelColor(x, y);
            int g = (c.red() + c.green() + c.blue()) / 3;
            currentImage.setPixelColor(x, y, QColor(g, g, g));
        }
    }

    ui->label->setPixmap(QPixmap::fromImage(currentImage));
}

/* ===== 存檔（QImage::save） ===== */
void Widget::on_saveButton_clicked()
{
    if (currentImage.isNull()) {
        QMessageBox::warning(this, "存檔", "沒有圖片可以存");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "儲存圖片",
        "",
        "PNG (*.png);;JPG (*.jpg);;BMP (*.bmp)"
        );

    if (fileName.isEmpty())
        return;

    if (!currentImage.save(fileName)) {
        QMessageBox::critical(this, "錯誤", "存檔失敗");
        return;
    }

    QMessageBox::information(this, "完成", "圖片已成功存檔");
}
