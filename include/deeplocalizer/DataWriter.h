#ifndef DEEP_LOCALIZER_DATAWRITER_H
#define DEEP_LOCALIZER_DATAWRITER_H


#include <memory>
#include <boost/filesystem.hpp>
#include <mutex>
#include <lmdb.h>

#include "TrainDatum.h"
#include "Dataset.h"

namespace deeplocalizer {
namespace tagger {

class DatasetWriter {
public:
    virtual void write(const std::vector <TrainDatum> &dataset, Dataset::Phase phase) = 0;
    virtual ~DatasetWriter() = default;
    static std::shared_ptr <DatasetWriter> fromSaveFormat(
            const std::string &output_dir,
            Dataset::Format format);
protected:
    inline std::pair<std::string, int> toFilenameLabel(const TrainDatum & datum) {
        return std::make_pair(datum.filename(), datum.tag().isYes());
    }
};

class ImageDatasetWriter : public DatasetWriter {
public:
    ImageDatasetWriter(const std::string & output_dir);
    virtual void write(const std::vector<TrainDatum> &dataset, Dataset::Phase phase);
    virtual ~ImageDatasetWriter() = default;
private:
    boost::filesystem::path _output_dir;
    boost::filesystem::path _train_dir;
    boost::filesystem::path _test_dir;


    std::ofstream _test_stream;
    std::mutex _test_stream_mutex;

    std::ofstream _train_stream;
    std::mutex _train_stream_mutex;

    void writeImages(const std::vector<TrainDatum> &data, Dataset::Phase phase) const;
    void writeLabelFile(const std::vector<TrainDatum> &data, Dataset::Phase phase);

    inline std::mutex &getMutex(Dataset::Phase phase) {
        return phase == Dataset::Train ? _train_stream_mutex : _test_stream_mutex;
    }
    inline std::ofstream &getStream(Dataset::Phase phase) {
        return phase == Dataset::Train ? _train_stream : _test_stream;
    }
};


class LMDBDatasetWriter : public DatasetWriter {
public:
    LMDBDatasetWriter(const std::string &output_dir);
    virtual void write(const std::vector <TrainDatum> &dataset, Dataset::Phase phase);
    virtual ~LMDBDatasetWriter();

private:
    boost::filesystem::path _output_dir;
    boost::filesystem::path _train_dir;
    boost::filesystem::path _test_dir;

    MDB_env *_train_mdb_env;
    MDB_env *_test_mdb_env;
    std::mutex _train_mutex;
    std::mutex _test_mutex;
    unsigned long _id = 0;

    void openDatabase(const boost::filesystem::path &lmdb_dir,
                      MDB_env **mdb_env);

    inline std::mutex &getMutex(Dataset::Phase phase) {
        return phase == Dataset::Train ? _train_mutex : _test_mutex;
    }
    inline MDB_env* &getMDB_env(Dataset::Phase phase) {
        return phase == Dataset::Train ? _train_mdb_env : _test_mdb_env;
    }
};
class AllFormatWriter : public DatasetWriter {
public:
    AllFormatWriter(const std::string &output_dir);
    virtual void write(const std::vector<TrainDatum> &dataset, Dataset::Phase phase);

private:
    std::unique_ptr<LMDBDatasetWriter> _lmdb_writer;
    std::unique_ptr<ImageDatasetWriter> _image_writer;
};

class DevNullWriter : public DatasetWriter {
    virtual void write(const std::vector<TrainDatum> &, Dataset::Phase) {}
};

}
}
#endif //DEEP_LOCALIZER_DATAWRITER_H
