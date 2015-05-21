//
// Created by leon on 19.05.15.
//

#include "Tag.h"

using namespace deeplocalizer::tagger;

Tag::Tag(QString _imagepath, cv::Mat _subimage, int _x, int _y) :
    imagepath(_imagepath), subimage(_subimage), x(_x), y(_y), is_tag(true)
{

}

const QString &Tag::getImagepath() const {
    return imagepath;
}

void Tag::setImagepath(const QString &imagepath) {
    this->imagepath = imagepath;
}

int Tag::getX() const {
    return x;
}

void Tag::setX(int x) {
    this->x = x;
}

int Tag::getY() const {
    return y;
}

void Tag::setY(int y) {
    this->y = y;
}

bool Tag::isTag() const {
    return this->is_tag;
}

void Tag::setIsTag(bool isTag) {
    this->is_tag = isTag;
}

const cv::Mat &Tag::getSubimage() const {
    return this->subimage;
}

void Tag::setSubimage(const cv::Mat &subimage) {
    this->subimage = subimage;
}

