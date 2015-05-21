#ifndef DEEP_LOCALIZER_IMAGETAGWIDGET_H
#define DEEP_LOCALIZER_IMAGETAGWIDGET_H

#include <QObject>
#include <QLabel>
#include "opencv2/core/core.hpp"


class ImageTagWidget : public QLabel{
    Q_OBJECT

public:
    ImageTagWidget();
    ImageTagWidget(QWidget * parent);
    void setRandomImage(int height, int width);
    void loadFile(const QString &);
    void setCvMat(cv::Mat mat);
private:
    cv::Mat _img;
    void init();
};


#endif //DEEP_LOCALIZER_IMAGETAGWIDGET_H
