#include "frame.h"
#include <QDebug>
Frame::Frame(int size) {
    height = size;
    width = size;
    for(int i = 0; i < height; i++){
        vector<Pixel> temp;
        for(int j = 0; j < width; j++){
            temp.push_back(Pixel());
        }
        pixels.push_back(temp);
    }
}

Frame::~Frame()
{

}

//std::tuple<int, int, int, int> Frame::getPixel(int x, int y)
//{
//    return this->pixels[x][y];
//}

void Frame::setPixel(int x, int y, QColor color)
{
    pixels[y][x] = Pixel(color);

    //qDebug() << pixels[y][x].getColor().name();
//    for(int i = 0; i < height; i++){
//        for(int j = 0; j < width; j++){
//            qDebug() << pixels.at(i).at(j).getColor().name();
//        }
//    }
}

vector<vector<Pixel>> Frame::getPixels() {
    return pixels;
}

//void Frame::setPixel(int x, int y, int r, int g, int b, int a)
//{
//    this->pixels[x][y] = std::make_tuple(r, g, b, a);
//}

QColor Frame::getColor(int x, int y)
{
    return pixels[y][x].getColor();
}

void Frame::setSize(int size){
    qDebug() << size;
//    if(size<width){
        pixels.resize(size);
        for(int index = 0; index < size; index++){
            pixels[index].resize(size);
            //pixels[index].erase(pixels[index].begin()+size);
        }
//    }else if(size>width){
//        for(int y = 0; y < height; y++){
//            for(int x = width; x < size; x++){
//                pixels[y].push_back(Pixel());
//            }
//        }
//        for(int y = height; y < size; y++){
//            pixels.push_back(vector<Pixel>());
//            for(int x = 0; x < size; x++){
//                pixels[y].push_back(Pixel());
//            }
//        }
//    }
    height = size;
    width = size;
}

int Frame::getFrameWidth() {
    return width;
}

int Frame::getFrameHeight() {
    return height;
}

void Frame::setPixels(vector<vector<Pixel>> newPixels) {
    pixels = newPixels;
}
