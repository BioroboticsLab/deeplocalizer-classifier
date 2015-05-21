
#ifndef DEEP_LOCALIZER_TAG_H
#define DEEP_LOCALIZER_TAG_H

#include <QString>
#include <cereal/types/memory.hpp>
#include <opencv2/core/core.hpp>

namespace deeplocalizer {
namespace  tagger {

class Tag {
public:
    Tag(QString imagepath, cv::Mat subimage, int x=0, int y=0);
    const QString &getImagepath() const;
    void setImagepath(const QString &imagepath);
    int getX() const;
    void setX(int x);
    int getY() const;
    void setY(int y);
    bool isTag() const;
    void setIsTag(bool is_tag);
    const cv::Mat &getSubimage() const;
    void setSubimage(const cv::Mat &subimage);


private:
    QString imagepath;
    cv::Mat subimage;
    int x;
    int y;
    bool is_tag;


    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar(imagepath, x, y, is_tag);
    }
};
}
}

#endif //DEEP_LOCALIZER_TAG_H
