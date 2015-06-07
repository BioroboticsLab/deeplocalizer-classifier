#include <boost/program_options.hpp>

#include <chrono>
#include "Image.h"
#include "utils.h"

using namespace deeplocalizer::tagger;
using namespace std::chrono;
namespace po = boost::program_options;
namespace io = boost::filesystem;

po::options_description desc_option("Options");
po::positional_options_description positional_opt;

time_point<system_clock> start_time;

void setupOptions() {
    desc_option.add_options()
            ("help,h", "Print help messages")
            ("output_dir,o", po::value<std::string>(), "Write images to this directory")
            ("pathfile", po::value<std::vector<std::string>>(), "File with paths");

    positional_opt.add("pathfile", 1);
}

io::path addWb(io::path filename) {
    io::path output_path(filename);
    auto extension = output_path.extension();
    output_path.replace_extension();
    output_path += "_wb" + extension.string();
    return output_path;
}

int run(const std::vector<ImageDesc> image_descs, io::path & output_dir) {
    io::create_directories(output_dir);
    start_time = system_clock::now();
    printProgress(start_time, 0);
    for (unsigned int i = 0; i < image_descs.size(); i++) {
        const ImageDesc & desc = image_descs.at(i);
        Image img(desc);
        img.addBorder();
        auto input_path =  io::path(desc.filename);
        auto output = addWb(output_dir / input_path.filename());
        if(not img.write(output)) {
            std::cerr << "Fail to write image : " << output.string() << std::endl;
            return 1;
        }
        printProgress(start_time, static_cast<double>(i+1)/image_descs.size());
    }
    return 0;
}
int main(int argc, char* argv[])
{
    setupOptions();
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc_option)
                      .positional(positional_opt).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << "Usage: add_border [options] pathfile.txt "<< std::endl;
        std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
        std::cout << desc_option << std::endl;
        return 0;
    }
    if(vm.count("pathfile") && vm.count("output_dir")) {
        std::string pathfile =
                vm.at("pathfile").as<std::vector<std::string>>().at(0);
        auto image_descs = ImageDesc::fromPathFile(pathfile);
        auto output_dir = io::path(vm.at("output_dir").as<std::string>());
        run(image_descs, output_dir);
    } else {
        std::cout << "No pathfile or output_dir are given" << std::endl;
        std::cout << "Usage: add_border [options] pathfile.txt "<< std::endl;
        std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
        std::cout << desc_option << std::endl;
        return 0;
    }
    return 0;
}
