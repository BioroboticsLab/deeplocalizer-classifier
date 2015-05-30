
#include "ProposalGenerator.h"

#include <thread>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "serialization.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

ProposalGenerator::ProposalGenerator() {
}

ProposalGenerator::ProposalGenerator(std::vector<QString> _image_paths) {
    this->_image_paths = _image_paths;
    for(auto path : _image_paths) {
        if(! io::exists(path.toStdString())) {
            throw std::runtime_error("Could not open file");
        }
        _images_before_pipeline.push_back(
                std::shared_ptr<ImageDescription>(new ImageDescription(path)));
    }
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
        connect(thread, &QThread::finished, [&thread]() {
            std::cout << "thread finished: " << QThread::currentThreadId()  << std::endl;
        });


        connect(worker, &PipelineWorker::resultReady, this, &ProposalGenerator::imageProcessed);
        thread->start();

        _workers.push_back(worker);
        _threads.push_back(thread);
    }
}

std::vector<QString> & ProposalGenerator::getImagePaths() {
    return _image_paths;
}

void ProposalGenerator::processPipeline() {
    unsigned long n = _threads.size();
    for(unsigned long i = 0; !_images_before_pipeline.empty(); i++) {
        unsigned long n_thread = i % n;
        auto img = _images_before_pipeline.front();
        std::cout << "before size: " << _images_before_pipeline.size() << std::endl;
        _images_before_pipeline.pop_front();
        std::cout << "before size: " << _images_before_pipeline.size() << std::endl;
        auto & worker = _workers.at(n_thread);
        emit worker->process(img);
    }
}

void ProposalGenerator::imageProcessed(std::shared_ptr<ImageDescription> img) {
    std::cout << "ImageProcessed: " << img->filename.toStdString() << std::endl;
    std::cout << "Thread: " << QThread::currentThreadId()  << std::endl;
    _images_with_proposals.push_back(img);
    if (_images_with_proposals.size() == _image_paths.size()) {
        std::cout << "pipeline finished: " << std::endl;
        emit finished();
    }
}

void ProposalGenerator::save(const std::string & path) const {
    std::ofstream os(path);
    boost::archive::xml_oarchive archive(os);
    archive << boost::serialization::make_nvp("tagger", *this);
}

std::unique_ptr<ProposalGenerator> ProposalGenerator::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::xml_iarchive archive(is);
    ProposalGenerator * loc = new ProposalGenerator;
    archive >> boost::serialization::make_nvp("tagger", loc);
    return std::unique_ptr<ProposalGenerator>(loc);
}

}
}
