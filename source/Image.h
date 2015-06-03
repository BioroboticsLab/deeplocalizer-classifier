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

class ImageDescription {
public:
    QString filename;
    ImageDescription();
    ImageDescription(const QString filename);
    ImageDescription(const QString filename, std::vector<Tag> _tags);
    QPixmap visualise_tags();
    void addTag(Tag&& tag);
    void setTags(std::vector<Tag> && tag);
    std::vector<Tag> & getTags();
    bool operator==(const ImageDescription & other) const;
    static void saves(const std::string &path, const std::deque<ImageDescription> * imgs);
    static std::unique_ptr<std::deque<ImageDescription>> loads(const std::string &path);
    static std::vector<ImageDescription> fromPathFile(const std::string &path);
    static std::vector<ImageDescription> fromPathFile(const boost::filesystem::path&pathfile);
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

class Image {
public:
    explicit Image();
    explicit Image(const ImageDescription & descr);
    cv::Mat getCvMat() const;
    void addBorder();
    bool write(boost::filesystem::path path = {}) const;

private:
    cv::Mat _mat;
    QString _filename;
};

}
}

#endif //DEEP_LOCALIZER_IMAGE_H
