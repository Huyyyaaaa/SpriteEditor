#include "model.h"
#include "exporter.h"
#include <QDebug>

Model::Model(){
    frames.push_back(Frame(64));
    currentTool = "pencil";
    frameIndex = 0;
    activeFrame = &frames.at(frameIndex);
    eraserMode = false;
    modified = false;
    undolist.push(activeFrame->getPixels());
}

void Model::getColor(int x,int y, bool mirror) {
    if(x < activeFrame->getFrameWidth() && y < activeFrame->getFrameHeight()){
        if(eraserMode) {
            activeFrame->setPixel(x, y, QColor(Qt::black));
            emit sendColor(QColor(Qt::black), x, y);
        }
        else {
            if(mirror){
                activeFrame->setPixel(activeFrame->getFrameWidth()-x-1,y,currentColor);
                emit sendColor(currentColor,activeFrame->getFrameWidth()-x-1,y);
            }
            activeFrame->setPixel(x,y,currentColor);
            emit sendColor(currentColor, x, y);

        }
        modified = true;
        //qDebug() << "x=" << x << " y=" << y;
    }
}
void Model::setColor(int r,int g,int b,int a){
    currentColor = QColor(r,g,b,a);
}

void Model::setEraserMode(bool mode) {
    eraserMode = mode;
}

void Model::setTool(std::string tool) {
    currentTool = tool;
    emit sendTool(currentTool);
}

void Model::getColorForBucket(int x,int y) {
    QColor clickedColor = activeFrame->getColor(x, y);
    qDebug() << clickedColor;
    std::vector<std::tuple<int,int>> pixelsChanged;

    // No changes need to be done if they've clicked with the same color that they currently have
    if(clickedColor == currentColor) {
        return;
    }

    fillBucketRecursive(clickedColor, x, y, pixelsChanged);

    emit changeMultiplePixels(currentColor,pixelsChanged);
    modified = true;
}



void Model::fillBucketRecursive(QColor clickedColor,int x,int y,std::vector<std::tuple<int, int>>& pixelsChangeList) {
    bool withinBounds = x >= 0 && x < activeFrame->getFrameWidth() && y >= 0 && y < activeFrame->getFrameHeight();
    qDebug() << "Within bounds" << withinBounds;

    if(withinBounds && activeFrame->getColor(x,y) == clickedColor) {
        activeFrame->setPixel(x,y,currentColor);
        pixelsChangeList.push_back(std::make_tuple(x,y));

        fillBucketRecursive(clickedColor,x-1,y,pixelsChangeList);
        fillBucketRecursive(clickedColor,x+1,y,pixelsChangeList);
        fillBucketRecursive(clickedColor,x,y+1,pixelsChangeList);
        fillBucketRecursive(clickedColor,x,y-1,pixelsChangeList);
    }
}

void Model::saveUndo(){
    undolist.push(activeFrame->getPixels());
}


void Model::undo(){
    if(undolist.size() > 1){
        vector<vector<Pixel>> ul = undolist.top();
        redolist.push(ul);
        undolist.pop();
        activeFrame->setPixels(undolist.top());
        emit setNewFrame(activeFrame->getPixels());
    }
}


void Model::redo(){
    if(redolist.size() > 0){
        vector<vector<Pixel>> rl = redolist.top();
        activeFrame->setPixels(rl);
        emit setNewFrame(activeFrame->getPixels());
        redolist.pop();
        undolist.push(rl);
    }
}

/**
 * Changes the active frame. When the down button is clicked, the user wants to move forward in the list to the next item if possible,
 * and vice versa.
 * @brief Model::changeActiveFrame
 * @param forwards
 */
void Model::changeActiveFrame(bool forwards) {
    if(forwards) {
        if(frameIndex != frames.size() - 1) {
            ++frameIndex;
            activeFrame = &frames.at(frameIndex);
        }
    }
    else {
        if(frameIndex != 0) {
            --frameIndex;
            activeFrame = &frames.at(frameIndex);
        }
    }
    emit setNewFrame(activeFrame->getPixels());
    makeFreshStacks();
}

void Model::createNewFrame() {
    frames.push_back(Frame(activeFrame->getFrameWidth()));
    ++frameIndex;
    activeFrame = &frames.at(frameIndex);

    emit setNewFrame(activeFrame->getPixels());
    emit numFramesEdited(true);
    modified = true;
    makeFreshStacks();
}

void Model::deleteCurrentFrame() {
    frames.erase(frames.begin() + static_cast<long>(frameIndex));
    // No project allowed to have 0 frames, so create one if they delete the last
    if(frames.size() == 0) {
        frames.push_back(Frame(activeFrame->getFrameWidth()));
    }
    else if (frameIndex == frames.size()) {
        --frameIndex;
    }

    activeFrame = &frames.at(frameIndex);
    emit setNewFrame(activeFrame->getPixels());
    emit numFramesEdited(false);
    modified = true;
    makeFreshStacks();
}

void Model::duplicateCurrentFrame() {
    frames.push_back(*activeFrame);
    ++frameIndex;
    activeFrame = &frames.at(frameIndex);

    emit setNewFrame(activeFrame->getPixels());
    emit numFramesEdited(true);
    modified = true;
    makeFreshStacks();
}

void Model::exportGIF(QString fileName, int fps) {
    Exporter exporter(fileName, frames, fps);
    exporter.saveGIF();
}

void Model::exportZippedPNGs(QString fileName) {
    Exporter exporter(fileName, frames);
    exporter.saveZippedPNGs();
}

void Model::createFromFile(QString fileName) {
    QFile file(fileName);
    QString val;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    frames.clear();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject parsedDoc = doc.object();
    QJsonValue height = parsedDoc.value(QString("height"));
    QJsonValue width = parsedDoc.value(QString("width"));
    QJsonValue numFrames = parsedDoc.value(QString("numberOfFrames"));
    QJsonObject frameList = parsedDoc.value(QString("frames")).toObject();

    for(int frameId = 0; frameId < numFrames.toInt(); ++frameId) {
        QString frameString("frame");
        QString frameIdString(to_string(frameId).c_str());
        QString fullName = frameString + frameIdString;

        QJsonArray frameArr = frameList.value(fullName).toArray();
        Frame createdFrame(frameArr.size());
        for(int row = 0; row < frameArr.size(); ++row) {
            QJsonArray rowArr = frameArr.at(row).toArray();
            for(int pixel = 0; pixel < rowArr.size(); ++pixel) {
                QJsonArray colorArr = rowArr.at(pixel).toArray();
                int red = colorArr.at(0).toInt();
                int green = colorArr.at(1).toInt();
                int blue = colorArr.at(2).toInt();
                int alpha = colorArr.at(3).toInt();
                createdFrame.setPixel(pixel, row, QColor(red, green, blue, alpha));
            }
        }
        frames.push_back(createdFrame);
    }

    // Display the first frame
    frameIndex = 0;
    activeFrame = &frames.at(frameIndex);

    emit setNewFrame(activeFrame->getPixels());
}

void Model::saveFile(QString fileName) {
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QJsonDocument doc;
    QJsonObject mainObject;
    QJsonObject listOfFrames;

    mainObject.insert("height", activeFrame->getFrameHeight());
    mainObject.insert("width", activeFrame->getFrameWidth());
    mainObject.insert("numberOfFrames", static_cast<int>(frames.size()));


    for(size_t frameId = 0; frameId < frames.size(); ++frameId) {
        QJsonArray frameArr;
        QString frameString("frame");
        QString frameIdString(to_string(frameId).c_str());
        QString fullName = frameString + frameIdString;

        vector<vector<Pixel>> pixels = frames.at(frameId).getPixels();

        for(size_t y = 0; y < pixels.size(); ++y) {
            QJsonArray rowArr;
            vector<Pixel> row = pixels.at(y);
            for(size_t x = 0; x < row.size(); ++x) {
                QColor color = row.at(x).getColor();

                QJsonArray colorArr;
                colorArr.push_back(color.red());
                colorArr.push_back(color.green());
                colorArr.push_back(color.blue());
                colorArr.push_back(color.alpha());
                rowArr.push_back(colorArr);
            }
            frameArr.push_back(rowArr);
        }

        listOfFrames.insert(fullName, frameArr);
    }

    mainObject.insert("frames", listOfFrames);
    doc.setObject(mainObject);
    file.write(doc.toJson());
    file.close();
    modified = false;
}

void Model::maybeSave() {
    if(modified) {
        emit openWarningDialog();
    }
    else {
        qDebug() << "No changes to save" << endl;
        emit closeWindow();
    }
}

void Model::sizeChanged(int size){
    for(int index = 0; index < frames.size(); index++){
        frames[index].setSize(size);
    }
    activeFrame = &frames[0];

    emit redraw(activeFrame->getPixels());
}

void Model::makeFreshStacks() {
    stack<vector<vector<Pixel>>> freshStack;
    redolist = freshStack;
    undolist = freshStack;
    undolist.push(activeFrame->getPixels());
}

void Model::getFrame(int index){
    Frame currentFrame = frames[index];
    for(int y = 0; y < currentFrame.getFrameHeight(); y++){
        for(int x = 0; x < currentFrame.getFrameWidth(); x++){
            emit sendPreviewPixel(currentFrame.getColor(x,y), x, y);
        }
    }
}
