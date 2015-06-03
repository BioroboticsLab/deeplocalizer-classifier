//
// Created by leon on 19.05.15.
//

#include "Tag.h"

#include <QPainter>
#include <mutex>

namespace deeplocalizer {
namespace tagger {

using boost::optional;


cv::Rect centerBoxAtEllipse(const cv::Rect & bb,
                            const pipeline::Ellipse & ellipse) {
    cv::Point2i center = ellipse.getCen();
    cv::Rect box(bb.x + center.x - TAG_WIDTH / 2,
                 bb.y + center.y - TAG_HEIGHT / 2,
                 TAG_WIDTH, TAG_HEIGHT);
    return box;
}

std::atomic_long Tag::id_counter(0);

unsigned long Tag::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<unsigned long> dis(0, ULONG_MAX);
    static std::mutex m;
    m.lock();
    unsigned  long id = dis(gen);
    m.unlock();
    return id;
}

Tag::Tag() {
    _id = Tag::generateId();
}

Tag::Tag(const pipeline::Tag & pipetag) {
    _id = Tag::generateId();
    boost::optional<pipeline::Ellipse> ellipse;
    for(auto candidate : pipetag.getCandidatesConst()) {
        if (!ellipse.is_initialized()) {
            ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
        } else if (ellipse.get() < candidate.getEllipse()) {
            ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
        }
    }
    if(ellipse) {
        _boundingBox = centerBoxAtEllipse(pipetag.getBox(), ellipse.get());
        _ellipse = ellipse;
        _ellipse.get().setCen(cv::Point2i(TAG_WIDTH/2, TAG_HEIGHT/2));
    } else {
        _boundingBox = pipetag.getBox();
    }
}

Tag::Tag(cv::Rect boundingBox, optional<pipeline::Ellipse> ellipse) :
        _boundingBox(boundingBox), _ellipse(ellipse)
{
}

void Tag::guessIsTag(int threshold) {
    _is_tag = _ellipse.is_initialized() && _ellipse.get().getVote() > threshold;
}
const optional<pipeline::Ellipse> & Tag::getEllipse () const {
    return _ellipse;
}
const cv::Rect & Tag::getBoundingBox() const {
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

cv::Mat Tag::getSubimage(const cv::Mat & orginal, unsigned int border) const {
    cv::Rect box = _boundingBox;
    box.x -= border;
    box.y -= border;
    box.width += 2*border;
    box.height+= 2*border;
    return orginal(box).clone();
}
void Tag::draw(QPainter & p) {
    auto bb = _boundingBox;
    if (_is_tag) {
        p.setPen(QPen(Qt::green, 3));
    } else {
        p.setPen(QPen(Qt::red, 3));
    }
    p.drawRect(QRect(bb.x, bb.y, bb.height, bb.width));
    if (_ellipse) {
        auto e = _ellipse.get();
        p.setPen(Qt::blue);
        auto t = p.worldTransform();
        QPoint center(bb.x+e.getCen().x, bb.y+e.getCen().y);
        p.translate(center);
        p.rotate(e.getAngle());
        p.drawPoint(QPoint(0, 0));
        p.drawEllipse(QPoint(0, 0), e.getAxis().width, e.getAxis().height);
        p.setWorldTransform(t);
    }
}
long Tag::getId() const {
    return _id;
}
}
}


