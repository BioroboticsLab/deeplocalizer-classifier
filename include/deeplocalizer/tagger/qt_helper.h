
#ifndef DEEP_LOCALIZER_QT_HELPER_H
#define DEEP_LOCALIZER_QT_HELPER_H

#include <QImage>
#include <QPixmap>
#include <QDebug>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
namespace deeplocalizer {
QImage cvMatToQImage(const cv::Mat &inMat);

inline QPixmap cvMatToQPixmap(const cv::Mat &inMat) {
    return QPixmap::fromImage(cvMatToQImage(inMat));
}
void registerQMetaTypes();
}

#include <QImage>
#include <QPixmap>

#endif //DEEP_LOCALIZER_QT_HELPER_H
