#ifndef PIXEL_H
#define PIXEL_H

#include <QObject>
#include <QColor>

class Pixel
{
public:
    Pixel();
    Pixel(QColor);
    void setColor(int, int, int, int);
    QColor getColor();
private:
    QColor color;
};

#endif // PIXEL_H
