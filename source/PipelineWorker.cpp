#include "PipelineWorker.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <pipeline/datastructure/Tag.h>

namespace deeplocalizer {
namespace tagger {
using boost::optional;
namespace io = boost::filesystem;

std::vector<Tag> PipelineWorker::tagsProposals(ImageDesc & img_descr) {
    Image img{img_descr};
    cv::Mat preprocced = _preprocessor.process(img.getCvMat());
    std::vector<pipeline::Tag> localizer_tags =
            _localizer.process(std::move(img.getCvMat()), std::move(preprocced));
    const std::vector<pipeline::Tag> pipeline_tags =
            _ellipseFitter.process(std::move(localizer_tags));
    std::vector<Tag> tags;
    for(auto ptag : pipeline_tags) {
        if (ptag.getCandidatesConst().size() > 0) {
            tags.emplace_back(Tag(ptag));
        }
    }
    return tags;
}

void PipelineWorker::process(ImageDesc img) {
    std::string std_image_path = img.filename;
    ASSERT(io::exists(std_image_path),
           "Could not open file: `" << std_image_path << "`");
    img.setTags(this->tagsProposals(img));
    emit resultReady(img);
}
void PipelineWorker::findEllipse(cv::Mat mat, Tag tag) {
    qDebug() << "findEllipse" << tag.getId();
    pipeline::Tag pipeTag(tag.getBoundingBox(), 0 /* id */);
    pipeTag.setOrigSubImage(mat);
    auto tagWithEllipses = _ellipseFitter.process(
            std::vector<pipeline::Tag>{pipeTag});
    Tag tagWithEll(tagWithEllipses.at(0));
    emit tagWithEllipseReady(tagWithEll);
}
}
}
