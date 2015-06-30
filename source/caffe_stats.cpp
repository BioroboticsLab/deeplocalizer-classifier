
#include <caffe/caffe.hpp>
#include <caffe/util/db.hpp>
#include <caffe/util/db_lmdb.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <thread>
#include <DataReader.h>

#include "Dataset.h"
#include "CaffeClassifier.h"

namespace po = boost::program_options;
using namespace caffe;
using namespace deeplocalizer;
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

// returns the number of collected examples.
size_t collectWrongExamples(const std::vector<int> &predicted_labels,
                            const std::vector<int> &labels,
                            const vector<Datum> &data,
                            const std::string &output_dir
) {
    size_t collected_examples = 0;
    for(unsigned int i = 0; i < predicted_labels.size(); i++) {
        unsigned int p = predicted_labels[i];
        unsigned int t = labels.at(i);
        if(p == t) {
            continue;
        }
        cv::Mat mat = DecodeDatumToCVMatNative(data.at(i));
        std::stringstream ss;
        ss << output_dir << "/predicted_" << p << "_actual_" << t << "/%%%%%%%%%%%%.jpeg";
        io::path path = io::unique_path(ss.str());
        io::create_directories(path.parent_path());
        cv::imwrite(path.string(), mat);
        collected_examples++;
    }
    return collected_examples;
}

int run(const std::string &model, const std::string trained_model,
        const std::string &data_dir,  const std::string &output_dir, bool use_gpu) {
    if(use_gpu) {
        std::cout << "using gpu" << std::endl;
    }
    size_t batch_size = 256;

    TransformationParameter trans_parameter {};
    trans_parameter.set_scale(float(1)/255);
    auto transformer = std::make_unique<DataTransformer<float>>(trans_parameter, TEST);
    CaffeClassifier classifier(model, trained_model, use_gpu, batch_size);
    ConfusionMatrix confMat;
    std::unique_ptr<DataReader> input = std::make_unique<LMDBReader>(data_dir,
                                                                     classifier.input_shape(),
                                                                     std::move(transformer));
    std::vector<int> true_labels;
    std::vector<caffe::Datum> data;
    caffe::Blob<float> blob;
    size_t collected_examples = 0;
    size_t max_collect = 256;

    for(size_t i = 0; input->read(blob, true_labels, data); i++) {
        if(i % 50 == 0) {
            std::cout << "Iteration #" << i << std::endl;
        }
        auto predicted_labels = classifier.topLabel(blob);
        confMat.add(true_labels, predicted_labels);
        if(collected_examples < max_collect) {
            collected_examples += collectWrongExamples(predicted_labels, true_labels,
                                                       data, output_dir);
        }
    }
    confMat.print();
    return 0;
}

void printUsage() {
    std::cout << "Usage: caffe_stats [options]"<< std::endl;
    std::cout << "    prints a confusion matrix and writes wrongly classified images to `--output-dir`."<< std::endl;
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
