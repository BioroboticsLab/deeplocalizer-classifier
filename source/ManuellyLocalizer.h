#ifndef LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
#define LOCALIZER_CAFFE_MANUELLCLASSIFIER_H


#include <string>
#include <vector>

#include <boost/optional/optional.hpp>
#include <opencv2/core/core.hpp>
#include <QString>

#include <pipeline/datastructure/Tag.h>
#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>
#include <pipeline/EllipseFitter.h>

#include "Tag.h"
#include "Image.h"


namespace deeplocalizer {
namespace tagger {

class ManuellyLocalizer {
private:
    std::vector<QString> image_paths;
    QString current_image_path;

    std::vector<Image> images;
    unsigned long _current_image_idx = 0;
    bool _first_image = true;
    pipeline::Preprocessor preprocessor;
    pipeline::Localizer localizer;
    pipeline::EllipseFitter ellipseFitter;

    void loadImage(Image &img);

    bool loadNextImage();
    const std::vector< std::shared_ptr< Tag > > getTagsProposal(cv::Mat image);
public:
    explicit ManuellyLocalizer();
    explicit ManuellyLocalizer(std::vector<QString> & _image_paths);
    const boost::optional<Image &> nextImage();
    Image& getCurrentImage();
    const QString getCurrentImagePath() const;
    const std::vector<QString> getImagePaths() const;
    void displayTags();

};
}
}

#endif //LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
