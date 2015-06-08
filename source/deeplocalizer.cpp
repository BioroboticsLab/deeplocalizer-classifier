#include "deeplocalizer.h"

#include "Tag.h"
#include "Image.h"

namespace deeplocalizer {
void registerQMetaTypes() {
    qRegisterMetaType<tagger::ImageDesc>("ImageDesc");
    qRegisterMetaType<tagger::Tag>("Tag");
    qRegisterMetaType<cv::Mat>("Mat");
}
}

#include "deeplocalizer.h"
