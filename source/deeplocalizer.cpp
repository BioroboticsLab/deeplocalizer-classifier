#include "deeplocalizer.h"

#include "Tag.h"
#include "Image.h"

namespace deeplocalizer {
void registerQMetaTypes() {
    qRegisterMetaType<ImageDesc>("ImageDesc");
    qRegisterMetaType<Tag>("Tag");
    qRegisterMetaType<cv::Mat>("Mat");
}
}

