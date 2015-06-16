#include <boost/filesystem.hpp>
#include "TrainData.h"

namespace  deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

TrainData::TrainData(const std::string &image_filename, const Tag &tag, cv::Mat mat,
              cv::Point2i translation, double rotation_angle) :
        _original_image_filename(image_filename),
        _tag(tag),
        _mat(mat),
        _translation(translation),
        _rotation_angle(rotation_angle)
{
}

TrainData::TrainData(const std::string &image_filename, const Tag &tag,
                     cv::Mat mat) :
        _original_image_filename(image_filename),
        _tag(tag),
        _mat(mat),
        _translation(cv::Point2i(0, 0)),
        _rotation_angle(0)
{
}

const std::string TrainData::filename() const {
    io::path path(_original_image_filename);
    io::path extension = path.extension();
    path.replace_extension("");
    std::stringstream ss;
    cv::Rect bb = _tag.getBoundingBox();
    long angle = std::lround(_rotation_angle);
    ss << "_bx" << bb.x << "_by" << bb.y <<
            "_tx" << _translation.x << "_ty" << _translation.y <<
            "_a" << angle;
    return path.string() + ss.str() + extension.string();
}

}
}
