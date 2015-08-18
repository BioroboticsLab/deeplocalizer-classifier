
#ifndef DEEP_LOCALIZER_CAFFECLASSIFIER_H
#define DEEP_LOCALIZER_CAFFECLASSIFIER_H

#include <vector>
#include <string>
#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>

namespace deeplocalizer {

class DataReader;

class CaffeClassifier {
public:
    CaffeClassifier(const std::string &model_file,
                    const std::string &trained_file,
                    bool use_gpu = false,
                    size_t batch_size = 256,
                    caffe::Net<float>* weightsharing_net = nullptr
    );
    void setBatchSize(size_t batch_size);
    std::vector<int> topLabel(caffe::Blob<float>& blob);
    std::vector<std::vector<int>> topN(caffe::Blob<float>& blob, int N);
    std::vector<std::vector<float>> forward(caffe::Blob<float> &blob);
    inline std::vector<int> input_shape() {
        return {int(_batch_size), _num_channels, _input_geometry.height, _input_geometry.width};
    }
private:
    caffe::Net<float> _net;
    cv::Size _input_geometry;
    int _num_channels;
    size_t _batch_size;
    void reshapeNet();
};

class ConfusionMatrix {
    std::vector<std::vector<unsigned long>> _confMat;
    std::vector<std::vector<double>> toDouble();
    unsigned long totalExamples();
public:
    void add(const std::vector<int> & true_labels, const std::vector<int> & predicted_labels);
    void add(int ground_truth_idx, int predicted_idx);
    void print();
    inline const std::vector<std::vector<unsigned long>>& confMat() {
        return _confMat;
    }
};
}

#endif //DEEP_LOCALIZER_CAFFECLASSIFIER_H
