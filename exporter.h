#ifndef EXPORTER_H
#define EXPORTER_H
#include "frame.h"
#include <vector>

class Exporter {
public:
    Exporter(QString, vector<Frame>, int fps = 1);
    void saveGIF();
    void saveZippedPNGs();

private:
    QString fileName;
    int fps;
    vector<Frame> frames;
    vector<QImage> images;
    static const int ONE_SECOND = 1000;
    int widthAndHeight;
};

#endif // EXPORTER_H
