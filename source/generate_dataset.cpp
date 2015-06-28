
#include <QCoreApplication>
#include <boost/program_options.hpp>
#include <chrono>
#include <functional>
#include <thread>

#include <ManuallyTagger.h>
#include <TrainsetGenerator.h>

#include "ProposalGenerator.h"
#include "deeplocalizer.h"
#include "utils.h"

using namespace deeplocalizer::tagger;
using namespace std::chrono;
using boost::optional;
using boost::make_optional;
namespace po = boost::program_options;
namespace io = boost::filesystem;

po::options_description desc_option("Options");
po::positional_options_description positional_opt;

time_point<system_clock> start_time;

void setupOptions() {
    desc_option.add_options()
            ("help,h", "Print help messages")
            ("pathfile", po::value<std::vector<std::string>>(), "File with image paths")
            ("format,f", po::value<std::string>(), "Format either `lmdb`, `images` or `all`. Default is `lmdb`. "
                                                   "`all` will save it both with lmdb and as images.")
            ("samples-per-tag,s", po::value<unsigned int>(), "Number of rotated and translated images per tag. Must be a multiple of 4."
                    " Default is 32.")
            ("output-dir,o", po::value<std::string>(), "Output images to this directory");
    positional_opt.add("pathfile", 1);
}


int run(QCoreApplication &,
        std::string pathfile,
        Dataset::Format save_format,
        std::string output_dir,
        unsigned int samples_per_tag
) {
    const auto img_descs = ImageDesc::fromPathFile(pathfile, ManuallyTagger::IMAGE_DESC_EXT);
    TrainsetGenerator gen{
            DataWriter::fromSaveFormat(output_dir, save_format)
    };
    gen.samples_per_tag = samples_per_tag;
    gen.wrong_samples_per_tag = samples_per_tag;
    gen.processParallel(img_descs);
    return 0;
}

void printUsage() {
    std::cout << "Usage: generate_proposals [options] pathfile.txt "<< std::endl;
    std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
    std::cout << desc_option << std::endl;
}

int main(int argc, char* argv[])
{
    QCoreApplication qapp(argc, argv);
    deeplocalizer::registerQMetaTypes();
    setupOptions();
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc_option)
                      .positional(positional_opt).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        printUsage();
        return 0;
    }
    if(vm.count("pathfile") && vm.count("output-dir") && vm.count("format")) {
        auto format_str = vm.at("format").as<std::string>();
        auto opt_format = Dataset::parseFormat(format_str);
        if(not opt_format){
            std::cout << "No a valid Save format: " << format_str << std::endl;
            std::cout << "Save format must be either `lmdb`, `images` or `all`" << std::endl;
            return 1;
        }
        unsigned int samples_per_tag = 32;
        if(vm.count("samples-per-tag")) {
            samples_per_tag = vm.at("samples-per-tag").as<unsigned int>();
        }
        auto pathfile = vm.at("pathfile").as<std::vector<std::string>>().at(0);
        auto output_dir = vm.at("output-dir").as<std::string>();
        return run(qapp, pathfile, opt_format.get(), output_dir, samples_per_tag);
    } else {
        std::cout << "No pathfile, format or output directory given." << std::endl;
        printUsage();
        return 1;
    }
}
