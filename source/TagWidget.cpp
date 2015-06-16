
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

void TagWidget::init() {
    this->setAutoFillBackground(true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFixedSize(sizeHint());
}
void TagWidget::setTag(Tag * tag, cv::Mat mat) {
    _tag = tag;
    _mat = mat;
    _pixmap = cvMatToQPixmap(_mat);
    setFixedSize(sizeHint());
    this->repaint();
}

QSize TagWidget::sizeHint() const {
    if (_tag) {
        auto box = _tag->getBoundingBox();
        return QSize(box.width + 2*_border, box.height + 2*_border);
    } else {
        return QSize(TAG_WIDTH + 2*_border, TAG_HEIGHT + 2*_border);
    }
}

void TagWidget::paintEvent(QPaintEvent *) {
    static const int lineWidth = 1;

    _painter.begin(this);
    _painter.drawPixmap(0, 0, _pixmap);
    if(_tag != nullptr) {
        auto box = _tag->getBoundingBox();
        _painter.save();
        _painter.translate(QPoint(-box.x + _border, -box.y + _border));
        _tag->draw(_painter, lineWidth);
        _painter.restore();
    }
    _painter.end();
}

void TagWidget::mousePressEvent(QMouseEvent *) {
    if (this->clickable) {
        emit clicked();
    }
}

void TagWidget::toggleTag() {
    _tag->toggleIsTag();
    this->repaint();
}

void TagWidget::setBorder(unsigned int border) {
    _border = border;
}

unsigned int TagWidget::border() {
    return _border;
}
}
}
