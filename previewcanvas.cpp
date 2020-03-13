#include "previewcanvas.h"

PreviewCanvas::PreviewCanvas(QWidget *parent) :
QGraphicsView(parent)
{
    canvasHeight=256;
    canvasLength=canvasHeight;
    numFrames=1;
    currentFrame=0;
    pixelSize = 8;
    timer = new QTimer();
    scene = new QGraphicsScene();
    this->setFixedSize(canvasLength+2, canvasHeight+2);
    this->setSceneRect(0, 0, canvasLength, canvasHeight);
    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(timer, &QTimer::timeout, this, &PreviewCanvas::update);
    timer->start(1000);

}

void PreviewCanvas::update(){
    emit getPreview(currentFrame);
    currentFrame++;
    if(currentFrame>=numFrames){
        currentFrame=0;
    }
}

void PreviewCanvas::displayPixel(QColor color, int x, int y){
    QRect rect(x*pixelSize,y*pixelSize,pixelSize,pixelSize);
    scene->addRect(rect,QPen(color),QBrush(color));
}

void PreviewCanvas::numFramesChanged(bool isAddition){
    if(isAddition){
        numFrames++;
    }else{
        numFrames--;
    }
}

void PreviewCanvas::sizeChanged(int size){
    pixelSize= canvasHeight/size;
}
PreviewCanvas::~PreviewCanvas(){
    delete scene;
    delete timer;
}
