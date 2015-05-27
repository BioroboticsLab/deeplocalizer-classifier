#ifndef DEEP_LOCALIZER_IMAGETAGWIDGET_H
#define DEEP_LOCALIZER_IMAGETAGWIDGET_H

#include <QObject>
#include <QLabel>
#include "opencv2/core/core.hpp"

#include "Tag.h"

class ImageTagWidget : public QLabel{
    Q_OBJECT

public:
    bool clickable = true;
    ImageTagWidget();
    ImageTagWidget(QWidget * parent);
    void setRandomImage(int height, int width);
    void loadFile(const QString &);
    void setTag(std::shared_ptr<deeplocalizer::tagger::Tag> tag);
public slots:
    void toggleTag();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent * event);
private:
    cv::Mat _img;
    std::shared_ptr<deeplocalizer::tagger::Tag> _tag;
    void init();
    void redraw();
};


#endif //DEEP_LOCALIZER_IMAGETAGWIDGET_H
