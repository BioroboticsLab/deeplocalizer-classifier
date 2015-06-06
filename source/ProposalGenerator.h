#ifndef DEEP_LOCALIZER_PROPOSALGENERATOR_H
#define DEEP_LOCALIZER_PROPOSALGENERATOR_H

#include <memory>
#include <QObject>
#include <QThread>

#include <boost/serialization/type_info_implementation.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/optional/optional.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Image.h"
#include "PipelineWorker.h"
#include "serialization.h"

namespace deeplocalizer {
namespace tagger {


class ProposalGenerator : public QObject {
    Q_OBJECT

public slots:
    void imageProcessed(ImageDesc image);
signals:
    void progress(double progress);
    void finished();

public:
    explicit ProposalGenerator(const std::vector<QString>& _image_paths);
    explicit ProposalGenerator(const std::vector<ImageDesc>&  _image_desc);
    explicit ProposalGenerator(const std::vector<ImageDesc> & images_todo,
                               const std::vector<ImageDesc> & images_done);

    void processPipeline();
    void saveProposals(const std::string &path) const;

    inline const std::deque<ImageDesc> & getBeforePipelineImages() const {
        return _images_before_pipeline;
    }

    inline const std::deque<ImageDesc> & getProposalImages() const {
        return _images_with_proposals;
    }
    virtual ~ProposalGenerator();
private:
    explicit ProposalGenerator();
    void init();

    std::deque<ImageDesc> _images_before_pipeline;
    std::deque<ImageDesc> _images_with_proposals;
    unsigned long _n_images;

    std::vector<QThread *> _threads;
    std::vector<PipelineWorker *> _workers;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(_images_before_pipeline);
        ar & BOOST_SERIALIZATION_NVP(_images_with_proposals);
    }
};
}
}

#endif //DEEP_LOCALIZER_PROPOSALGENERATOR_H
