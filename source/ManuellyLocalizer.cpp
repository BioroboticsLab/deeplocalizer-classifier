
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include "pipeline/Localizer.h"

#include "ManuellyLocalizer.h"

using namespace std;
using namespace boost;
using namespace deeplocalizer::tagger;

ManuellyLocalizer::ManuellyLocalizer() {
    this->current_image_idx = 0;
    this->current_image_path = "no image selected";
}

ManuellyLocalizer::ManuellyLocalizer(vector<QString> &_image_paths) {
    this->image_paths = _image_paths;
    for(auto path : image_paths) {
        if(! filesystem::exists(path.toUtf8().data())) {
            throw std::runtime_error("Could not open file");
        }
        Image img(path);
        this->images.insert({path, img});
    }
    this->current_image_idx = 0;
    this->current_image_path = this->getCurrentImagePath();
    this->load_image(this->current_image_path);
}

Image& ManuellyLocalizer::getCurrentImage() {
    auto found = this->images.find(this->current_image_path);
    if (found == this->images.end()) {
        throw std::runtime_error("cannot access current image");
    }
    return found->second;
}

const vector<QString> ManuellyLocalizer::getImagePaths() const {
    return this->image_paths;
}

const QString ManuellyLocalizer::getCurrentImagePath() const {
    return this->image_paths[this->current_image_idx];
}

const optional<Image &> ManuellyLocalizer::nextImage() {
    bool first_image = this->current_image_idx == 0;
    if(! first_image ) {
        if (!this->load_next_image()) {
            return optional<Image &>();
        }
    }
    return optional<Image &>(this->getCurrentImage());
}

void ManuellyLocalizer::load_image(const QString & image_path) {
    cout << "Loading next image: " << image_path.toUtf8().data() << endl;
    std::string std_image_path = image_path.toUtf8().data();
    if(! filesystem::exists(std_image_path)) {
        throw std::runtime_error("Could not open file: `" + std_image_path + "`");
    }
    auto & img = this->images.at(image_path);
    img.load();
    cout << "LOADED" << endl;
    auto tags = this->getTagsProposal(img.getCvMat());
    cout << "Tags found: " << tags.size() << endl;
    assert(!tags.empty());
    img.setTags(std::move(tags));
}


bool ManuellyLocalizer::load_next_image() {
    this->images[this->current_image_path].unload();
    this->current_image_idx++;
    if(this->image_paths.size() <= this->current_image_idx) {
        return false;
    }
    this->current_image_path = this->getCurrentImagePath();
    cout << "Loading next image: " << this->getCurrentImagePath().toUtf8().data() << endl;
    this->load_image(this->current_image_path);
    return true;
}

const std::vector<Tag> ManuellyLocalizer::getTagsProposal(cv::Mat image) {
    cv::Mat preprocced = this->preprocessor.process(image);
    cout << "Running Localizer" << endl;
    vector<pipeline::Tag> localizer_tags =
            this->localizer.process(std::move(image), std::move(preprocced));
    cout << "Running EllipseFitter " << endl;
    const vector<pipeline::Tag> &pipeline_tags =
            this->ellipseFitter.process(std::move(localizer_tags));
    vector<Tag> tags;
    for(auto ptag : pipeline_tags) {
        optional<pipeline::Ellipse> ellipse;
        for(auto candidate : ptag.getCandidatesConst()) {
            if (!ellipse.is_initialized()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            } else if (candidate.getEllipse().getVote() > ellipse.get().getVote()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            }

        }
        tags.push_back(Tag(this->getCurrentImagePath(),
                           ptag.getOrigSubImage(),
                           ptag.getBox(),
                           ellipse));
    }

    return tags;
}

void ManuellyLocalizer::displayTags() {
}
