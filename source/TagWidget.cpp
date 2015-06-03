
#include "TagWidget.h"

#include <QLayout>
#include <QStyle>
#include "opencv2/core/core.hpp"
#include "utils.h"

using namespace std;

namespace deeplocalizer {
namespace tagger {
TagWidget::TagWidget() {
    this->init();
}

TagWidget::TagWidget(QWidget *parent)
        : QWidget(parent)
{
    this->init();
}

void TagWidget::setTag(Tag * tag, const cv::Mat & img) {
    _tag = tag;
    cv::Rect box = tag->getBoundingBox();
    this->setFixedSize(box.width, box.height);
    this->redraw();
}

void TagWidget::paintEvent(QPaintEvent *) {
    _pixmap = cvMatToQPixmap(_mat);
    _painter.begin(this);
    _painter.scale(0.3, 0.3);
    _painter.drawPixmap(0, 0, _pixmap);
    _tag->draw(_painter);
    _painter.end();
}

void TagWidget::redraw() {
    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();
}

void TagWidget::init() {
    this->setAutoFillBackground(true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->layout()->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}


void TagWidget::mousePressEvent(QMouseEvent *) {
    if (this->clickable) {
        emit clicked();
    }
}

void TagWidget::toggleTag() {
    _tag->toggleIsTag();
    this->redraw();
}
}
}
