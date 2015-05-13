#ifndef LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
#define LOCALIZER_CAFFE_MANUELLCLASSIFIER_H


#include <string>
#include <vector>

#include <boost/optional/optional.hpp>
#include <opencv2/core/core.hpp>

#include <pipeline/datastructure/Tag.h>
#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>


namespace deep_localizer{
namespace manually {

class ManuellyLocalizer {
private:
    std::vector<std::string> image_paths;
    std::string current_image_path;
    std::map<std::string, std::vector<pipeline::Tag>> images_tags;
    cv::Mat current_image;
    unsigned long current_image_idx = 0;
    std::vector<pipeline::Tag> current_tags;
    unsigned long current_tag_idx = 0;

    pipeline::Preprocessor preprocessor;
    pipeline::Localizer localizer;

    void load_image(std::string & image_path);
    bool load_next_image();
    std::vector<pipeline::Tag> getTagsProposal(cv::Mat image);
public:
    explicit ManuellyLocalizer(std::vector<std::string> & image_files);
    const boost::optional<pipeline::Tag> next_tag();
    const cv::Mat& getCurrentImage() const;
    const std::string getCurrentImagePath() const;
    const std::vector<std::string> getImagePaths() const;
    void displayTags();
};
}
}

#endif //LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
