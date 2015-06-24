
#include <caffe/caffe.hpp>
#include <caffe/util/db.hpp>
#include <caffe/util/db_lmdb.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <thread>

#include "Dataset.h"

namespace po = boost::program_options;
using namespace caffe;
using namespace deeplocalizer::tagger;
namespace io = boost::filesystem;

po::options_description desc_option("Options");
po::positional_options_description positional_opt;


void setupOptions() {
    desc_option.add_options()
            ("help,h", "Print help messages")
            ("weights,w", po::value<std::string>(), "The weights of the model.")
            ("data,d", po::value<std::string>(), "Path to lmdb database.")
            ("model,m", po::value<std::string>(), "path to the model .prototxt file.")
            ("gpu", po::bool_switch(), "use GPU.")
            ("output-dir,o", po::value<std::string>(), "output dir to save wrong examples.");
}


class CaffeStats {
public:
  CaffeStats(const std::string& model_file,
             const std::string& trained_file,
             const std::string& data,
             const std::string& output_dir,
             bool use_gpu);

  vector<vector<double>> confusionMatrix();
private:
    std::vector<int> topLabel(Blob<float>& blob);
    std::vector<std::vector<int>> topN(Blob<float>& blob, int N);
    void getPrefetchData(Blob<float> &blob, std::vector<int> &labels,
                                     std::vector<Datum> &data);
    void prefetchData();

    void collectWrongExamples(const std::vector<int> &predicted_labels,
                                          const std::vector<int> &labels,
                                          const vector<Datum> &data);
    Net<float> _net;
    cv::Size _input_geometry;
    int _num_channels;
    const std::string& _output_dir;
    caffe::db::LMDB _lmdb;
    bool _lmdb_valid = true;
    std::unique_ptr<db::Cursor> _lmdb_cursor;
    std::unique_ptr<DataTransformer<float>> _data_transformer;
    int _batch_size = 256;
    unsigned long max_collected_examples = 4096;
    unsigned long _collected_examples = 0;
    Blob<float> _prefetch_blob;
    std::vector<int> _prefetch_labels;
    std::vector<Datum> _prefetch_data;
    std::thread _prefetch_thread;
};


CaffeStats::CaffeStats(const string& model_file,
                       const string& trained_file,
                       const string& data,
                       const string& output_dir,
                       bool use_gpu
)
        : _net(model_file, TEST),
          _output_dir(output_dir)
{
    TransformationParameter trans_parameter {};
    trans_parameter.set_scale(float(1)/255);
    _data_transformer = std::make_unique<DataTransformer<float>>(trans_parameter, TEST);

    _lmdb.Open(data, caffe::db::Mode::READ);
    _lmdb_cursor = std::unique_ptr<db::Cursor>(_lmdb.NewCursor());
    if (use_gpu) {
        Caffe::set_mode(Caffe::GPU);
    } else {
        Caffe::set_mode(Caffe::CPU);
    }

    _net.CopyTrainedLayersFrom(trained_file);

    CHECK_EQ(_net.num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(_net.num_outputs(), 1) << "Network should have exactly one output.";

    Blob<float>* input_layer = _net.input_blobs()[0];
    _num_channels = input_layer->channels();
    CHECK(_num_channels == 3 || _num_channels == 1)
    << "Input layer should have 1 or 3 channels.";
    _input_geometry = cv::Size(input_layer->width(), input_layer->height());

    _prefetch_blob.Reshape(_batch_size, _num_channels,
                             _input_geometry.height, _input_geometry.width);

    _prefetch_thread = std::thread(std::mem_fn<void()>(&CaffeStats::prefetchData), this);
}

static bool PairCompare(const std::pair<float, int>& lhs,
                        const std::pair<float, int>& rhs) {
  return lhs.first > rhs.first;
}

/* Return the indices of the top N values of vector v. */
static std::vector<std::vector<int>> argmax(const std::vector<float>& v, int batch_size, int N) {
    unsigned long n_channels = v.size() / batch_size;

    std::vector<std::vector<int>> results;
    for(int b = 0; b < batch_size; b++) {
        std::vector<std::pair<float, int>> pairs;
        for(unsigned int c = 0; c < n_channels; c++) {
            pairs.emplace_back(std::make_pair(v[b*n_channels + c], c));
        }
        std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);
        std::vector<int> result;
        for (int i = 0; i < N; ++i)
            result.push_back(pairs[i].second);
        results.emplace_back(std::move(result));
    }
    return results;
}

void CaffeStats::prefetchData() {
    Blob<float> input_blob(_batch_size, _num_channels,
                           _input_geometry.height, _input_geometry.width);
    Datum datum;
    _prefetch_labels.clear();
    for(int i = 0; i < _batch_size && _lmdb_cursor->valid(); i++) {
        datum.ParseFromString(_lmdb_cursor->value());
        _prefetch_data.emplace_back(std::move(datum));
        _lmdb_cursor->Next();
        _prefetch_labels.emplace_back(datum.label());
    }
    _lmdb_valid = _lmdb_cursor->valid();
    if(not _lmdb_valid) return;
    _data_transformer->Transform(_prefetch_data, &_prefetch_blob);

}
void CaffeStats::getPrefetchData(Blob<float> &blob, std::vector<int> &labels,
                                 std::vector<Datum> &data) {
    _prefetch_thread.join();
    caffe_copy<float>(_prefetch_blob.count(), _prefetch_blob.cpu_data(),
                      blob.mutable_cpu_data());
    labels = std::vector<int>(_prefetch_labels.begin(), _prefetch_labels.end());
    data = std::move(_prefetch_data);
    _prefetch_data = std::vector<Datum>();
    _prefetch_thread = std::thread(std::mem_fn<void()>(&CaffeStats::prefetchData), this);

}
std::vector<std::vector<double>> convertToDouble(
        std::vector<std::vector<unsigned long>> confusion, long n_examples) {
    std::vector<std::vector<double>> confusion_d;
    for(unsigned int i = 0; i < confusion.size(); i++) {
        confusion_d.emplace_back(std::vector<double>{});
        for(unsigned int j = 0; j < confusion.at(i).size(); j++) {
            confusion_d.at(i).emplace_back(confusion.at(i).at(j) / double(n_examples));
        }
    }
    return confusion_d;
}

void addToConfusionMatrix(const std::vector<int> & predicted_labels,
                          const std::vector<int> & labels,
                          std::vector<std::vector<unsigned long>>& confusion) {
    for(unsigned int i = 0; i < predicted_labels.size(); i++) {
        unsigned int p = predicted_labels[i];
        unsigned int t = labels.at(i);
        while(confusion.size() <= p) confusion.emplace_back(std::vector<unsigned long>{});
        while(confusion.at(p).size() <= t) confusion.at(p).emplace_back(0);
        confusion.at(p).at(t)++;
    }
}

void CaffeStats::collectWrongExamples(const std::vector<int> &predicted_labels,
                                      const std::vector<int> &labels,
                                      const vector<Datum> &data) {
    if(_collected_examples >= max_collected_examples) return;
    for(unsigned int i = 0; i < predicted_labels.size(); i++) {
        unsigned int p = predicted_labels[i];
        unsigned int t = labels.at(i);
        if(p == t) { continue; }
        cv::Mat mat = DecodeDatumToCVMatNative(data.at(i));
        std::stringstream ss;
        ss << _output_dir << "/predicted_" << p << "_actual_" << t << "/%%%%%%%%%%%%.jpeg";
        io::path path = io::unique_path(ss.str());
        io::create_directories(path.parent_path());
        cv::imwrite(path.string(), mat);
        _collected_examples++;
    }
}


std::vector<std::vector<double>> CaffeStats::confusionMatrix() {
    std::vector<std::vector<unsigned long>> confusion;
    long n_examples = 0;

    Blob<float> input_blob(_batch_size, _num_channels, _input_geometry.height, _input_geometry.width);
    std::vector<int> labels;
    std::vector<Datum> data;
    for(int iteration = 0; ; iteration++) {
        if (iteration % 50 == 0) {
            std::cout << "Iteration #" << iteration << std::endl;
        }
        getPrefetchData(input_blob, labels, data);
        if(not _lmdb_valid) { break; }
        auto predicted_labels = topLabel(input_blob);
        collectWrongExamples(predicted_labels, labels, data);
        addToConfusionMatrix(predicted_labels, labels, confusion);
        n_examples += _batch_size;
    }
    if(_prefetch_thread.joinable()) { _prefetch_thread.join(); }
    return convertToDouble(confusion, n_examples);
}

std::vector<int> CaffeStats::topLabel(Blob<float>& blob) {
    auto predictions = topN(blob, 1);
    std::vector<int> prediction_labels;
    prediction_labels.reserve(predictions.size());
    for(unsigned int i = 0; i < predictions.size(); i++) {
        prediction_labels.push_back(predictions.at(i).at(0));
    }
    return prediction_labels;
}
std::vector<std::vector<int>> CaffeStats::topN(Blob<float>& blob, int N) {
    Blob<float>* input_layer = _net.input_blobs()[0];
    input_layer->Reshape(_batch_size, _num_channels,
                       _input_geometry.height,
                       _input_geometry.width);
    _net.Reshape();
    CHECK(_net.input_blobs().size() == 1);
    const std::vector<Blob<float>*> input_blobs{&blob};
    _net.Forward(input_blobs);

    Blob<float>* output_layer = _net.output_blobs()[0];
    const float* begin = output_layer->cpu_data();
    const float* end = begin + output_layer->channels() * _batch_size;
    std::vector<float> outputs(begin, end);
    return argmax(outputs, _batch_size, N);
}

void printConfusionMat(const std::vector<std::vector<double>> & confMat) {
    std::cout << "Confusion Matrix: " << std::endl;
    std::cout << "Predicted class is vertical \\ Actual class is horizontal" << std::endl;
    std::cout.precision(4);
    for(unsigned long i = 0; i < confMat.size(); i++) {
        const auto& row = confMat.at(i);
        if( i == 0) {
            std::cout << "   ";
            for(unsigned long j = 0; j < row.size(); j++) {
                std::cout << std::setw(7) << j;
            }
            std::cout << std::endl;
        }
        std::cout << std::setw(3) << i << ": ";
        for(const auto &v: row) {
            std::cout << std::setw(7) << v*100;
        }
        std::cout << std::endl;
    }
    double accuracy = 0;
    for(unsigned int i=0;i < confMat.size(); i++) {
        accuracy += confMat.at(i).at(i);
    }
    std::cout << "Overall accuracy is: " << accuracy << std::endl;
}

int run(const std::string &model, const std::string trained_model,
        const std::string &data,  const std::string &output_dir, bool use_gpu) {
    if(use_gpu) {
        std::cout << "using gpu" << std::endl;
    }
    CaffeStats classifier(model, trained_model, data, output_dir, use_gpu);
    auto confMat = classifier.confusionMatrix();
    printConfusionMat(confMat);
    return 0;
}

void printUsage() {
    std::cout << "Usage: generate_proposals [options] pathfile.txt "<< std::endl;
    std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
    std::cout << desc_option << std::endl;
}

int main(int argc, char* argv[])
{
    setupOptions();
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc_option).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        printUsage();
        return 0;
    }
    if(vm.count("weights") && vm.count("data") && vm.count("model") && vm.count("output-dir")) {
        auto model = vm.at("model").as<std::string>();
        auto weights = vm.at("weights").as<std::string>();
        auto data = vm.at("data").as<std::string>();
        auto output_dir = vm.at("output-dir").as<std::string>();
        bool use_gpu = vm.at("gpu").as<bool>();
        return run(model, weights, data, output_dir, use_gpu);
    } else {
        std::cout << "No model, data or format given." << std::endl;
        printUsage();
        return 1;
    }
}
