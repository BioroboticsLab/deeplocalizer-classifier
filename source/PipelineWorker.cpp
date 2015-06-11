#include "PipelineWorker.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <pipeline/datastructure/Tag.h>

namespace deeplocalizer {
namespace tagger {
using boost::optional;
namespace io = boost::filesystem;

const std::string PipelineWorker::DEFAULT_CONFIG_FILE = "pipeline-config.json";

PipelineWorker::PipelineWorker() {
    init(DEFAULT_CONFIG_FILE);
}

PipelineWorker::PipelineWorker(const std::string & config_file) {
    init(config_file);
}

void PipelineWorker::init(const std::string & config_file) {
    using namespace pipeline;
    io::path config_path{config_file};
    ASSERT(io::exists(config_path), config_path.string() << " does not exist.");
    settings::preprocessor_settings_t pre_settings;
    settings::localizer_settings_t loc_settings;
    settings::ellipsefitter_settings_t ell_settings;

    pre_settings.loadFromJson(config_file);
    loc_settings.loadFromJson(config_file);
    ell_settings.loadFromJson(config_file);

    _preprocessor.loadSettings(pre_settings);
    _localizer.loadSettings(loc_settings);
    _ellipseFitter.loadSettings(ell_settings);
}

std::vector<Tag> PipelineWorker::tagsProposals(ImageDesc & img_descr) {
    Image img{img_descr};
    cv::Mat preprocced = _preprocessor.process(img.getCvMat());
    std::vector<pipeline::Tag> localizer_tags =
            _localizer.process(std::move(img.getCvMat()), std::move(preprocced));
    const std::vector<pipeline::Tag> pipeline_tags =
            _ellipseFitter.process(std::move(localizer_tags));
    std::vector<Tag> tags;
    for(auto ptag : pipeline_tags) {
        tags.emplace_back(Tag(ptag));
    }
    return tags;
}

void PipelineWorker::process(ImageDesc img) {
    ASSERT(io::exists(img.filename),
           "Could not open file: `" << img.filename << "`");
    img.setTags(this->tagsProposals(img));
    emit resultReady(img);
}
void PipelineWorker::findEllipse(cv::Mat mat, Tag tag) {
    pipeline::Tag pipeTag(tag.getBoundingBox(), 0 /* id */);
    pipeTag.setOrigSubImage(mat);
    auto pipelineTags= _ellipseFitter.process({pipeTag});
    Tag tagWithEll(pipelineTags.at(0));
    tagWithEll.setId(tag.id());
    tagWithEll.setIsTag(tag.isTag());
    emit tagWithEllipseReady(tagWithEll);
}
}
}
