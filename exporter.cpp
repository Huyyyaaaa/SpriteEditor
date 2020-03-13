#include "exporter.h"
#include "qgifimage.h"
#include "zip.h"

Exporter::Exporter(QString fileName, vector<Frame> frames, int fps) :
    fileName(fileName), fps(fps), frames(frames) {

    // QImages are easier to work with, and since both methods for the Exporter
    // class rely on them, it's easier to convert each frame into a QImage here
    // in the constructor instead of doing it later on
    for (Frame frame : frames) {
        vector<vector<Pixel>> pixels = frame.getPixels();
        widthAndHeight = static_cast<int>(pixels.size());
        QImage image(widthAndHeight, widthAndHeight, QImage::Format_RGBA8888);

        for (size_t column = 0; column < pixels.size(); column++) {
            for (size_t row = 0; row < pixels[column].size(); row++) {
                QColor pixelColor = pixels[column][row].getColor();
                image.setPixelColor(static_cast<int>(row), static_cast<int>(column), pixelColor);
            }
        }
        images.push_back(image);
    }
}

void Exporter::saveGIF() {
    QGifImage gif(QSize(widthAndHeight, widthAndHeight));
    gif.setDefaultDelay(ONE_SECOND / fps); // ms delay between frames. 1fps = 1000 ms delay, 2 fps = 500 ms delay, etc.
    for (QImage image : images) {
        gif.addFrame(image);
    }
    gif.save(fileName);
}

void Exporter::saveZippedPNGs() {
    // Zip library has to use const char* names, so this conversion happens a lot
    QByteArray fileNameInBytes = fileName.toUtf8();
    const char* constFileName = fileNameInBytes.constData();
    QString savePath = fileName.left(fileName.size() - 4);

    // Temporarily save every frame as a .png so the zip lib can read them as existing files.
    // There's probably a way to do this through buffers without full saves, but I couldn't
    // get that to work.
    vector<QString> imageNames;
    for (size_t imageNum = 0; imageNum < images.size(); imageNum++) {
        QString imagePath = QString("%1%2%3").arg(savePath).arg(imageNum).arg(".png");
        imageNames.push_back(imagePath);
        images[imageNum].save(imagePath);
    }

    // Put each temp .png file into the zipped folder
    struct zip_t *zip = zip_open(constFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        for (size_t imageNum = 0; imageNum < imageNames.size(); imageNum++) {
            QString frameName = QString("frame%1.png").arg(imageNum);
            QByteArray frameNameInBytes = frameName.toUtf8();
            const char* constFrameName = frameNameInBytes.constData();

            QByteArray imagePathInBytes = imageNames[imageNum].toUtf8();
            const char* constImagePath = imagePathInBytes.constData();

            zip_entry_open(zip, constFrameName);
            {
                zip_entry_fwrite(zip, constImagePath);
            }
            zip_entry_close(zip);
        }
    }
    zip_close(zip);

    // Cleanup the temporary .png files we had to save
    for (size_t imageNum = 0; imageNum < imageNames.size(); imageNum++) {
        QFile file(imageNames[imageNum]);
        file.remove();
    }
}
