
#ifndef DEEP_LOCALIZER_DATASET_H
#define DEEP_LOCALIZER_DATASET_H


#include <memory>
#include <boost/filesystem.hpp>

#include "TrainData.h"

namespace deeplocalizer {
namespace tagger {

class ImageDesc;

class Dataset  {
public:
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
    void writeImages(const std::string &output_dir) const;
    void writePaths(const std::string &output_dir) const;
    void saveLMDB(const std::string &output_dir) const;
signals:
    void progress(double p);

private:
    void writeImages(const boost::filesystem::path &output_dir,
                     const std::vector<TrainData> &data) const ;

    void writePaths(const boost::filesystem::path &pathfile,
                    const std::vector<std::pair<std::string, int>> &  paths_label) const;
    void saveLMDB(const boost::filesystem::path &lmdb_file,
                  const std::vector<TrainData> &data) const;
};
}
}
#endif //DEEP_LOCALIZER_DATASET_H
