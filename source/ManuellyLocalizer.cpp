
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <QtCore/qtextstream.h>
#include "pipeline/Localizer.h"

#include "ManuellyLocalizer.h"

using namespace std;
using namespace boost;
using namespace deeplocalizer::tagger;

ManuellyLocalizer::ManuellyLocalizer() {  }

ManuellyLocalizer::ManuellyLocalizer(vector<QString> &_image_paths) {
    this->image_paths = _image_paths;
    this->_image_paths = _image_paths;
    for(auto path : _image_paths) {
        if(! filesystem::exists(path.toStdString())) {
            throw std::runtime_error("Could not open file");
        }
        this->images.push_back(Image(path));
    }
}

Image& ManuellyLocalizer::getCurrentImage() {
    assert(_current_image_idx <= images.size());

    return images.at(_current_image_idx);
}

const vector<QString> ManuellyLocalizer::getImagePaths() const {
    return this->image_paths;
}

const QString ManuellyLocalizer::getCurrentImagePath() const {
    return this->image_paths[this->current_image_idx];
}

const optional<Image &> ManuellyLocalizer::nextImage() {
    if(! _first_image) {
        this->images.at(_current_image_idx).unload();
        if (_image_paths.size() <= _current_image_idx + 1) {
            return optional<Image &>();
        }
        _current_image_idx++;
    } else {
        _first_image = false;
    }
    auto path = this->getCurrentImagePath();
    cout << "Loading next image: " << path.toStdString() << endl;
    auto & img = images.at(_current_image_idx);
    this->loadImage(img);
    return optional<Image &>(img);
}

void ManuellyLocalizer::loadImage(Image &img) {
    std::string std_image_path = img.filename.toStdString();
    cout << "Loading next image: " << std_image_path << endl;
    if(! filesystem::exists(std_image_path)) {
        throw std::runtime_error("Could not open file: `" + std_image_path + "`");
    }
    img.load();
    auto tags = this->getTagsProposal(img.getCvMat());
    img.setTags(std::move(tags));
}
bool contains(const cv::Mat m, const cv::Rect & rect) {
    return (rect.x >= 0 && rect.y >= 0 && rect.width >= 0 && rect.height >= 0 &&
            rect.x + rect.width < m.cols && rect.y + rect.height < m.rows);
}
const cv::Mat getSubimage(cv::Mat orginal, cv::Rect boundingBox,
                          optional<pipeline::Ellipse> ellipse) {
    if(ellipse.is_initialized()) {
        cv::Point2i center = ellipse.get().getCen();

        cv::Rect box(center.x - TAG_WIDTH / 2, TAG_WIDTH,
                     center.y - TAG_HEIGHT / 2, TAG_HEIGHT);
        if (contains(orginal, box)) {
            return orginal(box).clone();
        }
    }
    return orginal(boundingBox).clone();
}
const vector< std::shared_ptr< Tag > > ManuellyLocalizer::getTagsProposal(cv::Mat image) {
    cv::Mat preprocced = this->preprocessor.process(image);
    cout << "Running Localizer" << endl;
    vector<pipeline::Tag> localizer_tags =
            this->localizer.process(std::move(image), std::move(preprocced));
    cout << "Running EllipseFitter " << endl;
    const vector<pipeline::Tag> &pipeline_tags =
            this->ellipseFitter.process(std::move(localizer_tags));
    vector< std::shared_ptr<Tag > > tags;
    for(auto ptag : pipeline_tags) {
        optional<pipeline::Ellipse> ellipse;
        for(auto candidate : ptag.getCandidatesConst()) {
            if (!ellipse.is_initialized()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            } else if (candidate.getEllipse().getVote() > ellipse.get().getVote()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            }

        }

        tags.push_back(std::make_shared<Tag>(
                Tag(this->getCurrentImagePath(),
                    getSubimage(image, ptag.getBox(), ellipse),
                    ptag.getBox(),
                    ellipse)));
    }

    return tags;
}

void ManuellyLocalizer::displayTags() {
}
