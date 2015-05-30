//
// Created by leon on 19.05.15.
//

#include "Tag.h"

using boost::optional;
using namespace std;
using namespace deeplocalizer::tagger;

Tag::Tag() {

}

Tag::Tag(cv::Rect boundingBox, optional<pipeline::Ellipse> ellipse) :
        _boundingBox(boundingBox), _ellipse(ellipse)
{
    _is_tag = _ellipse.is_initialized() && _ellipse.get().getVote() > 1500;
}

const optional<pipeline::Ellipse> & Tag::getEllipse () const {
    return _ellipse;
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

bool Tag::operator==(const Tag &other) const {
    if (_ellipse.is_initialized() && other._ellipse.is_initialized()) {
        auto te = _ellipse.get();
        auto oe = other._ellipse.get();
        return (_boundingBox == other._boundingBox &&
                _is_tag == other._is_tag &&
                te.getAngle() == oe.getAngle() &&
                te.getVote() == oe.getVote() &&
                te.getAxis() == oe.getAxis() &&
                te.getCen() == oe.getCen());
    }
    return _ellipse.is_initialized() == other._ellipse.is_initialized();
}

void Tag::setIsTag(bool isTag) {
    this->_is_tag = isTag;
}

const cv::Mat Tag::getSubimage(const cv::Mat & orginal) const {
    return orginal(_boundingBox).clone();
}
