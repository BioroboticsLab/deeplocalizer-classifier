#ifndef DEEP_LOCALIZER_DATAREADER_H
#define DEEP_LOCALIZER_DATAREADER_H

#include <thread>
#include <lmdb.h>
#include <caffe/caffe.hpp>
#include <caffe/util/db_lmdb.hpp>
#include "TrainDatum.h"

namespace deeplocalizer {

using DataTransformerPtr = std::unique_ptr<caffe::DataTransformer<float>>;

class DataReader {
public:
    DataReader(std::vector<int> shape);
    virtual bool read(caffe::Blob<float> &data, std::vector<int> & labels) = 0;

    inline const std::vector<int>& shape() {
        return _shape;
    }

    virtual ~DataReader() = default;
protected:
    std::vector<int> _shape;
    class DataPrefetcher {
        using PrefetchSignature = void(size_t, std::vector<caffe::Datum> &);
        using PrefetchFn = std::function<PrefetchSignature>;
        std::vector<caffe::Datum> _prefetch_buf;
        bool _valid = true;
        std::vector<int> _shape;
        size_t _batch_size;
        DataTransformerPtr _data_transformer;
        PrefetchFn _prefetch_read_fn;
        std::thread _prefetch_thread;
        caffe::Blob<float> _prefetch_blob;
        std::vector<int> _prefetch_labels;
    public:
        DataPrefetcher(PrefetchFn &&prefetch_read_fn,
                       DataTransformerPtr &&trans, std::vector<int> shape);
        bool read(caffe::Blob<float> & blob, std::vector<int> &labels);
        void prefetch_thread_fn();
        ~DataPrefetcher();
    };
};

class ImageReader : public DataReader {
    boost::filesystem::path _pathfile;
    std::vector<std::pair<std::string, int>> _paths_labels;
    std::vector<std::pair<std::string, int>> readPathsLabelsFile();
    size_t _current_pos = 0;
    DataPrefetcher _prefecher;
    void prefetch_read(size_t n, std::vector<caffe::Datum> & data);
public:
    ImageReader(std::string path, std::vector<int> batch_size);
    ImageReader(std::string pathfile, std::vector<int> batch_size, DataTransformerPtr && trans);
    virtual inline bool read(caffe::Blob<float> &blob,
                             std::vector<int> & labels) override {
        return _prefecher.read(blob, labels);
    };
    using DataReader::read;
};

class LMDBReader : public DataReader {
    boost::filesystem::path _path;
    caffe::db::LMDB _lmdb;
    std::unique_ptr<caffe::db::Cursor> _lmdb_cursor;
    std::unique_ptr<DataPrefetcher> _prefecher;
    void prefetch_read(size_t n, std::vector<caffe::Datum> & data);
public:
    LMDBReader(std::string path, std::vector<int> shape);
    LMDBReader(std::string path, std::vector<int> shape, DataTransformerPtr && trans);
    virtual inline bool read(caffe::Blob<float> &blob,
                             std::vector<int> & labels) override {
        return _prefecher->read(blob, labels);
    };
    using DataReader::read;
};

}

#endif //DEEP_LOCALIZER_DATAREADER_H
