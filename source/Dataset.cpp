#include <lmdb.h>
#include <iomanip>
#include "utils.h"
#include "Dataset.h"

#include "TrainsetGenerator.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

void Dataset::clearImages() {
    train.clear();
    test.clear();
}

boost::optional<Dataset::Format> Dataset::parseFormat(const std::string &str) {
    if(str == "images") {
        return Dataset::Format::Images;
    } else if (str == "all") {
        return Dataset::Format::All;
    } else if (str == "lmdb") {
        return Dataset::Format::LMDB;
    } else {
        return boost::optional<Format>();
    }
}

std::shared_ptr<DatasetWriter> DatasetWriter::fromSaveFormat(
        const std::string &output_dir, Dataset::Format save_format) {
    switch (save_format) {
        case Dataset::Format::All:
            return std::make_shared<AllFormatWriter>(output_dir);
        case Dataset::Format::Images:
            return std::make_shared<ImageDatasetWriter>(output_dir);
        case Dataset::Format::LMDB:
            return std::make_shared<LMDBDatasetWriter>(output_dir);
        default:
            return std::make_shared<DevNullWriter>();
    }
}

ImageDatasetWriter::ImageDatasetWriter(const std::string &output_dir)
    : _output_dir(output_dir),
    _train_dir{_output_dir / "train"},
    _test_dir{_output_dir / "test"}
{

    io::create_directories(_output_dir);
    io::create_directories(_train_dir);
    io::create_directories(_test_dir);

    _test_label_stream.open((_test_dir / "test.txt").string());
    _train_label_stream.open((_train_dir / "train.txt").string());
}

void ImageDatasetWriter::write(const Dataset &dataset) {
    writeImages(dataset);
    writeLabelFiles(dataset);
}
void ImageDatasetWriter::writeImages(const Dataset &dataset) const {

    writeImages(_train_dir, dataset.train);
    writeImages(_test_dir, dataset.test);
}

void ImageDatasetWriter::writeImages(const boost::filesystem::path &output_dir,
                                     const std::vector<TrainDatum> &data) const {
    io::create_directories(output_dir);
    for(const auto & d: data) {
        io::path output_file(output_dir);
        output_file.append(d.filename());
        cv::imwrite(output_file.string(), d.mat());
    }
}

void ImageDatasetWriter::writeLabelFiles(const Dataset &dataset) {
    writeLabelFile(_train_dir, dataset.train_name_labels, _train_label_stream,
                   _train_label_stream_mutex);
    writeLabelFile(_test_dir, dataset.test_name_labels, _test_label_stream,
                   _test_label_stream_mutex);
}

void ImageDatasetWriter::writeLabelFile(
        const boost::filesystem::path &output_dir,
        const std::vector<std::pair<std::string, int>> &paths_label,
        std::ofstream &label_stream, std::mutex &mutex) {
    std::lock_guard<std::mutex> lock(mutex);
    for(const auto & pair : paths_label) {
        io::path image_path = output_dir / pair.first;
        label_stream << image_path.string() << " " << pair.second << "\n";
    }
}

ImageDatasetWriter::~ImageDatasetWriter() { }

LMDBDatasetWriter::LMDBDatasetWriter(const std::string &output_dir) :
    _output_dir(output_dir),
    _train_dir{_output_dir},
    _test_dir{_output_dir}
{
    _train_dir.append("train");
    _test_dir.append("test");

    io::create_directories(_output_dir);
    io::create_directories(_train_dir);
    io::create_directories(_test_dir);

    openDatabase(_train_dir, &_train_mdb_env);
    openDatabase(_test_dir, &_test_mdb_env);
}

void LMDBDatasetWriter::openDatabase(const boost::filesystem::path &lmdb_dir,
                                     MDB_env **mdb_env
) {
    ASSERT(mdb_env_create(mdb_env) == MDB_SUCCESS,
           "mdb_env_create failed");
    ASSERT(mdb_env_set_mapsize(*mdb_env, 1099511627776) == MDB_SUCCESS, "");  // 1TB
    ASSERT(mdb_env_open(*mdb_env, lmdb_dir.string().c_str(), 0, 0664) == MDB_SUCCESS,
           "mdb_env_open failed");
}
void LMDBDatasetWriter::write(const Dataset &dataset) {
    write(dataset.train, _train_mdb_env);
    write(dataset.test, _test_mdb_env);
}

unsigned long swap(unsigned long i) {
    long b0, b1, b2, b3, b4, b5, b6, b7;
    b0 = (i & 0x00000000000000ff) << 56u;
    b1 = (i & 0x000000000000ff00) << 40u;
    b2 = (i & 0x0000000000ff0000) << 24u;
    b3 = (i & 0x00000000ff000000) << 8u;
    b4 = (i & 0x000000ff00000000) >> 8u;
    b5 = (i & 0x0000ff0000000000) >> 24u;
    b6 = (i & 0x00ff000000000000) >> 40u;
    b7 = (i & 0xff00000000000000) >> 56u;
    return b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7;
}

void LMDBDatasetWriter::write(const std::vector<TrainDatum> &data,
                              MDB_env *mdb_env) {
    auto indecies = shuffledIndecies(data.size());
    MDB_txn * mdb_txn = nullptr;
    MDB_val mdb_key, mdb_data;
    MDB_dbi mdb_dbi;
    std::lock_guard<std::mutex> lock(_mutex);
    ASSERT(mdb_env != nullptr, "Error: mdb_env is nullptr");

    ASSERT(mdb_txn_begin(mdb_env, nullptr, 0, &mdb_txn) == MDB_SUCCESS,
           "mdb_txn_begin failed");
    ASSERT(mdb_dbi_open(mdb_txn, nullptr, 0, &mdb_dbi) == MDB_SUCCESS,
           "mdb_open failed. Does the lmdb already exist? ");
    std::string data_string;
    unsigned long n = 1024;
    for(unsigned int i = 0; i < data.size(); i++) {
        const auto & d = data.at(indecies.at(i));
        auto caffe_datum = d.toCaffe();
        caffe_datum.SerializeToString(&data_string);
        // lmdb uses memcmp therefore we convert to big endian
        unsigned long key = swap(_id++);
        mdb_data.mv_size = data_string.size();
        mdb_data.mv_data = reinterpret_cast<void *>(&data_string[0]);
        mdb_key.mv_size = sizeof(unsigned long);
        mdb_key.mv_data = reinterpret_cast<void *>(&key);

        auto err = mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, MDB_APPEND);
        if (err != MDB_SUCCESS)
            ASSERT(false, "mdb_put failed: " << err << ", key: " << _id-1);

        if(i % n == 0 && i != 0) {
            ASSERT(mdb_txn_commit(mdb_txn) == MDB_SUCCESS,
                   "mdb_txn_commit failed");
            ASSERT(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn) == MDB_SUCCESS,
                   "mdb_txn_begin failed");
        }
    }
    ASSERT(mdb_txn_commit(mdb_txn) == MDB_SUCCESS, "mdb_txn_commit failed");
    mdb_dbi_close(mdb_env, mdb_dbi);
}

LMDBDatasetWriter::~LMDBDatasetWriter() {
    mdb_env_close(_train_mdb_env);
    mdb_env_close(_test_mdb_env);
}
AllFormatWriter::AllFormatWriter(const std::string &output_dir) :
    _lmdb_writer(std::make_unique<LMDBDatasetWriter>(output_dir)),
    _image_writer(std::make_unique<ImageDatasetWriter>(output_dir))
{
}

void AllFormatWriter::write(const Dataset &dataset) {
    _lmdb_writer->write(dataset);
    _image_writer->write(dataset);
}
}
}
