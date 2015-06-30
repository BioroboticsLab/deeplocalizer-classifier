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


class TagWidget : public QWidget {
    Q_OBJECT

public:
    bool clickable = true;
    TagWidget(QWidget * parent, Tag & tag, cv::Mat mat);
    void setBorder(unsigned int border);
    unsigned int border();
    static const unsigned int DEFAULT_BORDER = 16;
public slots:
    void toggleTag();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const;
private:
    cv::Mat _mat;
    Tag & _tag;
    unsigned int _border = DEFAULT_BORDER;
    QPainter _painter;
    QPixmap _pixmap;
    void init();
};
using TagWidgetPtr = std::shared_ptr<TagWidget>;
}

#endif //DEEP_LOCALIZER_IMAGETAGWIDGET_H
