
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Image.h"

using namespace std;
using namespace deeplocalizer::tagger;

Image::Image() {

}

Image::Image(const QString _filename) : filename(_filename) {

}

Image::Image(const QString _filename, vector< shared_ptr< Tag > > _tags) :
        filename(_filename), tags(_tags) {

}

boost::optional< shared_ptr< Tag > > Image::nextTag() {
    if (current_tag + 1 >= this->tags.size() ) {
        return boost::optional< shared_ptr< Tag > >();
    }
    return this->tags.at(current_tag++);
}

QPixmap Image::visualise_tags() {
    return QPixmap();
}


void Image::load() {
    if(! this->img_mat) {
        auto mat = cv::imread(this->filename.toStdString());
        auto mat_with_border = cv::Mat(mat.rows, mat.cols, CV_8U);
        cv::copyMakeBorder(mat, mat_with_border, TAG_HEIGHT, TAG_HEIGHT,
                       TAG_WIDTH, TAG_WIDTH, cv::BORDER_REPLICATE);
        this->img_mat = boost::optional< cv::Mat >(mat_with_border);
    }
}

void Image::unload() {
    if(this->img_mat) {
        this->img_mat.get().release();
    }
}

void Image::addTag(Tag && tag) {
    this->tags.push_back(std::make_shared<Tag>(tag));
}

void Image::setTags(std::vector<shared_ptr<Tag>> && tags) {
    this->tags = tags;
}

const std::vector<shared_ptr<Tag>> & Image::getTags() {
    return this->tags;
}

cv::Mat Image::getCvMat() {
    if(! this->img_mat) {
        this->load();
    }
    return *(this->img_mat);
}
