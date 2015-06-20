
#include <QCoreApplication>
#include <boost/program_options.hpp>
#include <chrono>
#include <functional>
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
                                                   "`all` will save it both with lmdb and as images."
            )
            ("output-dir,o", po::value<std::string>(), "Output images to this directory");
    positional_opt.add("pathfile", 1);
}


int run(QCoreApplication & qapp,
        std::string pathfile,
        Dataset::SaveFormat save_format,
        std::string output_dir
) {
    auto img_descs = ImageDesc::fromPathFile(pathfile, ManuallyTagger::IMAGE_DESC_EXT);
    std::shared_ptr<DatasetWriter> writer;
    switch (save_format) {
        case Dataset::SaveFormat::All:
           writer = std::make_shared<AllFormatWriter>(output_dir); break;
        case Dataset::SaveFormat::Images:
            writer = std::make_shared<ImageDatasetWriter>(output_dir); break;
        case Dataset::SaveFormat::LMDB:
            writer = std::make_shared<LMDBDatasetWriter>(output_dir); break;
    }
    TrainsetGenerator gen{writer};
    start_time = system_clock::now();
    auto printProgressFn = std::bind(&printProgress<system_clock>, std::cref(start_time),
                                     std::placeholders::_1);
    gen.connect(&gen, &TrainsetGenerator::progress, printProgressFn);
    printProgress(start_time, 0);
    gen.process(img_descs);
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
    if(vm.count("pathfile") && vm.count("output-dir")) {
        auto save_format = Dataset::SaveFormat::LMDB;
        if(vm.count("save-format")) {
            std::string save_format_str = vm.at("save-format").as<std::string>();
            if(save_format_str == "images") {
                save_format = Dataset::SaveFormat::Images;
            } else if (save_format_str == "all") {
                save_format = Dataset::SaveFormat::All;
            } else if (save_format_str == "lmdb") {
                save_format = Dataset::SaveFormat::LMDB;
            } else {
                std::cout << "No a valid Save format: " << save_format_str << std::endl;
            }
        }
        auto pathfile = vm.at("pathfile").as<std::vector<std::string>>().at(0);
        auto output_dir = vm.at("output-dir").as<std::string>();
        return run(qapp, pathfile, save_format, output_dir);
    } else {
        std::cout << "No pathfile or output directory given." << std::endl;
        printUsage();
        return 1;
    }
}
