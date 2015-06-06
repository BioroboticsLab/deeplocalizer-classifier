
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/deque.hpp>
#include "serialization.h"

#include "Image.h"
#include "utils.h"

namespace deeplocalizer {
namespace tagger {

using namespace std;
namespace io = boost::filesystem;

ImageDesc::ImageDesc() {

}

ImageDesc::ImageDesc(const QString _filename) : filename(_filename) {

}

ImageDesc::ImageDesc(const QString _filename, std::vector<Tag> _tags) :
        filename(_filename), tags(_tags) {

}

const std::vector<Tag> &ImageDesc::getTags() const {
    return tags;
}
std::vector<Tag> &ImageDesc::getTags() {
    return tags;
}

QPixmap ImageDesc::visualise_tags() {
    return QPixmap();
}

void ImageDesc::addTag(Tag && tag) {
    this->tags.push_back(tag);
}

void ImageDesc::setTags(std::vector<Tag> && tags) {
    this->tags = tags;
}


bool ImageDesc::operator==(const ImageDesc & other) const {
    return (filename == other.filename &&
            tags == other.tags);
}

std::vector<ImageDesc> ImageDesc::fromPathFile(const std::string &path) {
    return ImageDesc::fromPathFile(io::path(path));
}

std::vector<ImageDesc> ImageDesc::fromPathFile(
        const io::path& pathfile) {
    ASSERT(io::exists(pathfile), "File " << pathfile << " does not exists.");
    ifstream ifs{pathfile.string()};
    std::string path_to_image;
    std::vector<QString> paths;
    for(int i = 0; std::getline(ifs, path_to_image); i++) {
        ASSERT(io::exists(path_to_image), "File " << path_to_image << " does not exists.");
        paths.emplace_back(QString::fromStdString(path_to_image));
    }
    return fromPaths(paths);
}

std::vector<ImageDesc> ImageDesc::fromPaths(const std::vector<QString> paths)  {
    std::vector<ImageDesc> descs;
    for(auto & path: paths) {
        ASSERT(io::exists(path.toStdString()), "File " << path.toStdString() << " does not exists.");
        auto desc = ImageDesc(path);
        if(io::exists(desc.save_path())) {
            desc = *ImageDesc::load(desc.save_path());
        }
        descs.push_back(desc);
    }
    return descs;
}
std::string ImageDesc::save_path() const {
    std::string str = filename.toStdString();
    str.append(".desc");
    return str;
}
void ImageDesc::save() {
    save(save_path());
}
void ImageDesc::save(const std::string & path) {
    std::ofstream os(path);
    boost::archive::binary_oarchive archive(os);
    archive << boost::serialization::make_nvp("image_desc", *this);
}

ImageDescPtr ImageDesc::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::binary_iarchive archive(is);
    ImageDesc img_desc;
    archive >> boost::serialization::make_nvp("image_desc", img_desc);
    return std::make_shared<ImageDesc>(std::move(img_desc));
}

Image::Image() {
}

Image::Image(const ImageDesc & descr) : _filename(descr.filename)  {
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
