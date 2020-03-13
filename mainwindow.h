#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "model.h"
#include "previewwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Model*, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PreviewWindow previewWindow;
    std::vector<QImage> imageList;
    void closeEvent (QCloseEvent *event);
    void saveFileDialog();

signals:
    void changeColor(int,int,int,int);
    void setCurrentTool(std::string);
    void changeActiveFrame(bool); // true is up in the list, false is down
    void createNewFrame();
    void deleteCurrentFrame();
    void duplicateCurrentFrame();
    void showPreviewSignal(int value, const std::vector<QImage>& imageList);
    void exportZippedPNGs(QString);
    void exportGIF(QString, int);
    void setPenSize(int);
    void createFromFile(QString);
    void saveFile(QString);
    void maybeSave();
    void changeSize(int);
    void sizeChanged(int);
    void undo();
    void redo();

public slots:
    void setMousePoint (QPointF point);
    void displayColorSelector();
    void openWarningDialog();
    void closeWindow();

private slots:
    void on_fillBucketButton_clicked();
    void on_penButton_clicked();
    void on_eraseButton_clicked();
    void on_mirrorPenButton_triggered(QAction *arg1);
    void on_mirrorPenButton_clicked();
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_dupeButton_clicked();
    void on_previewButton_clicked();
    void on_menuExportGIF_triggered();
    void on_menuExportPNG_triggered();
    void on_penSize1Button_clicked();
    void on_penSize2Button_clicked();
    void on_penSize3Button_clicked();
    void on_penSize4Button_clicked();
    void on_menuLoad_triggered();
    void on_menuSave_triggered();
    void on_action8X8_triggered();
    void on_action16X16_triggered();
    void on_action32X32_triggered();
    void on_action64X64_triggered();
    void on_action128X128_triggered();
    void on_action4X4_triggered();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
};
#endif // MAINWINDOW_H
