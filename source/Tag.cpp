//
// Created by leon on 19.05.15.
//

#include "Tag.h"

using namespace boost;
using namespace std;
using namespace deeplocalizer::tagger;

Tag::Tag(QString _imagepath, cv::Mat _subimage,
         cv::Rect boundingBox, optional<pipeline::Ellipse> ellipse) :
    _imagepath(_imagepath), _subimage(_subimage), _boundingBox(boundingBox),
    _ellipse(ellipse)
{
    _is_tag = _ellipse.is_initialized() && _ellipse.get().getVote() > 1500;
}

const QString &Tag::getImagepath() const {
    return _imagepath;
}

void Tag::setImagepath(const QString &imagepath) {
    this->_imagepath = imagepath;
}

cv::Rect Tag::getBoundingBox() const {
    return _boundingBox;
}

void Tag::setBoundingBox(cv::Rect rect) {
    _boundingBox = rect;
}

bool Tag::isTag() const {
    return this->_is_tag;
}

void Tag::toggleIsTag() {
    _is_tag = !_is_tag;
}
void Tag::setIsTag(bool isTag) {
    this->_is_tag = isTag;
}

const cv::Mat &Tag::getSubimage() const {
    return this->_subimage;
}

void Tag::setSubimage(const cv::Mat &subimage) {
    this->_subimage = subimage;
}

