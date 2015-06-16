
#ifndef DEEP_LOCALIZER_DATASET_H
#define DEEP_LOCALIZER_DATASET_H


#include <memory>
#include <boost/filesystem.hpp>

#include "TrainData.h"

namespace deeplocalizer {
namespace tagger {

class ImageDesc;

class Dataset  {
    enum OutputType {
        IMAGES,
        LLDB
    };

public:
    OutputType output_type = LLDB;
    const double test_partition = 0.15;
    const double validation_partition = 0.15;
    const double train_partition = 1 - test_partition - validation_partition;
    std::vector <TrainData> train;
    std::vector <TrainData> validation;
    std::vector <TrainData> test;

    std::vector<std::pair<std::string, int>> train_name_labels;
    std::vector<std::pair<std::string, int>> test_name_labels;
    std::vector<std::pair<std::string, int>> validation_name_labels;

    void clearImages();
    void writeImages(const std::string &output_dir);
    void writePaths(const std::string &output_dir);

signals:
    void progress(double p);

private:
    void writeImages(const boost::filesystem::path &output_dir,
                     const std::vector<TrainData> &data);

    void writePaths(const boost::filesystem::path &pathfile,
                    const std::vector<std::pair<std::string, int>> &  paths_label);
};
}
}
#endif //DEEP_LOCALIZER_DATASET_H
