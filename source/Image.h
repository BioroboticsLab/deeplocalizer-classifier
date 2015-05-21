#ifndef DEEP_LOCALIZER_IMAGE_H
#define DEEP_LOCALIZER_IMAGE_H

#include <QPixmap>
#include <boost/optional.hpp>

#include "Tag.h"

namespace deeplocalizer {
namespace  tagger {

class Image {
public:
    const QString filename;
    Image();
    Image(const QString filename);
    Image(const QString filename, std::vector<Tag> _tags);
    void load();
    void unload();
    boost::optional<Tag> nextTag();
    QPixmap visualise_tags();
    void addTag(Tag&& tag);
    void setTags(std::vector<Tag>&& tag);
    std::vector<Tag>& getTags();
    cv::Mat getCvMat();
private:
    boost::optional<cv::Mat> img_mat;
    std::vector<Tag> tags;
    unsigned long current_tag = 0;
};

}
}

#endif //DEEP_LOCALIZER_IMAGE_H
