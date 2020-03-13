#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "previewwindow.h"
#include <QColorDialog>
#include <QPixmap>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCloseEvent>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(Model *model, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setMouseTracking (true);
    QWidget::connect (ui->canvas, SIGNAL(sendMousePoint(QPointF)),this, SLOT(setMousePoint(QPointF)));
    QWidget::connect (ui->canvas, &PixelCanvas::getColor,model, &Model::getColor);
    QWidget::connect (model, &Model::sendColor, ui->canvas, &PixelCanvas::setPixel);
    QWidget::connect (ui->canvas, &PixelCanvas::changeColor,model, &Model::setColor);
    QWidget::connect (ui->colorButton, &QPushButton::pressed, this, &MainWindow::displayColorSelector);
    QWidget::connect (this, &MainWindow::changeColor,model, &Model::setColor);
    QWidget::connect(this, &MainWindow::setCurrentTool, model, &Model::setTool);
    QWidget::connect(model, &Model::sendTool, ui->canvas, &PixelCanvas::setCurrentTool);
    QWidget::connect(ui->canvas, &PixelCanvas::getColorForBucket, model, &Model::getColorForBucket);
    QWidget::connect(ui->canvas, &PixelCanvas::setEraserMode, model, &Model::setEraserMode);
    QWidget::connect(model, &Model::changeMultiplePixels, ui->canvas, &PixelCanvas::setMultiplePixels);
    QWidget::connect(this, &MainWindow::changeActiveFrame, model, &Model::changeActiveFrame);
    QWidget::connect(model, &Model::setNewFrame, ui->canvas, &PixelCanvas::setNewScene);
    QWidget::connect(this, &MainWindow::createNewFrame, model, &Model::createNewFrame);
    QWidget::connect(this, &MainWindow::deleteCurrentFrame, model, &Model::deleteCurrentFrame);
    QWidget::connect(this, &MainWindow::duplicateCurrentFrame, model, &Model::duplicateCurrentFrame);
    QWidget::connect(this, &MainWindow::showPreviewSignal, &previewWindow, &PreviewWindow::previewSlot);
    QWidget::connect(ui->fpsSlider, SIGNAL(valueChanged(int)), ui->fpsBox, SLOT(setValue(int)));
    QWidget::connect(ui->fpsBox, SIGNAL(valueChanged(int)), ui->fpsSlider, SLOT(setValue(int)));
    QWidget::connect(this, &MainWindow::exportZippedPNGs, model, &Model::exportZippedPNGs);
    QWidget::connect(this, &MainWindow::exportGIF, model, &Model::exportGIF);
    QWidget::connect(this, &MainWindow::setPenSize, ui->canvas, &PixelCanvas::setPenSize);
    QWidget::connect(this, &MainWindow::createFromFile, model, &Model::createFromFile);
    QWidget::connect(this, &MainWindow::saveFile, model, &Model::saveFile);
    QWidget::connect(this, &MainWindow::maybeSave, model, &Model::maybeSave);
    QWidget::connect(model, &Model::openWarningDialog, this, &MainWindow::openWarningDialog);
    QWidget::connect(model, &Model::closeWindow, this, &MainWindow::closeWindow);
    QWidget::connect(this, &MainWindow::changeSize, ui->canvas, &PixelCanvas::sizeChanged);
    QWidget::connect(this, &MainWindow::changeSize, model, &Model::sizeChanged);
    QWidget::connect(this, &MainWindow::changeSize,  ui->previewCanvas, &PreviewCanvas::sizeChanged);
    QWidget::connect(model, &Model::numFramesEdited,  ui->previewCanvas, &PreviewCanvas::numFramesChanged);
    QWidget::connect(this, &MainWindow::undo, model, &Model::undo);
    QWidget::connect(this, &MainWindow::redo, model, &Model::redo);
    QWidget::connect(ui->action8X8, &QAction::triggered, this, &MainWindow::on_action8X8_triggered);
    QWidget::connect(ui->action16X16, &QAction::triggered, this, &MainWindow::on_action16X16_triggered);
    QWidget::connect(ui->action32X32, &QAction::triggered, this, &MainWindow::on_action32X32_triggered);
    QWidget::connect(ui->action64X64, &QAction::triggered, this, &MainWindow::on_action64X64_triggered);
    QWidget::connect(ui->action128X128, &QAction::triggered, this, &MainWindow::on_action128X128_triggered);
    QWidget::connect(ui->canvas, &PixelCanvas::saveUndo, model, &Model::saveUndo);
    QWidget::connect(model, &Model::redraw, ui->canvas, &PixelCanvas::redraw);
    QWidget::connect(ui->previewCanvas, &PreviewCanvas::getPreview, model, &Model::getFrame);
    QWidget::connect(model, &Model::sendPreviewPixel, ui->previewCanvas, &PreviewCanvas::displayPixel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action8X8_triggered(){
    emit changeSize(8);
}

void MainWindow::on_action16X16_triggered(){
    emit changeSize(16);
}

void MainWindow::on_action32X32_triggered(){
    emit changeSize(32);
}

void MainWindow::on_action64X64_triggered(){
    emit changeSize(64);
}

void MainWindow::on_action128X128_triggered(){
    emit changeSize(128);
}

void MainWindow::on_action4X4_triggered(){
    emit changeSize(4);
}

void MainWindow::setMousePoint (QPointF point)
{
    QRect rect(point.x(), point.y(), 50, 50);
}

void MainWindow::displayColorSelector(){
    QColor newColor = QColorDialog::getColor(Qt::white);
    changeColor(newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());

    //update button with newly selected color
    if(newColor.isValid()) {
    QString qss = QString("background-color: %1").arg(newColor.name());
    ui->colorButton->setStyleSheet(qss);
    }

}

void MainWindow::on_penButton_clicked() {
    emit setCurrentTool("pencil");
    ui->penButton->setEnabled(false);
    ui->fillBucketButton->setEnabled(true);
    ui->eraseButton->setEnabled(true);
    ui->mirrorPenButton->setEnabled(true);
}

void MainWindow::on_fillBucketButton_clicked() {
    emit setCurrentTool("fill");
    ui->penButton->setEnabled(true);
    ui->fillBucketButton->setEnabled(false);
    ui->eraseButton->setEnabled(true);
    ui->mirrorPenButton->setEnabled(true);
}

void MainWindow::on_eraseButton_clicked() {
    emit setCurrentTool("eraser");
    ui->penButton->setEnabled(true);
    ui->fillBucketButton->setEnabled(true);
    ui->eraseButton->setEnabled(false);
    ui->mirrorPenButton->setEnabled(true);
}

void MainWindow::on_mirrorPenButton_triggered(QAction *arg1) {
    emit setCurrentTool("mirror");
}

void MainWindow::on_mirrorPenButton_clicked() {
    emit setCurrentTool("mirror");
    ui->penButton->setEnabled(true);
    ui->fillBucketButton->setEnabled(true);
    ui->eraseButton->setEnabled(true);
    ui->mirrorPenButton->setEnabled(false);
}

void MainWindow::on_upButton_clicked() {
    emit changeActiveFrame(false);
}

void MainWindow::on_downButton_clicked() {
    emit changeActiveFrame(true);
}

void MainWindow::on_addButton_clicked() {
    emit createNewFrame();

}

void MainWindow::on_deleteButton_clicked() {
    emit deleteCurrentFrame();
}

void MainWindow::on_dupeButton_clicked() {
    emit duplicateCurrentFrame();
}

void MainWindow::on_previewButton_clicked()
{
    emit showPreviewSignal(ui->fpsBox->value(), imageList);
    previewWindow.show();
}

void MainWindow::on_menuExportGIF_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export As GIF"), "./", tr("Image Files (*.gif)"));

    if (fileName != "") {
        emit exportGIF(fileName, ui->fpsSlider->value());
    }
}

void MainWindow::on_menuExportPNG_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Zipped PNGs"), "./", tr("Zip Files (*.zip)"));

    if (fileName != "") {
        emit exportZippedPNGs(fileName);
    }
}
//1x1
void MainWindow::on_penSize1Button_clicked()
{
    emit setPenSize(1);
    ui->penSize1Button->setEnabled(false);
    ui->penSize2Button->setEnabled(true);
    ui->penSize3Button->setEnabled(true);
    ui->penSize4Button->setEnabled(true);

}
//2x2
void MainWindow::on_penSize2Button_clicked()
{
    emit setPenSize(2);
    ui->penSize1Button->setEnabled(true);
    ui->penSize2Button->setEnabled(false);
    ui->penSize3Button->setEnabled(true);
    ui->penSize4Button->setEnabled(true);
}
//3x3
void MainWindow::on_penSize3Button_clicked()
{
    emit setPenSize(3);
    ui->penSize1Button->setEnabled(true);
    ui->penSize2Button->setEnabled(true);
    ui->penSize3Button->setEnabled(false);
    ui->penSize4Button->setEnabled(true);
}
//4x4
void MainWindow::on_penSize4Button_clicked()
{
    emit setPenSize(4);
    ui->penSize1Button->setEnabled(true);
    ui->penSize2Button->setEnabled(true);
    ui->penSize3Button->setEnabled(true);
    ui->penSize4Button->setEnabled(false);
}

void MainWindow::on_menuLoad_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open sprite"), "", tr("Sprite (*.ssp);;All Files (*)"));

    if(!fileName.isEmpty()) {
        emit createFromFile(fileName);
    }
}

void MainWindow::on_menuSave_triggered() {
    saveFileDialog();
}

void MainWindow::closeEvent (QCloseEvent *event) {
    event->ignore();
    emit maybeSave();
}

void MainWindow::openWarningDialog() {
    QString title("Unsaved Changes");
    QString body("Edits have been made since the last save. Would you like to save your changes before you exit?");
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, title, body, QMessageBox::Cancel | QMessageBox::No | QMessageBox::Save, QMessageBox::Save);

    if(resBtn == QMessageBox::Save) {
        saveFileDialog();
    }
    else if(resBtn == QMessageBox::No) {
        closeWindow();
    }
}

void MainWindow::saveFileDialog() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save"), "./", tr("Sprite (*.ssp)"));

    if(!fileName.isEmpty()) {
        emit saveFile(fileName);
    }
}

void MainWindow::closeWindow() {
    QApplication::quit();
}

void MainWindow::on_undoButton_clicked()
{
    emit undo();
}

void MainWindow::on_redoButton_clicked()
{
    emit redo();
}
