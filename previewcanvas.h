#ifndef PREVIEWCANVAS_H
#define PREVIEWCANVAS_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QObject>
#include <QTimer>

class PreviewCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit PreviewCanvas(QWidget *parent = 0);
    ~PreviewCanvas();
private:
    QGraphicsScene* scene;
    int canvasHeight;
    int canvasLength;
    int pixelSize;
    int numFrames;
    int currentFrame;
    QTimer* timer;
signals:
    void getPreview(int);
public slots:
    void update();
    void displayPixel(QColor,int,int);
    void sizeChanged(int);
    void numFramesChanged(bool);
};

#endif // PREVIEWCANVAS_H
