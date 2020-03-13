#include "pixel.h"

Pixel::Pixel()
{
    color = QColor(0,0,0,255);
}

Pixel::Pixel(QColor _color){
    color=_color;
}

QColor Pixel::getColor(){
    return color;
}

void Pixel::setColor(int r, int g, int b, int a){
    color = QColor(r,g,b,a);

}
