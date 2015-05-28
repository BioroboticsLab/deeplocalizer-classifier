
#ifndef DEEP_LOCALIZER_TAG_H
#define DEEP_LOCALIZER_TAG_H

#include <QString>
#include <opencv2/core/core.hpp>
#include "pipeline/datastructure/Ellipse.h"

#include "pipeline/datastructure/serialization.hpp"
#include "serialization.h"

namespace deeplocalizer {
namespace  tagger {

const int TAG_WIDTH = 128;
const int TAG_HEIGHT = 128;

class Tag {
public:
    Tag(QString imagepath, cv::Mat subimage, cv::Rect boundingBox,
        boost::optional<pipeline::Ellipse> ellipse);
    const QString &getImagepath() const;
    void setImagepath(const QString &imagepath);
    cv::Rect getBoundingBox() const;
    void setBoundingBox(cv::Rect boundingBox);
    bool isTag() const;
    void setIsTag(bool is_tag);
    void toggleIsTag();
    const cv::Mat &getSubimage() const;
    void setSubimage(const cv::Mat &subimage);

    bool operator==(const Tag &other) const;

private:
    QString _imagepath;
    cv::Mat _subimage;
    cv::Rect _boundingBox;
    boost::optional<pipeline::Ellipse> _ellipse;
    bool _is_tag;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_NVP(_imagepath);
        ar & BOOST_SERIALIZATION_NVP(_boundingBox);
        ar & BOOST_SERIALIZATION_NVP(_ellipse);
        ar & BOOST_SERIALIZATION_NVP(_is_tag);
    }
};
}
}

#endif //DEEP_LOCALIZER_TAG_H
