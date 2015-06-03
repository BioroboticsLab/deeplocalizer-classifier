#include "Tag.h"
#include "Image.h"

namespace deeplocalizer {
void registerQMetaTypes() {
    qRegisterMetaType<tagger::ImageDescription>("ImageDescription");
    qRegisterMetaType<tagger::Tag>("Tag");
    qRegisterMetaType<cv::Mat>("Mat");
}
}

#include "deeplocalizer.h"
