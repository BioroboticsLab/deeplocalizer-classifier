
#include "ProposalGenerator.h"

#include <thread>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "serialization.h"
#include "utils.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

ProposalGenerator::ProposalGenerator() {
}

ProposalGenerator::ProposalGenerator(const std::vector<ImageDescription> & image_desc)
    : _images_before_pipeline(image_desc.cbegin(), image_desc.cend()),
      _n_images(image_desc.size())
{
    this->init();
}

ProposalGenerator::ProposalGenerator(const std::vector<QString>& image_paths)
    : _n_images(image_paths.size())
{
    for (auto path : image_paths) {
        if (!io::exists(path.toStdString())) {
            throw std::runtime_error("Could not open file");
        }
        _images_before_pipeline.push_back(ImageDescription(path));
    }
    this->init();
}

void ProposalGenerator::init() {
    int cpus = std::thread::hardware_concurrency();
    for(int i = 0; i < (cpus != 0 ? cpus: 1); i++) {
        auto worker = new PipelineWorker();
        auto thread = new QThread;
        worker->moveToThread(thread);
        // connect(worker, &worker->error, this, &this->errorString);
        connect(this, &ProposalGenerator::finished, thread, &QThread::quit);
        connect(this, &ProposalGenerator::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(thread, &QThread::finished, worker, &QThread::deleteLater);

        connect(worker, &PipelineWorker::resultReady, this, &ProposalGenerator::imageProcessed);
        thread->start();

        _workers.push_back(worker);
        _threads.push_back(thread);
    }
}


void ProposalGenerator::processPipeline() {
    unsigned long n = _threads.size();
    for(unsigned long i = 0; !_images_before_pipeline.empty(); i++) {
        unsigned long worker_idx = i % n;
        auto img = _images_before_pipeline.front();
        _images_before_pipeline.pop_front();
        auto & worker = _workers.at(worker_idx);
        QMetaObject::invokeMethod(worker, "process", Q_ARG(ImageDescription, img));
    }
}

void ProposalGenerator::imageProcessed(ImageDescription img) {
    _images_with_proposals.push_back(img);
    emit progress(_images_with_proposals.size() / static_cast<double>(_n_images));
    if (_images_with_proposals.size() == _n_images) {
        emit finished();
    }
}

void ProposalGenerator::save(const std::string & path) const {
    std::ofstream os(path);
    boost::archive::binary_oarchive archive(os);
    archive << boost::serialization::make_nvp("tagger", *this);
}

std::unique_ptr<ProposalGenerator> ProposalGenerator::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::binary_iarchive archive(is);
    ProposalGenerator * loc = new ProposalGenerator;
    archive >> boost::serialization::make_nvp("tagger", loc);
    return std::unique_ptr<ProposalGenerator>(loc);
}

}
}
