#ifndef DEEP_LOCALIZER_DEEPLOCALIZER_H
#define DEEP_LOCALIZER_DEEPLOCALIZER_H

#include <QMetaType>
#include <opencv2/core/core.hpp>


Q_DECLARE_METATYPE(cv::Mat)

namespace deeplocalizer {
void registerQMetaTypes();
}

#endif //DEEP_LOCALIZER_DEEPLOCALIZER_H
