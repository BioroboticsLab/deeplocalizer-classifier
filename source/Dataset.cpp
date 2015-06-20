#include <lmdb.h>
#include "utils.h"
#include "Dataset.h"

#include "TrainsetGenerator.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

void Dataset::clearImages() {
    train.clear();
    test.clear();
    validation.clear();
}


ImageDatasetWriter::ImageDatasetWriter(const std::string &output_dir)
    : _output_dir(output_dir),
    _train_dir{_output_dir / "train"},
    _test_dir{_output_dir / "test"},
    _validation_dir{_output_dir / "validation"}
{

    io::create_directories(_output_dir);
    io::create_directories(_train_dir);
    io::create_directories(_test_dir);
    io::create_directories(_validation_dir);

    _test_label_stream.open((_test_dir / "test.txt").string());
    _train_label_stream.open((_train_dir / "train.txt").string());
    _validation_label_stream.open((_validation_dir / "validation.txt").string());
}

void ImageDatasetWriter::write(const Dataset &dataset) {
    writeImages(dataset);
    writeLabelFiles(dataset);
}
void ImageDatasetWriter::writeImages(const Dataset &dataset) const {

    writeImages(_train_dir, dataset.train);
    writeImages(_test_dir, dataset.test);
    writeImages(_validation_dir, dataset.validation);
}

void ImageDatasetWriter::writeImages(const boost::filesystem::path &output_dir,
                                     const std::vector<TrainData> &data) const {
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
    writeLabelFile(_validation_dir, dataset.validation_name_labels,
                   _validation_label_stream,
                   _validation_label_stream_mutex);
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
    std::cout << "opening: " << lmdb_dir.string() << std::endl;
    ASSERT(mdb_env_create(mdb_env) == MDB_SUCCESS,
           "mdb_env_create failed");
    ASSERT(mdb_env_set_mapsize(*mdb_env, 1099511627776) == MDB_SUCCESS, "");  // 1TB
    ASSERT(mdb_env_open(*mdb_env, lmdb_dir.string().c_str(),
                        MDB_WRITEMAP | MDB_MAPASYNC, 0664) == MDB_SUCCESS,
           "mdb_env_open failed");
}
void LMDBDatasetWriter::write(const Dataset &dataset) {
    write(dataset.train, _train_mdb_env);
    write(dataset.test, _test_mdb_env);
}

void LMDBDatasetWriter::write(const std::vector<TrainData> &data,
                              MDB_env *mdb_env) {
    MDB_txn * mdb_txn = nullptr;
    MDB_val mdb_key, mdb_data;
    MDB_dbi mdb_dbi;

    ASSERT(mdb_env != nullptr, "Error: mdb_env is nullptr");

    ASSERT(mdb_txn_begin(mdb_env, nullptr, 0, &mdb_txn) == MDB_SUCCESS,
           "mdb_txn_begin failed");
    ASSERT(mdb_dbi_open(mdb_txn, nullptr, 0, &mdb_dbi) == MDB_SUCCESS,
           "mdb_open failed. Does the lmdb already exist? ");

    std::string data_string;
    for(unsigned int i = 0; i < data.size(); i++) {
        const auto & d = data.at(i);
        auto caffe_datum = d.toCaffe();
        auto random_str = io::unique_path("%%%%%%%%%%%%%%").string();
        caffe_datum.SerializeToString(&data_string);
        mdb_data.mv_size = data_string.size();
        mdb_data.mv_data = reinterpret_cast<void *>(&data_string[0]);
        mdb_key.mv_size = random_str.size();
        mdb_key.mv_data = reinterpret_cast<void *>(&random_str[0]);

        ASSERT(mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, 0) == MDB_SUCCESS,
               "mdb_put failed");
        if(i % 256) {
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
