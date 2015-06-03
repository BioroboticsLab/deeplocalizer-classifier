//
// Created by leon on 19.05.15.
//

#include "Tag.h"


namespace deeplocalizer {
namespace tagger {

using boost::optional;
using namespace std;

bool contains(const cv::Mat & m, const cv::Rect & rect) {
    return (rect.x >= 0 && rect.y >= 0 && rect.width >= 0 && rect.height >= 0 &&
            rect.x + rect.width < m.cols && rect.y + rect.height < m.rows);
}

const cv::Rect centerBoxAtEllipse(const cv::Rect & boundingBox,
                                  const cv::Mat & image ,
                                  const optional<pipeline::Ellipse> & ellipse) {
    if(ellipse.is_initialized()) {
        cv::Point2i center = ellipse.get().getCen();

        cv::Rect box(center.x - TAG_WIDTH / 2, TAG_WIDTH,
                     center.y - TAG_HEIGHT / 2, TAG_HEIGHT);
        if (contains(image, box)) {
            return box;
        }
    }
    return boundingBox;
}

std::atomic_long Tag::id_counter(0);

long Tag::generateId() {
    return Tag::id_counter.fetch_add(1, std::memory_order_relaxed);
}

Tag::Tag() {
    _id = Tag::generateId();
}

Tag::Tag(const pipeline::Tag & pipetag, const cv::Mat & image) {
    _id = Tag::generateId();
    boost::optional<pipeline::Ellipse> ellipse;
    for(auto candidate : pipetag.getCandidatesConst()) {
        if (!ellipse.is_initialized()) {
            ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
        } else if (ellipse.get() < candidate.getEllipse()) {
            ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
        }
    }
    _boundingBox = centerBoxAtEllipse(pipetag.getBox(), image,  ellipse);
    _ellipse = ellipse;
}

Tag::Tag(cv::Rect boundingBox, optional<pipeline::Ellipse> ellipse) :
        _boundingBox(boundingBox), _ellipse(ellipse)
{
    _id = Tag::generateId();
    _is_tag = _ellipse.is_initialized() && _ellipse.get().getVote() > 1500;
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

const cv::Mat Tag::getSubimage(const cv::Mat & orginal) const {
    return orginal(_boundingBox).clone();
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
}
}


