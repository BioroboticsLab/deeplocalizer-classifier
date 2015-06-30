#ifndef DEEP_LOCALIZER_PIPELINETHREAD_H
#define DEEP_LOCALIZER_PIPELINETHREAD_H

#include <memory>
#include <QThread>

#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>
#include <pipeline/EllipseFitter.h>


namespace deeplocalizer {
    class ImageDesc;
    class Tag;

    class PipelineWorker : public QObject {
    Q_OBJECT

    public:
        PipelineWorker();
        PipelineWorker(const std::string &  config_file);
        void init(const std::string & config_file);
        static const std::string DEFAULT_CONFIG_FILE;

    public slots:
        void process(ImageDesc img);
        void findEllipse(cv::Mat mat, Tag tag);

    signals:
        void resultReady(ImageDesc img);
        void tagWithEllipseReady(Tag tag);

    private:
        pipeline::Preprocessor _preprocessor;
        pipeline::Localizer _localizer;
        pipeline::EllipseFitter _ellipseFitter;
        std::vector<Tag> tagsProposals(ImageDesc & img);
        static const std::string DEFAULT_JSON_SETTINGS;
    };
}


#endif //DEEP_LOCALIZER_PIPELINETHREAD_H
