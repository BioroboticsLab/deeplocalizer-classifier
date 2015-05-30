
#include <boost/program_options.hpp>

#include "Image.h"

using namespace deeplocalizer::tagger;
namespace po = boost::program_options;

// po::options_description desc_option("Options");
// po::positional_options_description positional_opt;

// void setupOptions() {
//     desc_option.add_options()
//             ("help", "Print help messages");
//     positional_opt.add("pathfile", 1);
// }

int main(int argc, char* argv[])
{
    po::options_description desc_option("Options");
    po::positional_options_description positional_opt;

    desc_option.add_options()
            ("help", "Print help messages")
            ("pathfile", po::value<std::vector<std::string>>(), "file with paths");

    positional_opt.add("pathfile", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc_option)
                      .positional(positional_opt).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << "Usage: add_border [options] pathfile.txt "<< std::endl;
        std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
        std::cout << desc_option << std::endl;
    }
    if(vm.count("pathfile")) {
        std::string pathfile = vm.at("pathfile").as<std::vector<std::string>>().at(0);
        auto image_descs = ImageDescription::fromPathFile(pathfile);

        for(auto & desc : image_descs) {
            std::cout << desc.filename.toStdString() << std::endl;
        }
    } else {
        std::cout << "no pathfile given" << std::endl;
        return 1;
    }
    return 0;
}
