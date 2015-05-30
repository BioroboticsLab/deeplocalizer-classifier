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
    using ImageDescrQueue = std::deque<std::shared_ptr<ImageDescription>>;
    Q_OBJECT

public slots:
    void imageProcessed(std::shared_ptr<ImageDescription> image);
signals:
    void finished();

public:
    explicit ProposalGenerator(std::vector<QString> _image_paths);

    void processPipeline();
    std::vector<QString> & getImagePaths();
    void save(const std::string & path) const;
    static std::unique_ptr<ProposalGenerator> load(const std::string & path);
    virtual ~ProposalGenerator() {
        for(auto &t: _threads) {
            t->quit();
            t->wait();
            delete t;
        }
    }

    inline const ImageDescrQueue & getBeforePipelineImages() const {
        return _images_before_pipeline;
    }

    inline const ImageDescrQueue & getProposalImages() const {
        return _images_with_proposals;
    }

private:
    explicit ProposalGenerator();

    std::vector<QString> _image_paths;
    ImageDescrQueue _images_before_pipeline;
    ImageDescrQueue _images_with_proposals;

    std::vector<QThread *> _threads;
    std::vector<PipelineWorker *> _workers;

    friend class boost::serialization::access;

    template <class Archive>
    void serialize( Archive & ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(_image_paths);
        ar & BOOST_SERIALIZATION_NVP(_images_before_pipeline);
        ar & BOOST_SERIALIZATION_NVP(_images_with_proposals);
    }
};
}
}

#endif //DEEP_LOCALIZER_PROPOSALGENERATOR_H
