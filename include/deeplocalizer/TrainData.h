
#ifndef DEEP_LOCALIZER_TRAINDATA_H
#define DEEP_LOCALIZER_TRAINDATA_H

#include "Tag.h"
#include "Image.h"

namespace deeplocalizer {
namespace tagger {

class TrainData {
public:
    TrainData(const std::string & image_filename, const Tag &tag,
              cv::Point2i translation, double rotation_angle,
              cv::Mat mat);

    TrainData(const std::string & image_filename, const Tag &tag,
              cv::Mat mat);

    const std::string filename() const ;

    const Tag & tag() const {
        return _tag;
    };
    const cv::Point2i & translation() const {
        return _translation;
    };
    double rotation_angle() const {
        return _rotation_angle;
    };
    const cv::Mat & mat() const {
        return _mat;
    };
private:
    const std::string _original_image_filename;
    const Tag _tag;
    const cv::Point2i _translation;

    const double _rotation_angle;
    const cv::Mat _mat;
};
}
}

#endif //DEEP_LOCALIZER_TRAINDATA_H
