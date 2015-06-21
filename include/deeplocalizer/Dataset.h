
#ifndef DEEP_LOCALIZER_DATASET_H
#define DEEP_LOCALIZER_DATASET_H


#include <memory>
#include <boost/filesystem.hpp>
#include <mutex>
#include <lmdb.h>

#include "TrainDatum.h"

namespace deeplocalizer {
namespace tagger {

class ImageDesc;

class Dataset  {
public:
    enum SaveFormat {
        Images,
        LMDB,
        All,
        DevNull
    };
    const double test_partition = 0.15;
    const double train_partition = 1 - test_partition;
    std::vector <TrainDatum> train;
    std::vector <TrainDatum> test;

    std::vector<std::pair<std::string, int>> train_name_labels;
    std::vector<std::pair<std::string, int>> test_name_labels;

    void clearImages();
};


class DatasetWriter {
public:
    virtual void write(const Dataset& dataset) = 0;
    virtual ~DatasetWriter() {};
    static std::shared_ptr <DatasetWriter> fromSaveFormat(
            const std::string &output_dir,
            Dataset::SaveFormat format);
};

class ImageDatasetWriter : public DatasetWriter {
public:
    ImageDatasetWriter(const std::string & output_dir);
    virtual void write(const Dataset& dataset);
    void writeImages(const Dataset& dataset) const;
    void writeLabelFiles(const Dataset& dataset);
    virtual ~ImageDatasetWriter();
private:
    boost::filesystem::path _output_dir;

    boost::filesystem::path _train_dir;
    boost::filesystem::path _test_dir;


    std::ofstream _test_label_stream;
    std::mutex _test_label_stream_mutex;

    std::ofstream _train_label_stream;
    std::mutex _train_label_stream_mutex;

    void writeImages(const boost::filesystem::path &output_dir,
                     const std::vector<TrainDatum> &imgaes) const;
    void writeLabelFile(
            const boost::filesystem::path &output_dir,
            const std::vector<std::pair<std::string, int>> &paths_label,
            std::ofstream &label_stream, std::mutex &mutex);
};

class LMDBDatasetWriter : public DatasetWriter {
public:
    LMDBDatasetWriter(const std::string & output_dir);
    virtual void write(const Dataset& dataset);
    virtual ~LMDBDatasetWriter();

private:
    boost::filesystem::path _output_dir;
    boost::filesystem::path _train_dir;
    boost::filesystem::path _test_dir;

    MDB_env *_train_mdb_env;
    MDB_env *_test_mdb_env;

    void write(const std::vector<TrainDatum> &data, MDB_env *mdb_env);
    void openDatabase(const boost::filesystem::path &lmdb_dir,
                      MDB_env **mdb_env);

};
class AllFormatWriter : public DatasetWriter {
public:
    AllFormatWriter(const std::string &output_dir);
    virtual void write(const Dataset& dataset);

private:
    std::unique_ptr<LMDBDatasetWriter> _lmdb_writer;
    std::unique_ptr<ImageDatasetWriter> _image_writer;
};

class DevNullWriter : public DatasetWriter {
    virtual void write(const Dataset&) {}
};

}
}
#endif //DEEP_LOCALIZER_DATASET_H
