
#ifndef DEEP_LOCALIZER_TRAINSETGENERATOR_H
#define DEEP_LOCALIZER_TRAINSETGENERATOR_H

#include <boost/filesystem.hpp>
#include <QObject>

#include "Image.h"
#include "TrainDatum.h"
#include "Dataset.h"

namespace deeplocalizer {
namespace tagger {

class Dataset;

class TrainsetGenerator : public QObject  {
Q_OBJECT
public:
    TrainsetGenerator();
    TrainsetGenerator(std::shared_ptr<DatasetWriter> writer);
    const int uniform_wrong_tags = 10;
    const unsigned int samples_per_tag = 32;
    const unsigned int wrong_samples_per_tag = 32;
    const int shrinking = 32;
    unsigned long current_idx;
    boost::filesystem::path output_dir;

    cv::Mat randomAffineTransformation(const cv::Point2f & center);
    TrainDatum trainData(const ImageDesc & desc, const Tag & tag, const cv::Mat & subimage);
    void wrongSamplesAround(const Tag &tag,
                                               const ImageDesc &desc,
                                               const Image &img,
                                               std::vector<TrainDatum> &train_data);
    void wrongSamples(const ImageDesc & desc, std::vector<TrainDatum> & train_data);
    void trueSamples(const ImageDesc & desc, std::vector<TrainDatum> & train_data);
    void trueSamples(const ImageDesc & desc, const Tag &tag, const cv::Mat & subimage,
                     std::vector<TrainDatum> & train_data);
    cv::Mat rotate(const cv::Mat & src, double degrees);
    void process(const std::vector<ImageDesc> &descs);

    void process(const ImageDesc & desc,
            std::vector<TrainDatum> & train_data);
    void processDesc(const ImageDesc &desc, std::vector<TrainDatum> &data,
                     std::vector<std::pair<std::string, int>> &names);
signals:
    void progress(double p);

private:
    static const int MIN_TRANSLATION;
    static const int MAX_TRANSLATION;
    static const int MIN_AROUND_WRONG;
    static const int MAX_AROUND_WRONG;
    std::random_device _rd;
    std::mt19937  _gen;
    std::uniform_real_distribution<double> _angle_dis;
    std::uniform_int_distribution<int> _translation_dis;
    std::uniform_int_distribution<int> _around_wrong_dis;
    std::shared_ptr<DatasetWriter> _writer;
    std::vector<cv::Rect> getNearbyTagBoxes(const Tag &tag,
                                            const ImageDesc &desc);
    cv::Rect proposeWrongBox(const Tag &tag);
    bool intersectsNone(std::vector<cv::Rect> &tag_boxes, cv::Rect wrong_box);
    void wrongSampleRot90(const Image &img,
                          const cv::Rect &wrong_box,
                          std::vector<TrainDatum> &train_data);
    int wrongAroundCoordinate();
};
}
}

#endif //DEEP_LOCALIZER_TRAINSETGENERATOR_H
