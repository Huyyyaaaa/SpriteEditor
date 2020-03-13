#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>

namespace Ui {
class PreviewWindow;
}

class PreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewWindow(QWidget *parent = 0);
    ~PreviewWindow();

private:
    int fps;
    int counter;

    Ui::PreviewWindow *ui;
    QPixmap pixImage;
    QTimer timer;
    QGraphicsPixmapItem pixMap;
    QGraphicsView theView;
    QGraphicsScene *theScene;
    std::vector<QImage> imageList;

public slots:
    void timerSlot();
    void previewSlot(int value, std::vector<QImage> _imageList);
};

#endif // PREVIEWWINDOW_H
