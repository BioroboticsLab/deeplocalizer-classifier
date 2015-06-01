#include "PipelineWorker.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <pipeline/datastructure/Tag.h>

namespace deeplocalizer {
namespace tagger {
using boost::optional;
namespace io = boost::filesystem;

bool contains(const cv::Mat & m, const cv::Rect & rect) {
    return (rect.x >= 0 && rect.y >= 0 && rect.width >= 0 && rect.height >= 0 &&
            rect.x + rect.width < m.cols && rect.y + rect.height < m.rows);
}

const cv::Rect centerBoxAtEllipse(const cv::Rect & boundingBox,
                                  const cv::Mat & image ,
                                  const optional<pipeline::Ellipse> & ellipse) {
    if(ellipse.is_initialized()) {
        cv::Point2i center = ellipse.get().getCen();

        cv::Rect box(center.x - TAG_WIDTH / 2, TAG_WIDTH,
                     center.y - TAG_HEIGHT / 2, TAG_HEIGHT);
        if (contains(image, box)) {
            return box;
        }
    }
    return boundingBox;
}

std::vector<Tag> PipelineWorker::tagsProposals(ImageDescription & img_descr) {
    Image img{img_descr};
    cv::Mat preprocced = _preprocessor.process(img.getCvMat());
    std::vector<pipeline::Tag> localizer_tags =
            _localizer.process(std::move(img.getCvMat()), std::move(preprocced));
    const std::vector<pipeline::Tag> pipeline_tags =
            _ellipseFitter.process(std::move(localizer_tags));
    std::vector<Tag> tags;
    for(auto ptag : pipeline_tags) {
        optional<pipeline::Ellipse> ellipse;
        for(auto candidate : ptag.getCandidatesConst()) {
            if (!ellipse.is_initialized()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            } else if (ellipse.get() < candidate.getEllipse()) {
                ellipse = optional<pipeline::Ellipse>(candidate.getEllipse());
            }
        }
        tags.push_back(
                Tag(centerBoxAtEllipse(ptag.getBox(), img.getCvMat(),  ellipse),
                    ellipse));
    }
    return tags;
}

void PipelineWorker::process(ImageDescription img) {
    std::string std_image_path = img.filename.toStdString();
    ASSERT(io::exists(std_image_path),
           "Could not open file: `" << std_image_path << "`");
    img.setTags(this->tagsProposals(img));
    emit resultReady(img);
}
}
}
