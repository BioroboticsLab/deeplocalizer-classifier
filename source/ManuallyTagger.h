#ifndef LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
#define LOCALIZER_CAFFE_MANUELLCLASSIFIER_H


#include <string>
#include <deque>
#include <memory>

#include <boost/version.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/optional/optional.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <opencv2/core/core.hpp>
#include <QString>

#include <pipeline/datastructure/Tag.h>
#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>
#include <pipeline/EllipseFitter.h>

#include "Tag.h"
#include "Image.h"
#include "serialization.h"


namespace deeplocalizer {
namespace tagger {


class ManuallyTagger : public QObject {
    Q_OBJECT

public slots:
    void save(const std::string & path) const;
    void loadNextImage();
    void loadLastImage();
    void loadImage(unsigned long idx);
signals:
    void loadedImage(ImageDescPtr desc, ImagePtr img);
    void outOfRange(unsigned long idx);
    void lastImage();
    void firstImage();
public:
    explicit ManuallyTagger();
    explicit ManuallyTagger(const std::vector<ImageDesc> & descriptions);
    explicit ManuallyTagger(const std::vector<ImageDescPtr> & descriptions);
    explicit ManuallyTagger(std::vector<ImageDescPtr> && descriptions);

    static std::unique_ptr<ManuallyTagger> load(const std::string & path);
    const std::vector<ImageDescPtr> & getProposalImages() const {
        return _image_descs;
    }

private:
    std::vector<ImageDescPtr> _image_descs;
    ImagePtr _image;
    ImageDescPtr _desc;
    unsigned long _image_idx = 0;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(_image_idx);
    }
};
}
}

#endif //LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
