#include "utils.h"
#include "CaffeClassifier.h"
#include "DataReader.h"

namespace deeplocalizer {

CaffeClassifier::CaffeClassifier(const std::string &model_file,
                                 const std::string &trained_file,
                                 bool use_gpu,
                                 size_t batch_size
) :
    _net(model_file, caffe::TEST),
    _batch_size(batch_size)
{
    if (use_gpu) {
        caffe::Caffe::set_mode(caffe::Caffe::GPU);
    } else {
        caffe::Caffe::set_mode(caffe::Caffe::CPU);
    }

    _net.CopyTrainedLayersFrom(trained_file);
    CHECK_EQ(_net.num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(_net.num_outputs(), 1) << "Network should have exactly one output.";
    caffe::Blob<float>* input_layer = _net.input_blobs()[0];
    _num_channels = input_layer->channels();
    CHECK(_num_channels == 3 || _num_channels == 1)
        << "Input layer should have 1 or 3 channels.";
    _input_geometry = cv::Size(input_layer->width(), input_layer->height());
}

static bool PairCompare(const std::pair<float, int>& lhs,
                        const std::pair<float, int>& rhs) {
    return lhs.first > rhs.first;
}

/* Return the indices of the top N values of vector v. */
static std::vector<std::vector<int>> argmax(const std::vector<std::vector<float>>& vv, int N) {

    std::vector<std::vector<int>> results;
    for(const auto & v : vv) {
        std::vector<std::pair<float, int>> pairs;
        for(size_t i = 0; i < v.size(); i++) {
            const auto & elem = v.at(i);
            pairs.emplace_back(std::make_pair(elem, int(i)));
        }
        std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);
        std::vector<int> result;
        for (int i = 0; i < N; ++i)
            result.push_back(pairs[i].second);
        results.emplace_back(std::move(result));
    }
    return results;
}

std::vector<int> CaffeClassifier::topLabel(caffe::Blob<float> &blob) {
    auto predictions = topN(blob, 1);
    std::vector<int> prediction_labels;
    prediction_labels.reserve(predictions.size());
    for(unsigned int i = 0; i < predictions.size(); i++) {
        prediction_labels.push_back(predictions.at(i).at(0));
    }
    return prediction_labels;
}

std::vector<std::vector<int>> CaffeClassifier::topN(caffe::Blob<float> &blob,
                                                    int N) {
    auto outputs = forward(blob);
    return argmax(outputs, N);
}

std::vector<std::vector<float>> CaffeClassifier::forward(caffe::Blob<float> &blob) {
    setBatchSize(_batch_size);
    const std::vector<caffe::Blob<float>*> input_blobs{&blob};
    _net.Forward(input_blobs);
    caffe::Blob<float>* output_layer = _net.output_blobs()[0];
    size_t n_labels = output_layer->channels();
    std::vector<std::vector<float>> outputs;
    outputs.reserve(n_labels);
    for(size_t i = 0; i < _batch_size; i++) {
        const float* begin = output_layer->cpu_data() + n_labels*i;
        const float* end = begin + n_labels;
        outputs.emplace_back(std::vector<float>(begin, end));
    }
    return outputs;
}


void ConfusionMatrix::add(const std::vector<int> &true_labels,
                           const std::vector<int> &predicted_labels) {
    ASSERT(true_labels.size() == predicted_labels.size(), "labels vector mismatch in size");
    for(unsigned int i = 0; i < predicted_labels.size(); i++) {
        add(true_labels.at(i), predicted_labels.at(i));
    }
}

void ConfusionMatrix::add(int ground_truth_idx, int predicted_idx) {
    const size_t p = predicted_idx;
    const size_t t = ground_truth_idx;
    // std::cout << "t: " << t << ", p: " << p << std::endl;
    while(_confMat.size() <= p) _confMat.emplace_back(std::vector<unsigned long>{});
    while(_confMat.at(p).size() <= t) _confMat.at(p).emplace_back(0);
    _confMat.at(p).at(t)++;
}

void ConfusionMatrix::print() {
    const auto confMat = toDouble();
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
        accuracy += confMat[i][i];
    }
    std::cout << "Overall accuracy is: " << accuracy << std::endl;
}

std::vector<std::vector<double>> ConfusionMatrix::toDouble() {
    std::vector<std::vector<double>> confMatDouble;
    double n_examples = totalExamples();
    for(unsigned int i = 0; i < _confMat.size(); i++) {
        confMatDouble.emplace_back(std::vector<double>{});
        for(unsigned int j = 0; j < _confMat.at(i).size(); j++) {
            confMatDouble.at(i).emplace_back(_confMat.at(i).at(j) / n_examples);
        }
    }
    return confMatDouble;
}

unsigned long ConfusionMatrix::totalExamples() {
    unsigned long n = 0;
    for(const auto & row: _confMat) {
        for(const auto &elem: row) {
            n += elem;
        }
    }
    return n;
}

void CaffeClassifier::setBatchSize(size_t batch_size) {
    if (batch_size == _batch_size) return;
    _batch_size = batch_size;
    reshapeNet();
}

void CaffeClassifier::reshapeNet() {
    CHECK(_net.input_blobs().size() == 1);
    caffe::Blob<float>* input_layer = _net.input_blobs()[0];
    input_layer->Reshape(_batch_size, _num_channels,
                         _input_geometry.height,
                         _input_geometry.width);
    _net.Reshape();
}
}
