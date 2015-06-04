
#include "ManuallyTaggerWindow.h"
#include "deeplocalizer.h"
#include <QApplication>
#include <boost/program_options.hpp>

using namespace deeplocalizer::tagger;

namespace po = boost::program_options;
namespace io = boost::filesystem;

po::options_description desc_option("Options");
po::positional_options_description positional_opt;

void setupOptions() {
    desc_option.add_options()
            ("help,h", "Print help messages")
            ("out,o", po::value<std::string>(), "Write tagging result to this file")
            ("proposals", po::value<std::vector<std::string>>(), "File with tags proposals");

    positional_opt.add("proposals", 1);
}
int run(QApplication & qapp, std::string proposals_file, std::string out_file) {
    auto proposals = ImageDescription::loads(proposals_file);
    ManuallyTagWindow window(std::move(*proposals));
    window.show();
    return qapp.exec();
}

void printUsage() {
    std::cout << "Usage: generate_proposals [options] pathfile.txt "<< std::endl;
    std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
    std::cout << desc_option << std::endl;
}
int main(int argc, char* argv[])
{
    QApplication qapp(argc, argv);
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
    if(vm.count("proposals") && vm.count("out")) {
        auto out = vm.at("out").as<std::string>();
        auto proposals = vm.at("proposals").as<std::vector<std::string>>().at(0);
        return run(qapp, proposals, out);
    } else {
        std::cout << "No proposal file or output directory given." << std::endl;
        printUsage();
        return 1;
    }
}
