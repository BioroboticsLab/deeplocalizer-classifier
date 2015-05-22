
#ifndef DEEP_LOCALIZER_TAG_H
#define DEEP_LOCALIZER_TAG_H

#include <QString>
#include <cereal/types/memory.hpp>
#include <opencv2/core/core.hpp>

#include "pipeline/datastructure/Ellipse.h"

namespace deeplocalizer {
namespace  tagger {

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
    const cv::Mat &getSubimage() const;
    void setSubimage(const cv::Mat &subimage);


private:
    QString _imagepath;
    cv::Mat _subimage;
    cv::Rect _boundingBox;
    boost::optional<pipeline::Ellipse> _ellipse;

    bool _is_tag;


    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(_imagepath),
           CEREAL_NVP(_boundingBox),
           CEREAL_NVP(_ellipse),
           CEREAL_NVP(_is_tag));
    }
};
}
}

#endif //DEEP_LOCALIZER_TAG_H
