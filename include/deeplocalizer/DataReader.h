#ifndef DEEP_LOCALIZER_DATAREADER_H
#define DEEP_LOCALIZER_DATAREADER_H

#include <thread>
#include <lmdb.h>
#include <caffe/util/db_lmdb.hpp>
#include "TrainDatum.h"

namespace deeplocalizer {
namespace tagger {

class DataReader {
public:
    virtual void read(size_t n, std::vector<caffe::Datum> &data) = 0;

    inline std::vector<caffe::Datum> read(size_t n) {
        std::vector<caffe::Datum> data_buf;
        read(n, data_buf);
        return data_buf;
    }

    virtual ~DataReader() = default;
protected:
    class DataPrefetcher {
        using PrefetchSignature = void(size_t, std::vector<caffe::Datum> &);
        using PrefetchFn = std::function<PrefetchSignature>;
        std::vector<caffe::Datum> _prefetch_buf;
        PrefetchFn _prefetch_read_fn;
        std::thread _prefetch_thread;
    public:
        DataPrefetcher() : _prefetch_read_fn(static_cast<PrefetchFn>([](size_t, std::vector<caffe::Datum> &){})) {}
        DataPrefetcher(PrefetchFn && prefetch_read_fn) :
                _prefetch_read_fn(prefetch_read_fn),
                _prefetch_thread(_prefetch_read_fn, 256, std::ref(_prefetch_buf)) {}

        void read(size_t n, std::vector<caffe::Datum> & data) {
            if(_prefetch_thread.joinable()) {
                _prefetch_thread.join();
            }
            if(_prefetch_buf.size() < n) {
                _prefetch_read_fn(n, _prefetch_buf);
            }
            auto end = _prefetch_buf.begin() + n;
            std::move(_prefetch_buf.begin(), end, std::back_inserter(data));
            _prefetch_buf.erase(_prefetch_buf.begin(), end);
            _prefetch_thread = std::thread(_prefetch_read_fn, n, std::ref(_prefetch_buf));
        }

        ~DataPrefetcher() {
            if(_prefetch_thread.joinable()) {
                _prefetch_thread.join();
            }
        }
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
    ImageReader(std::string path);
    virtual inline void read(size_t n, std::vector<caffe::Datum> &data) override {
        _prefecher.read(n, data);
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
    LMDBReader(std::string path);
    virtual inline void read(size_t n, std::vector<caffe::Datum> &data) override {
        _prefecher->read(n, data);
    };
    using DataReader::read;
};

}
}

#endif //DEEP_LOCALIZER_DATAREADER_H
