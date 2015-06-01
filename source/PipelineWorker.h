#ifndef DEEP_LOCALIZER_PIPELINETHREAD_H
#define DEEP_LOCALIZER_PIPELINETHREAD_H

#include <memory>
#include <QThread>

#include <pipeline/Preprocessor.h>
#include <pipeline/Localizer.h>
#include <pipeline/EllipseFitter.h>

#include "Tag.h"
#include "Image.h"

Q_DECLARE_METATYPE(deeplocalizer::tagger::ImageDescription)

namespace deeplocalizer {
namespace tagger {

    class PipelineWorker : public QObject {
    Q_OBJECT

    public slots:
        void process(ImageDescription img);
    signals:
        void resultReady(ImageDescription img);

    private:
        pipeline::Preprocessor _preprocessor;
        pipeline::Localizer _localizer;
        pipeline::EllipseFitter _ellipseFitter;
        std::vector<Tag> tagsProposals(ImageDescription & img);
    };
}
}


#endif //DEEP_LOCALIZER_PIPELINETHREAD_H
