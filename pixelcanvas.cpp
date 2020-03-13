#include "pixelcanvas.h"
#include "mainwindow.h"
#include <QPointF>
#include <QDebug>
#include <math.h>
#include <random>


PixelCanvas::PixelCanvas(QWidget *parent) :
QGraphicsView(parent)
{
    canvasHeight=512;
    canvasLength=canvasHeight;
    pixelSize=16;
    currentTool = "pencil";
    penSize = 1;
    scene = new QGraphicsScene();
    this->setFixedSize(canvasLength+2, canvasHeight+2);
    this->setSceneRect(0, 0, canvasLength, canvasHeight);
    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for(int x=0; x<=canvasLength; x+=pixelSize) {
        scene->addLine(x,0,x,canvasLength, QPen(Qt::gray));

    }
    for (int y=0; y<=canvasHeight; y+=pixelSize){
        scene->addLine(0,y,canvasHeight,y, QPen(Qt::gray));
    }

    QPixmap cursorPen = QPixmap(":/resources/Icons/pencil.png");
    QCursor cursor = QCursor(cursorPen, 17, 46); // 17, 46 is the point of the pencil.
    this->setCursor(cursor);
}

void PixelCanvas::mousePressEvent(QMouseEvent * e)
{
    QPointF pt = mapToScene(e->pos());
    pressed = true;

    if (currentTool == "fill") {
        emit getColorForBucket(static_cast<int>(floor(pt.x()/pixelSize)), static_cast<int>(floor(pt.y()/pixelSize)));
    }
    else {
        pressed = true;

        int xPosStart = static_cast<int>(floor(pt.x()/pixelSize));
        int xPosEnd = xPosStart + penSize;
        int yPosStart = static_cast<int>(floor(pt.y()/pixelSize));
        int yPosEnd = yPosStart + penSize;

        for (int xCount = xPosStart; xCount < xPosEnd; xCount++) {
            for (int yCount = yPosStart; yCount < yPosEnd; yCount++) {
                emit getColor(xCount, yCount, currentTool=="mirror");
            }
        }
    }
}

void PixelCanvas::mouseReleaseEvent(QMouseEvent * e)
{
    //qDebug() << "pressed is false";
    pressed = false;
    emit saveUndo();
}

void PixelCanvas::mouseMoveEvent (QMouseEvent *move)
{
    int x = move->pos().rx();
    int y = move->pos().ry();
    //qDebug() << "x=" << x << " y=" << y << "pressed= " << pressed << "checkAlreadyDrawn= " << !checkAlreadyDrawn(x,y) << "checkCoordinates=" << checkCoordinates(x,y);
    if(pressed && !checkAlreadyDrawn(x,y) && checkCoordinates(x,y) ){
        int xPosStart = static_cast<int>(floor(x/pixelSize));
        int xPosEnd = xPosStart + penSize;
        int yPosStart = static_cast<int>(floor(y/pixelSize));
        int yPosEnd = yPosStart + penSize;

        for (int xCount = xPosStart; xCount < xPosEnd; xCount++) {
            for (int yCount = yPosStart; yCount < yPosEnd; yCount++) {
                emit getColor(xCount, yCount, currentTool=="mirror");
            }
        }
    }
}

void PixelCanvas::setPixel(QColor color, int x, int y){
    int rectSize;
    int rectLocationX = (x * pixelSize) + 1; // Allows the gray line to stay between the elements
    int rectLocationY = (y * pixelSize) + 1;
    rectSize = pixelSize - 2;

    QRect rect(rectLocationX, rectLocationY, rectSize, rectSize);
    strokes.push(rect);
    scene->addRect(rect,QPen(color),QBrush(color));
    if(penSize > 1) {
        for(int count = 0,
            pixelDimX = rectLocationX - 1,
            pixelDimY = rectLocationY - 1,
            rectSizeXScaled = rectLocationX + rectSize + 2,
            rectSizeYScaled = rectLocationY + rectSize + 2;
            count < penSize;
            count++,
            pixelDimX+=pixelSize,
            pixelDimY+=pixelSize) {
            scene->addLine(rectLocationX, pixelDimY, rectSizeXScaled, pixelDimY, QPen(Qt::gray));
            scene->addLine(pixelDimX, rectLocationY, pixelDimX, rectSizeYScaled, QPen(Qt::gray));
        }
    }
}

void PixelCanvas::setPenSize(int size) {
    penSize = size;
}

void PixelCanvas::setCurrentTool(std::string tool) {
    currentTool = tool;
    qDebug() << "New tool is: " << currentTool.c_str();
    QPixmap cursorPen;
    QCursor cursor;
    if(currentTool == "pencil") {
        cursorPen = QPixmap(":/resources/Icons/pencil.png");
        cursor = QCursor(cursorPen, 17, 46); // 17, 46 is the point of the pencil.
        emit setEraserMode(false);
    }
    else if(currentTool == "fill") {
        cursorPen = QPixmap(":/resources/Icons/bucket.png");
        cursor = QCursor(cursorPen, 48, 43); // 48, 43 is the point of the fill bucket.
        emit setEraserMode(false);
    }
    else if(currentTool == "eraser") {
        cursorPen = QPixmap(":/resources/Icons/eraser.png");
        cursor = QCursor(cursorPen, 12, 41); // 12, 41 is the point of the eraser.
        emit setEraserMode(true);
    }
    else if(currentTool == "mirror") {
        cursorPen = QPixmap(":/resources/Icons/mirror.png");
        cursor = QCursor(cursorPen, 28, 54); // 28, 54 is the point of the mirror pen.
        emit setEraserMode(false);
    }

    this->setCursor(cursor);
}

void PixelCanvas::setMultiplePixels(QColor color,vector<tuple<int,int>> coordinates) {
    std::vector<tuple<int,int>>::iterator it;
    for (it = coordinates.begin(); it != end (coordinates); ++it) {
        setPixel(color, std::get<0>(*it), std::get<1>(*it));
    }
}

void PixelCanvas::setNewScene(vector<vector<Pixel>> pixels) {
    vector<vector<Pixel>>::iterator it;
    for (it = pixels.begin(); it != pixels.end(); ++it) {
        int yIndex = it - pixels.begin();
        vector<Pixel>::iterator pixelIt;
        for(pixelIt = it->begin(); pixelIt != it->end(); ++pixelIt) {
            int xIndex = pixelIt - it->begin();
            setPixel(pixelIt->getColor(), xIndex, yIndex);
        }
    }
}

void PixelCanvas::redraw(vector<vector<Pixel>> pixels){
    for(int y = 0; y < pixels.size(); y++){
        for(int x = 0; x < pixels[y].size(); x++){
            setPixel(pixels[y][x].getColor(),x,y);
            //qDebug() << pixels[y][x].getColor();
            //qDebug() << QColor(255,0,0,255);
        }
    }
    for(int x=0; x<=canvasLength; x+=pixelSize) {
        scene->addLine(x,0,x,canvasLength, QPen(Qt::gray));
    }
    for (int y=0; y<=canvasHeight; y+=pixelSize){
        scene->addLine(0,y,canvasHeight,y, QPen(Qt::gray));
    }
}

void PixelCanvas::sizeChanged(int size){
    pixelSize=canvasHeight/size;
}

bool PixelCanvas::checkCoordinates(int x, int y){
    return x >= 0 && x < 512 && y >= 0  && y < 512;
}

bool PixelCanvas::checkAlreadyDrawn(int x, int y){
    QRect lastDrawn = strokes.top();
    return lastDrawn.x() == floor(x/pixelSize)*pixelSize+1 && lastDrawn.y() == floor(y/pixelSize)*pixelSize+1;
}

PixelCanvas::~PixelCanvas(){
    delete scene;
}
