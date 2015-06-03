
#ifndef DEEP_LOCALIZER_TAG_H
#define DEEP_LOCALIZER_TAG_H

#include <atomic>
#include <QString>
#include <opencv2/core/core.hpp>
#include <pipeline/datastructure/Tag.h>
#include "pipeline/datastructure/Ellipse.h"

#include "pipeline/datastructure/serialization.hpp"
#include "serialization.h"

class QPainter;

namespace deeplocalizer {
namespace  tagger {

const int TAG_WIDTH = 96;
const int TAG_HEIGHT = 96;

class Tag {
public:
    Tag();
    Tag(const pipeline::Tag & pipetag, const cv::Mat & image);
    Tag(cv::Rect boundingBox, boost::optional<pipeline::Ellipse> ellipse);

    long getId() const;
    const cv::Rect & getBoundingBox() const;
    void setBoundingBox(cv::Rect boundingBox);

    const boost::optional<pipeline::Ellipse> & getEllipse () const;

    bool isTag() const;
    void setIsTag(bool is_tag);
    void toggleIsTag();

    const cv::Mat getSubimage(const cv::Mat &orginal) const;
    bool operator==(const Tag &other) const;
    void guessIsTag(int threshold);
    void draw(QPainter & p);

private:
    unsigned long _id;
    cv::Rect _boundingBox;
    boost::optional<pipeline::Ellipse> _ellipse;
    bool _is_tag;


    static unsigned long generateId();
    static std::atomic_long id_counter;
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(_boundingBox);
        ar & BOOST_SERIALIZATION_NVP(_ellipse);
        ar & BOOST_SERIALIZATION_NVP(_is_tag);
    }
    static const int IS_TAG_THRESHOLD = 1600;
};
}
}

#endif //DEEP_LOCALIZER_TAG_H
