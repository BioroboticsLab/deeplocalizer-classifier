#ifndef DEEP_LOCALIZER_IMAGE_H
#define DEEP_LOCALIZER_IMAGE_H

#include <QPixmap>
#include <boost/optional.hpp>
#include <boost/serialization/list.hpp>
#include <boost/filesystem.hpp>

#include "Tag.h"
#include "serialization.h"

namespace deeplocalizer {
namespace  tagger {

class ImageDesc {
public:
    QString filename;
    ImageDesc();
    ImageDesc(const QString filename);
    ImageDesc(const QString filename, std::vector<Tag> _tags);
    QPixmap visualise_tags();
    void addTag(Tag&& tag);
    void setTags(std::vector<Tag> && tag);
    const std::vector<Tag> & getTags() const;
    std::vector<Tag> & getTags();
    bool operator==(const ImageDesc & other) const;
    static void saves(const std::string &path, const std::deque<ImageDesc> * imgs);
    static std::unique_ptr<std::deque<ImageDesc>> loads(const std::string &path);
    static std::vector<ImageDesc> fromPathFile(const std::string &path);
    static std::vector<ImageDesc> fromPathFile(const boost::filesystem::path&pathfile);
private:
    std::vector<Tag> tags;
    unsigned long current_tag = 0;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
      ar & BOOST_SERIALIZATION_NVP(filename);
      ar & BOOST_SERIALIZATION_NVP(tags);
    }
};
using ImageDescPtr = std::shared_ptr<ImageDesc>;
class Image {
public:
    explicit Image();
    explicit Image(const ImageDesc & descr);
    cv::Mat getCvMat() const;
    void addBorder();
    bool write(boost::filesystem::path path = {}) const;
    bool operator==(const Image & other) const;

private:
    cv::Mat _mat;
    QString _filename;
};

using ImagePtr = std::shared_ptr<Image>;
}
}

#endif //DEEP_LOCALIZER_IMAGE_H
