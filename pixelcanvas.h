#ifndef PIXELCANVAS_H
#define PIXELCANVAS_H
#include <pixel.h>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <stack>
#include <tuple>
using namespace std;

class PixelCanvas : public QGraphicsView
{
    Q_OBJECT

    public:
    explicit PixelCanvas(QWidget *parent = 0);
    ~PixelCanvas();

    signals:
    void sendMousePoint(QPointF point);
    void getColor(int,int, bool);
    void getColorForBucket(int, int);
    void changeColor(int, int, int, int);
    void setEraserMode(bool);
    void passMultiplePixels(vector<tuple<int,int>>);
    void saveUndo();

    public slots:
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent (QMouseEvent *move);
    void mouseReleaseEvent(QMouseEvent *event);
    void setPixel(QColor, int, int);
    void setCurrentTool(string tool);
    void setMultiplePixels(QColor,vector<tuple<int,int>>);
    void setNewScene(vector<vector<Pixel>>);
    void setPenSize(int);
    void sizeChanged(int);
    void redraw(vector<vector<Pixel>>);

    private:
    bool checkCoordinates(int, int);
    bool checkAlreadyDrawn(int x, int y);
    void drawPixels(int, int);
    QGraphicsScene * scene;
    bool pressed;
    int canvasHeight;
    int canvasLength;
    int pixelSize;
    int penSize;
    stack<QRect> strokes;
    std::string currentTool;
};

#endif // PIXELCANVAS_H
