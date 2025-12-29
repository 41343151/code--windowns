#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ✅ 建立中央 Widget（不使用 ui）
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // ✅ 建立 Layout
    QVBoxLayout *layout = new QVBoxLayout(central);

    // ✅ 建立顯示影像的 QLabel
    imageLabel = new QLabel;
    imageLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    imageLabel->setMouseTracking(true);

    // ✅ 建立顯示資訊的 QLabel
    infoLabel = new QLabel("X:  Y:  Gray:");
    infoLabel->setFixedHeight(30);

    layout->addWidget(imageLabel);
    layout->addWidget(infoLabel);

    // ✅ 整個視窗都允許滑鼠移動事件
    setMouseTracking(true);
    central->setMouseTracking(true);

    // ✅ 載入圖片（請確認路徑）
    img.load("test.jpg");

    imageLabel->setPixmap(QPixmap::fromImage(img));
}

MainWindow::~MainWindow()
{
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    if (x >= 0 && y >= 0 &&
        x < img.width() &&
        y < img.height())
    {
        int gray = qGray(img.pixel(x, y));

        infoLabel->setText(
            QString("X:%1  Y:%2  Gray:%3")
                .arg(x)
                .arg(y)
                .arg(gray)
            );
    }
}
