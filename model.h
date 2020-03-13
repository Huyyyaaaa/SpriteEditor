#ifndef MODEL_H
#define MODEL_H
#include "frame.h"
#include <QObject>
#include <QColor>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <vector>
#include <tuple>
class Model : public QObject {
    Q_OBJECT
private:
    bool eraserMode;
    bool modified;
    QColor currentColor;
    vector<Frame> frames;
    Frame* activeFrame;
    std::string currentTool;
    unsigned long frameIndex;
    stack<vector<vector<Pixel>>> undolist;
    stack<vector<vector<Pixel>>> redolist;
    void fillBucketRecursive(QColor,int,int,std::vector<std::tuple<int, int>>&);
    void makeFreshStacks();
public:
    Model();
public slots:
    void getColor(int,int, bool);
    void setColor(int,int,int,int);
    void setTool(std::string);
    void setEraserMode(bool);
    void getColorForBucket(int,int);
    void changeActiveFrame(bool);
    void createNewFrame();
    void deleteCurrentFrame();
    void duplicateCurrentFrame();
    void exportGIF(QString, int);
    void exportZippedPNGs(QString);
    void createFromFile(QString);
    void saveFile(QString);
    void maybeSave();
    void saveUndo();
    void undo();
    void redo();
    void sizeChanged(int);
    void getFrame(int);

signals:
    void sendColor(QColor,int,int);
    void sendTool(std::string);
    void changeMultiplePixels(QColor,std::vector<std::tuple<int,int>>);
    void setNewFrame(vector<vector<Pixel>>);
    void redraw(vector<vector<Pixel>>);
    void openWarningDialog();
    void closeWindow();
    void sendPreviewPixel(QColor, int, int);
    void numFramesEdited(bool);
};
#endif // MODEL_H
