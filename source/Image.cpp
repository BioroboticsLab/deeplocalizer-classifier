
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Image.h"
#include "utils.h"

using namespace std;
using namespace deeplocalizer::tagger;
namespace io = boost::filesystem;

ImageDescription::ImageDescription() {

}

ImageDescription::ImageDescription(const QString _filename) : filename(_filename) {

}

ImageDescription::ImageDescription(const QString _filename, std::vector<Tag> _tags) :
        filename(_filename), tags(_tags) {

}

const std::vector<Tag> & ImageDescription::getTags() const {
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
Image::Image() {
}

Image::Image(const ImageDescription & descr) : _filename(descr.filename)  {
    _mat = cv::imread(_filename.toStdString());
}

void Image::addBorder() {
    auto mat_with_border = cv::Mat(_mat.rows + 2*TAG_HEIGHT,
                                   _mat.cols + 2*TAG_WIDTH, CV_8U);
    cv::copyMakeBorder(_mat, mat_with_border, TAG_HEIGHT, TAG_HEIGHT,
                       TAG_WIDTH, TAG_WIDTH, cv::BORDER_REPLICATE);
    this->_mat.release();
    this->_mat = mat_with_border;
}

cv::Mat Image::getCvMat() const {
    return _mat;
}

