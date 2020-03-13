#ifndef FRAME_H
#define FRAME_H
#include <vector>
#include <tuple>
#include <iostream>
#include <QColor>
#include <QGraphicsScene>
#include <QObject>
#include "pixel.h"
#include "pixelcanvas.h"

using namespace std;
class Frame
{
public:
    Frame(int);
    ~Frame();

    void setPixel(int x, int y, QColor color);
//    void setPixel(int x, int y, std::tuple<int, int, int, int> p);
//    std::tuple<int, int, int, int> getPixel(int x, int y);
    QColor getColor(int x, int y);
    vector<vector<Pixel>> getPixels();
    void setPixels(vector<vector<Pixel>>);
//    QImage *image;
    int getFrameWidth();
    int getFrameHeight();
    void setSize(int);

private:
    int width;
    int height;
    vector<vector<Pixel>> pixels;

};

#endif // FRAME_H
