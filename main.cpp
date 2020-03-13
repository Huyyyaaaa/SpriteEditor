#include "mainwindow.h"
#include "model.h"
#include <QApplication>

int main(int argc, char *argv[])

{
    QApplication a(argc, argv);
    Model* model = new Model();
    model->setColor(255,0,0,255);
    MainWindow w(model);
    w.show();

    return a.exec();

}
