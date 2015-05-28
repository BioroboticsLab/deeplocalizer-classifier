#ifndef DEEP_LOCALIZER_IMAGE_H
#define DEEP_LOCALIZER_IMAGE_H

#include <QPixmap>
#include <boost/optional.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include "Tag.h"
#include "serialization.h"

namespace deeplocalizer {
namespace  tagger {

class Image {
public:
    QString filename;
    Image();
    Image(const QString filename);
    Image(const QString filename, std::vector<std::shared_ptr<Tag>> _tags);
    void load();
    void unload();
    boost::optional< std::shared_ptr<Tag>> nextTag();
    QPixmap visualise_tags();
    void addTag(Tag&& tag);
    void setTags(std::vector< std::shared_ptr<Tag>> && tag);
    const std::vector<std::shared_ptr<Tag>> & getTags();
    cv::Mat getCvMat();
    bool operator==(const Image & other) const;
private:
    boost::optional<cv::Mat> img_mat;
    std::vector< std::shared_ptr< Tag > > tags;
    unsigned long current_tag = 0;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
      ar & BOOST_SERIALIZATION_NVP(filename);
      ar & BOOST_SERIALIZATION_NVP(tags);
    }
};

}
}

#endif //DEEP_LOCALIZER_IMAGE_H
