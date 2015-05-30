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


class ManuellyTagger : public QObject {
    Q_OBJECT

signals:
    void finishedTagging();
public:
    explicit ManuellyTagger();
    explicit ManuellyTagger(const std::vector<ImageDescription> & images_with_proposals);

    ImageDescription nextImageDescr();
    void addVerified(ImageDescription && img);

    void save(const std::string & path) const;
    static std::unique_ptr<ManuellyTagger> load(const std::string & path);

    const std::deque<ImageDescription> & getProposalImages() const {
        return _images_with_proposals;
    }

    const std::deque<ImageDescription> & getClassifiedImages() const {
        return _images_verified;
    }

private:
    std::deque<ImageDescription> _images_with_proposals;
    std::deque<ImageDescription> _images_verified;


    friend class boost::serialization::access;
    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(_images_with_proposals);
        ar & BOOST_SERIALIZATION_NVP(_images_verified);
    }
};
}
}

#endif //LOCALIZER_CAFFE_MANUELLCLASSIFIER_H
