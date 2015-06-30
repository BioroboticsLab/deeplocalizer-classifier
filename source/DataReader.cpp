#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <caffe/util/io.hpp>

#include "utils.h"
#include "DataReader.h"

namespace deeplocalizer {


namespace io = boost::filesystem;

DataReader::DataReader(std::vector<int> shape) : _shape(shape) {

}


DataReader::DataPrefetcher::DataPrefetcher(PrefetchFn &&prefetch_read_fn,
                                           DataTransformerPtr &&trans,
                                           std::vector<int> shape) :
    _shape(shape),
    _batch_size(size_t(_shape[0])),
    _data_transformer(std::move(trans)),
    _prefetch_read_fn(prefetch_read_fn)
{
    if(not _data_transformer) {
        caffe::TransformationParameter param;
        _data_transformer = std::make_unique<caffe::DataTransformer<float>>(param, caffe::TEST);
    }
    _prefetch_blob.Reshape(_shape);
}

bool DataReader::DataPrefetcher::read(caffe::Blob<float> & blob, std::vector<int> &labels) {
    if(_prefetch_thread.joinable()) {
        _prefetch_thread.join();
    }
    if(not _valid) {
        return false;
    }
    blob.ReshapeLike(_prefetch_blob);
    caffe::caffe_copy(_prefetch_blob.count(), _prefetch_blob.cpu_data(),
                      blob.mutable_cpu_data());
    labels = std::vector<int>(_prefetch_labels);
    _prefetch_thread = std::thread(std::mem_fn(&DataReader::DataPrefetcher::prefetch_thread_fn), this);
    return true;
}

void DataReader::DataPrefetcher::prefetch_thread_fn() {
    _prefetch_buf.clear();
    _prefetch_labels.clear();
    _prefetch_read_fn(_batch_size, _prefetch_buf);
    if(_prefetch_buf.size() != _batch_size) {
        _valid = false;
        return;
    }
    _data_transformer->Transform(_prefetch_buf, &_prefetch_blob);
}

DataReader::DataPrefetcher::~DataPrefetcher() {
    if(_prefetch_thread.joinable()) {
        _prefetch_thread.join();
    }
}

ImageReader::ImageReader(std::string pathfile, std::vector<int> shape) :
    ImageReader(pathfile, shape, std::unique_ptr<caffe::DataTransformer<float>>())
{}

ImageReader::ImageReader(std::string pathfile, std::vector<int> shape, DataTransformerPtr && trans) :
    DataReader(shape),
    _pathfile(pathfile),
    _paths_labels(readPathsLabelsFile()),
    _prefecher(std::bind(&ImageReader::prefetch_read,
                         this, std::placeholders::_1,
                         std::placeholders::_2),
               std::move(trans), _shape
    )
{
}

std::vector<std::pair<std::string, int>> ImageReader::readPathsLabelsFile() {
    ASSERT(io::exists(_pathfile), "Could not open label file: " << _pathfile.string());
    std::ifstream ifs(_pathfile.string());
    std::string line;
    std::vector<std::pair<std::string, int>> paths_labels;
    std::vector<std::string> strs;
    for(int i = 0; std::getline(ifs, line); i++) {
        boost::split(strs, line, boost::is_any_of("\t "));
        ASSERT(strs.size() == 2, "In " << _pathfile <<  ":" << i <<
                " Expected filepath and label to be sperated by a whitespace or tab.");
        std::string path_to_image = strs.at(0);
        int label = std::stoi(strs.at(1));
        ASSERT(io::exists(path_to_image), "File " << path_to_image << " does not exists.");
        paths_labels.emplace_back(std::make_pair(path_to_image, label));
    }
    return paths_labels;
}
void ImageReader::prefetch_read(size_t n, std::vector<caffe::Datum> & data) {
    size_t end_pos = _current_pos + n;
    for(; _current_pos < end_pos && _current_pos < _paths_labels.size(); _current_pos++) {
        caffe::Datum datum;
        const auto & pair = _paths_labels.at(_current_pos);
        caffe::ReadFileToDatum(pair.first, pair.second, &datum);
        data.emplace_back(std::move(datum));
    }
}

LMDBReader::LMDBReader(std::string path, std::vector<int> shape) :
    LMDBReader(path, shape, DataTransformerPtr())
{}

LMDBReader::LMDBReader(std::string path, std::vector<int> shape, DataTransformerPtr && trans) :
    DataReader(shape),
    _path(path)
{
    _lmdb.Open(_path.string(), caffe::db::Mode::READ);
    _lmdb_cursor = std::unique_ptr<caffe::db::Cursor>(_lmdb.NewCursor());
    _prefecher = std::make_unique<DataPrefetcher>(
            std::bind(&LMDBReader::prefetch_read, this, std::placeholders::_1, std::placeholders::_2),
            std::move(trans), _shape
    );
}

void LMDBReader::prefetch_read(size_t n, std::vector<caffe::Datum> & data) {
    ASSERT(_lmdb_cursor.get() != nullptr, "Expected _lmdb_cursor to be initialized");
    ASSERT(_lmdb_cursor->valid(), "Expected _lmdb_cursor to be valid");
    for(size_t i = 0; i < n && _lmdb_cursor->valid(); i++) {
        caffe::Datum datum;
        datum.ParseFromString(_lmdb_cursor->value());
        data.emplace_back(std::move(datum));
        _lmdb_cursor->Next();
    }
}
}
