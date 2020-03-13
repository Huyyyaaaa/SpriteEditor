#include "previewwindow.h"
#include "ui_previewwindow.h"

PreviewWindow::PreviewWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Preview Animation");
    connect(&timer, &QTimer::timeout, this, &PreviewWindow::timerSlot);
}

PreviewWindow::~PreviewWindow()
{
    delete ui;
}

void PreviewWindow::timerSlot()
{
    theScene = new QGraphicsScene(ui->previewView);
    QImage theImage = imageList.at(counter % imageList.size());
    pixImage = QPixmap::fromImage(theImage);
    theScene->addPixmap(pixImage);
    theScene->setSceneRect(pixImage.rect());
    ui->previewView->setScene(theScene);
    counter++;
}

void PreviewWindow::previewSlot(int value, std::vector<QImage> _imageList)
{
    fps = value;
    imageList = _imageList;
    counter = imageList.size();
    timer.start((1000 / fps));
}

