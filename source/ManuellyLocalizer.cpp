
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include "pipeline/Localizer.h"
#include "ManuellyLocalizer.h"

using namespace std;
using namespace deep_localizer::manually;

ManuellyLocalizer::ManuellyLocalizer(vector<string> &_image_paths) {
    this->image_paths = _image_paths;
    for(auto path = begin(this->image_paths);
        path!=end(this->image_paths); path++) {
        if(! boost::filesystem::exists(*path)) {
            throw std::runtime_error("Could not open file");
        }
    }
    this->current_image_idx = 0;
    this->load_image(this->image_paths[0]);
}

const cv::Mat& ManuellyLocalizer::getCurrentImage() const {
    return this->current_image;
}

const vector<string> ManuellyLocalizer::getImagePaths() const {
    return this->image_paths;
}

const string ManuellyLocalizer::getCurrentImagePath() const {
    return this->image_paths[this->current_image_idx];
}

const boost::optional<pipeline::Tag> ManuellyLocalizer::next_tag() {
    while(this->current_tag_idx >= current_tags.size())
    {
        if(!this->load_next_image()) {
            return boost::optional<pipeline::Tag>();
        }
    }
    return boost::optional<pipeline::Tag>(current_tags[this->current_tag_idx++]);
}

void ManuellyLocalizer::load_image(std::string& image_path) {
    cout << "Loading next image: " << image_path << endl;

    if(! boost::filesystem::exists(image_path)) {
        throw std::runtime_error("Could not open file: `" + image_path + "`");
    }
    this->current_image = cv::imread(image_path);
    this->current_tags = this->getTagsProposal(this->current_image);
    this->current_tag_idx = 0;
}

bool ManuellyLocalizer::load_next_image() {
    this->current_image_idx++;
    if(this->image_paths.size() <= this->current_image_idx) {
        return false;
    }
    this->current_image_path = this->getCurrentImagePath();
    cout << "Loading next image: " << this->getCurrentImagePath() << endl;
    this->load_image(this->current_image_path);
    return true;
}

std::vector<pipeline::Tag> ManuellyLocalizer::getTagsProposal(cv::Mat image) {
    cv::Mat preprocced = this->preprocessor.process(image);
    return this->localizer.process(std::move(image), std::move(preprocced));
}

void ManuellyLocalizer::displayTags() {
    cv::Mat highlightedTagsImg = this->localizer.highlightTags(this->getCurrentImage());
    cv::imwrite( "highlightedTags.jpeg", highlightedTagsImg);
}
