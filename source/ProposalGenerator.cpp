
#include "ProposalGenerator.h"

#include <thread>
#include "utils.h"

#include "serialization.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

const std::string ProposalGenerator::IMAGE_DESC_EXT = "proposal.desc";

ProposalGenerator::ProposalGenerator() {
}

ProposalGenerator::ProposalGenerator(const std::vector<ImageDesc> & image_desc)
    : _images_before_pipeline(image_desc.cbegin(), image_desc.cend()),
      _n_images(image_desc.size())
{
    this->init();
}

ProposalGenerator::ProposalGenerator(
        const std::vector<ImageDesc> & images_todo,
        const std::vector<ImageDesc> & images_done
)
        : _images_before_pipeline(images_todo.cbegin(), images_todo.cend()),
          _images_with_proposals(images_done.cbegin(), images_done.cend()),
          _n_images(images_todo.size() + images_done.size())
{
    this->init();
}
ProposalGenerator::ProposalGenerator(const std::vector<std::string>& image_paths)
    : _n_images(image_paths.size())
{
    for (auto path : image_paths) {
        if (!io::exists(path)) {
            throw std::runtime_error("Could not open file");
        }
        _images_before_pipeline.push_back(ImageDesc(path));
    }
    this->init();
}

void ProposalGenerator::init() {
    int cpus = std::thread::hardware_concurrency();
    for(int i = 0; i < (cpus != 0 ? cpus: 1); i++) {
        auto worker = new PipelineWorker();
        auto thread = new QThread;
        worker->moveToThread(thread);

        connect(this, &ProposalGenerator::finished, thread, &QThread::quit);
        connect(this, &ProposalGenerator::finished, worker, &PipelineWorker::deleteLater);
        thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);

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
        QMetaObject::invokeMethod(worker, "process", Q_ARG(ImageDesc, img));
    }
    if (_images_with_proposals.size() == _n_images) {
        emit finished();
    }
}

void ProposalGenerator::imageProcessed(ImageDesc img) {
    _images_with_proposals.push_back(img);
    img.setSavePathExtension(IMAGE_DESC_EXT);
    img.save();
    emit progress(_images_with_proposals.size() / static_cast<double>(_n_images));
    if (_images_with_proposals.size() == _n_images) {
        emit finished();
    }
}

ProposalGenerator::~ProposalGenerator() {
}
}
}
