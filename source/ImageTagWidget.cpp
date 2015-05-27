
#include <QStyle>
#include "opencv2/core/core.hpp"

#include "QtHelper.h"
#include "ImageTagWidget.h"

using namespace std;
using namespace deeplocalizer::tagger;

ImageTagWidget::ImageTagWidget() {
    this->init();
}

ImageTagWidget::ImageTagWidget(QWidget *parent)
        : QLabel(parent)
{
    this->init();
}
void ImageTagWidget::setRandomImage(int height, int width) {
    cv::Mat rand_img = cv::Mat(height, width, CV_8UC3);
    cv::randu(rand_img, cv::Scalar::all(0), cv::Scalar::all(255));
    this->setPixmap(cvMatToQPixmap(rand_img));
}

void ImageTagWidget::loadFile(const QString &filename) {
    QPixmap pixmap(filename);
    this->setPixmap(pixmap);
}

void ImageTagWidget::setTag(shared_ptr<Tag> tag) {
    _tag = tag;
    this->setPixmap(cvMatToQPixmap(tag->getSubimage()));
    cv::Rect box = tag->getBoundingBox();
    this->setFixedSize(box.width, box.height);
    this->redraw();
}
void ImageTagWidget::redraw() {
    if (_tag->isTag()) {
        this->setProperty("is_tag", "yes");
    } else {
        this->setProperty("is_tag", "no");
    }
    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();

}
void ImageTagWidget::init() {
    this->setAutoFillBackground(true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setStyleSheet(
            "ImageTagWidget { border: 5px solid yellow}\n"
            "ImageTagWidget[is_tag=\"no\"]{ border-color: red}\n"
            "ImageTagWidget[is_tag=\"yes\"]{ border-color: green}"
    );
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}


void ImageTagWidget::mousePressEvent(QMouseEvent *event) {
    if (this->clickable) {
        emit clicked();
    }
}

void ImageTagWidget::toggleTag() {
    _tag->toggleIsTag();
}
