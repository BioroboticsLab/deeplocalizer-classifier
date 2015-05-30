#ifndef DEEP_LOCALIZER_PIPELINETHREAD_H
#define DEEP_LOCALIZER_PIPELINETHREAD_H

#include <memory>
#include <QThread>

#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>
#include <pipeline/EllipseFitter.h>

#include "Tag.h"
#include "Image.h"

Q_DECLARE_METATYPE(std::shared_ptr<deeplocalizer::tagger::Image>)

namespace deeplocalizer {
namespace tagger {

    class PipelineWorker : public QObject {
    Q_OBJECT

    public slots:
        void process(std::shared_ptr<Image> img);
    signals:
        void resultReady(std::shared_ptr<Image> img);

    private:
        pipeline::Preprocessor _preprocessor;
        pipeline::Localizer _localizer;
        pipeline::EllipseFitter _ellipseFitter;
        std::vector<Tag> tagsProposals(Image & img);
    };
}
}


#endif //DEEP_LOCALIZER_PIPELINETHREAD_H
