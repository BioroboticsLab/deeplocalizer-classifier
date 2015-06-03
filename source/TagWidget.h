#ifndef DEEP_LOCALIZER_IMAGETAGWIDGET_H
#define DEEP_LOCALIZER_IMAGETAGWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include "opencv2/core/core.hpp"

#include "Tag.h"
#include "Image.h"

namespace deeplocalizer {
namespace tagger {

class TagWidget : public QWidget {
    Q_OBJECT

public:
    bool clickable = true;
    TagWidget();
    TagWidget(QWidget * parent);
    void setRandomImage(int height, int width);
    void loadFile(const QString &);
    void setTag(Tag * tag, const cv::Mat & img);
public slots:
    void toggleTag();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *);
private:
    cv::Mat _mat;
    deeplocalizer::tagger::Tag * _tag;
    QPainter _painter;
    QPixmap _pixmap;
    void init();
    void redraw();
};

}
}

#endif //DEEP_LOCALIZER_IMAGETAGWIDGET_H
