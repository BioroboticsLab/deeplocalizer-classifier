
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Image.h"

using namespace std;
using namespace boost;
using namespace deeplocalizer::tagger;

Image::Image() {

}

Image::Image(const QString _filename) : filename(_filename) {

}

Image::Image(const QString _filename, std::vector<Tag> _tags) :
        filename(_filename), tags(_tags) {

}

boost::optional <Tag> Image::nextTag() {
    current_tag++;
    if (current_tag >= this->tags.size()) {
        return boost::optional<Tag>();
    }
    return this->tags.at(current_tag);
}

QPixmap Image::visualise_tags() {
    return QPixmap();
}


void Image::load() {
    if(! this->img_mat) {
        this->img_mat = optional< cv::Mat >(cv::imread(this->filename.toUtf8().data()));
    }
}

void Image::unload() {
    if(this->img_mat) {
        (*img_mat).release();
    }
}

void Image::addTag(Tag && tag) {
    this->tags.push_back(tag);
}

void Image::setTags(std::vector<Tag> && tags) {
    this->tags = tags;
}

std::vector<Tag> & Image::getTags() {
    return this->tags;
}

cv::Mat Image::getCvMat() {
    if(! this->img_mat) {
        this->load();
    }
    return *(this->img_mat);
}
