
#include "opencv2/core/core.hpp"
#include "ImageTagWidget.h"
#include "QtHelper.h"

using namespace deeplocalizer::tagger;

ImageTagWidget::ImageTagWidget() {
    this->init();
}

ImageTagWidget::ImageTagWidget(QWidget *parent) : QLabel(parent)
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

void ImageTagWidget::setCvMat(cv::Mat mat) {
    this->setPixmap(cvMatToQPixmap(mat));
}

void ImageTagWidget::init() {
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setAutoFillBackground(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setStyleSheet(
            "ImageTagWidget { border: 5px solid yellow}\n"
            "ImageTagWidget[is_tag=\"no\"]{ border-color: red}\n"
            "ImageTagWidget[is_tag=\"yes\"]{ border-color: green}"
    );
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setRandomImage(40, 40);
}
