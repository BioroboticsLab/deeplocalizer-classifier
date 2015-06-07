#ifndef DEEP_LOCALIZER_QTHELPER_H
#define DEEP_LOCALIZER_QTHELPER_H


#include <iostream>
#include <fstream>

#include <QImage>
#include <QPixmap>
#include <QDebug>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include <boost/serialization/nvp.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>

#ifndef NDEBUG
#include <sstream>
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::stringstream ss; \
            ss << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message; \
            std::cerr << ss.str() << std::endl; \
            throw ss.str(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

namespace deeplocalizer {
namespace tagger {

inline QImage cvMatToQImage(const cv::Mat &inMat) {
    switch (inMat.type()) {
        // 8-bit, 4 channel
        case CV_8UC4: {
            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32);

            return image;
        }

            // 8-bit, 3 channel
        case CV_8UC3: {
            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888);

            return image.rgbSwapped();
        }

            // 8-bit, 1 channel
        case CV_8UC1: {
            static QVector <QRgb> sColorTable;

            // only create our color table once
            if (sColorTable.isEmpty()) {
                for (int i = 0; i < 256; ++i)
                    sColorTable.push_back(qRgb(i, i, i));
            }

            QImage image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8);

            image.setColorTable(sColorTable);

            return image;
        }

        default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}

inline QPixmap cvMatToQPixmap(const cv::Mat &inMat) {
    return QPixmap::fromImage(cvMatToQImage(inMat));
}

template<typename T>
void safe_serialization(const std::string &path, const boost::serialization::nvp<T> &nvp) {
    boost::filesystem::path save_path{path};
    boost::filesystem::path tmp_path = boost::filesystem::unique_path(save_path.parent_path() / "%%%%%%%%%.binary");
    try {
        std::ofstream os(tmp_path.string());
        boost::archive::binary_oarchive archive(os);
        archive << nvp;
    } catch(boost::archive::archive_exception e) {
        std::cerr << "Could not serialize file " << path;
        throw;
    }
    boost::filesystem::rename(tmp_path, save_path);
}

}
}
#endif //DEEP_LOCALIZER_QTHELPER_H
