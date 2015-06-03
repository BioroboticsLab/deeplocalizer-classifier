
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/deque.hpp>
#include "serialization.h"

#include "Image.h"
#include "utils.h"

namespace deeplocalizer {
namespace tagger {

using namespace std;
namespace io = boost::filesystem;

ImageDescription::ImageDescription() {

}

ImageDescription::ImageDescription(const QString _filename) : filename(_filename) {

}

ImageDescription::ImageDescription(const QString _filename, std::vector<Tag> _tags) :
        filename(_filename), tags(_tags) {

}

std::vector<Tag> & ImageDescription::getTags() {
    return tags;
}

QPixmap ImageDescription::visualise_tags() {
    return QPixmap();
}

void ImageDescription::addTag(Tag && tag) {
    this->tags.push_back(tag);
}

void ImageDescription::setTags(std::vector<Tag> && tags) {
    this->tags = tags;
}


bool ImageDescription::operator==(const ImageDescription & other) const {
    return (filename == other.filename &&
            tags == other.tags);
}

std::vector<ImageDescription> ImageDescription::fromPathFile(const std::string &path) {
    return ImageDescription::fromPathFile(io::path(path));
}
std::vector<ImageDescription> ImageDescription::fromPathFile(
        const io::path& pathfile) {
    ASSERT(io::exists(pathfile), "File " << pathfile << " does not exists.");
    ifstream ifs{pathfile.string()};
    std::string path_to_image;
    std::vector<ImageDescription> descs;
    for(int i = 0; std::getline(ifs, path_to_image); i++) {
        ASSERT(io::exists(path_to_image), "File " << path_to_image << " does not exists.");
        descs.push_back(ImageDescription{QString::fromStdString(path_to_image)});
    }

    return descs;
}


void ImageDescription::saves(const std::string & path, const std::deque<ImageDescription> * imgs) {
    std::ofstream os(path);
    boost::archive::binary_oarchive archive(os);
    archive << boost::serialization::make_nvp("images", imgs);
}

std::unique_ptr<std::deque<ImageDescription>> ImageDescription::loads(const std::string & path) {
    std::ifstream is(path);
    boost::archive::binary_iarchive archive(is);
    std::deque<ImageDescription> * imgs;
    archive >> boost::serialization::make_nvp("images", imgs);
    return std::unique_ptr<std::deque<ImageDescription>>(imgs);
}

Image::Image() {
}

Image::Image(const ImageDescription & descr) : _filename(descr.filename)  {
    _mat = cv::imread(_filename.toStdString());
}

void Image::addBorder() {
    auto mat_with_border = cv::Mat(_mat.rows + TAG_HEIGHT,
                                   _mat.cols + TAG_WIDTH, CV_8U);
    cv::copyMakeBorder(_mat, mat_with_border, TAG_HEIGHT / 2, TAG_HEIGHT / 2,
                       TAG_WIDTH / 2, TAG_WIDTH / 2, cv::BORDER_REPLICATE);
    this->_mat.release();
    this->_mat = mat_with_border;
}

cv::Mat Image::getCvMat() const {
    return _mat;
}

bool Image::write(io::path path) const {
    if (path.empty()) {
        return cv::imwrite(_filename.toStdString(), _mat);
    } else {
        return cv::imwrite(path.string(), _mat);
    }
}

bool Image::operator==(const Image &other) const {
    if(_filename != other._filename) {
        return false;
    }
    cv::Mat & m = const_cast<cv::Mat &>(_mat);
    cv::Mat & o = const_cast<cv::Mat &>(other._mat);

    if (m.empty() && o.empty()) {
        return true;
    }
    if (m.cols != o.cols || m.rows != o.rows ||
            m.dims != o.dims) {
        return false;
    }
    return std::equal(m.begin<uchar>(), m.end<uchar>(), o.begin<uchar>(), o.end<uchar>());
}
}
}
