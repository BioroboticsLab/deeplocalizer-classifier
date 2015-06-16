
#include "Dataset.h"

#include "TrainsetGenerator.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

void Dataset::writeImages(const std::string &output_dir) {
    io::path train_dir{output_dir};
    io::path test_dir{output_dir};
    io::path validation_dir{output_dir};

    train_dir.append("train");
    test_dir.append("test");
    validation_dir.append("validation");

    writeImages(train_dir, train);
    writeImages(test_dir, test);
    writeImages(validation_dir, validation);
}

void Dataset::writePaths(const std::string &output_dir) {
    io::path train_pathfile{output_dir};
    io::path test_pathfile{output_dir};
    io::path validation_pathfile{output_dir};

    train_pathfile.append("train").append("train.txt");
    test_pathfile.append("test").append("test.txt");
    validation_pathfile.append("validation").append("validation.txt");

    writePaths(train_pathfile, train_name_labels);
    writePaths(test_pathfile, test_name_labels);
    writePaths(validation_pathfile, validation_name_labels);
}

void Dataset::writeImages(const boost::filesystem::path &output_dir,
                          const std::vector<TrainData> &data) {
    io::create_directories(output_dir);
    for(const auto & d: data) {
        io::path output_file(output_dir);
        output_file.append(d.filename());
        cv::imwrite(output_file.string(), d.mat());
    }
}

void Dataset::writePaths(const boost::filesystem::path &pathfile,
                         const std::vector<std::pair<std::string, int>> &  paths_label) {
    std::ofstream o(pathfile.string());
    for(const auto & pair : paths_label) {
        o << pair.first << " " << pair.second << "\n";
    }
}
void Dataset::clearImages() {
    train.clear();
    test.clear();
    validation.clear();
}
}
}
