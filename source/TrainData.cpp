#include <QPainter>
#include <boost/filesystem.hpp>
#include <utils.h>
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

    std::string right_or_wrong;
    if(_tag.isYes()) {
        right_or_wrong = "_right";
    } else {
        right_or_wrong = "_wrong";
    }
    ss << "_bx" << bb.x << "_by" << bb.y <<
            "_tx" << _translation.x << "_ty" << _translation.y <<
            "_a" << angle << right_or_wrong;
    return path.string() + ss.str() + extension.string();
}

void TrainData::draw(QPainter &painter) const {
    static const int line_width = 1;
    painter.save();
    painter.translate(_translation.x, _translation.y);
    _tag.draw(painter, line_width);
    painter.restore();
}
caffe::Datum TrainData::toCaffe() const {
    caffe::Datum datum;
    datum.set_label(_tag.isYes());
    ASSERT(_mat.cols == TAG_WIDTH, "Expected _mat.cols to be equal " << TAG_WIDTH);
    datum.set_width(_mat.cols);
    ASSERT(_mat.rows == TAG_HEIGHT, "Expected _mat.rows to be equal " << TAG_HEIGHT);
    datum.set_height(_mat.rows);
    datum.set_channels(1);
    datum.set_data(_mat.data, _mat.rows * _mat.cols);
    return datum;
}
}
}
