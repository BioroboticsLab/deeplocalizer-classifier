
#include <QCoreApplication>
#include <boost/program_options.hpp>
#include <chrono>

#include "ProposalGenerator.h"
#include "Image.h"

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
            ("out,o", po::value<std::string>(), "Write proposals to this file")
            ("pathfile", po::value<std::vector<std::string>>(), "File with image paths");

    positional_opt.add("pathfile", 1);
}

void printProgress(double progress) {
    int width = 40;
    duration<double> elapsed = system_clock::now() - start_time;
    auto eta = elapsed / progress - elapsed;
    long progress_chars = std::lround(width*progress);
    auto crosses = std::string(progress_chars, '#');
    auto spaces = std::string(width-progress_chars, ' ');

    auto h = duration_cast<hours>(eta).count();
    auto m = duration_cast<minutes>(eta).count() - 60*h;
    auto s = duration_cast<seconds>(eta).count() - 60*m;
    std::cout << "\r " << static_cast<int>(progress*100) << "% ["
              << crosses << spaces << "] eta ";
    if(h)
        std::cout << h << "h ";
    if(h || m)
        std::cout << m << "m ";

    std::cout << s << "s      " << std::flush;
}

int run(QCoreApplication & qapp, std::string pathfile, std::string output_file) {
    auto descrs = ImageDescription::fromPathFile(pathfile);
    ProposalGenerator * gen = new ProposalGenerator(descrs);

    std::function<void()>save_fn = std::bind(&ProposalGenerator::save, gen, output_file);
    gen->connect(gen, &ProposalGenerator::progress, &printProgress);
    gen->connect(gen, &ProposalGenerator::progress, save_fn);
    gen->connect(gen, &ProposalGenerator::finished, save_fn);
    gen->connect(gen, &ProposalGenerator::finished,
                &qapp, &QCoreApplication::quit, Qt::QueuedConnection);

    start_time = system_clock::now();
    gen->processPipeline();
    return qapp.exec();
}

int main(int argc, char* argv[])
{
    QCoreApplication qapp(argc, argv);
    qRegisterMetaType<ImageDescription>("ImageDescription");
    setupOptions();
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc_option)
                      .positional(positional_opt).run(), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << "Usage: generate_proposals [options] pathfile.txt "<< std::endl;
        std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
        std::cout << desc_option << std::endl;
        return 0;
    }
    if(vm.count("pathfile") && vm.count("out")) {
        std::string pathfile =
                vm.at("pathfile").as<std::vector<std::string>>().at(0);
        return run(qapp, pathfile, vm.at("out").as<std::string>());
    } else {
        std::cout << "No pathfile or output_dir are given" << std::endl;
        std::cout << "Usage: add_border [options] pathfile.txt "<< std::endl;
        std::cout << "    where pathfile.txt contains paths to images."<< std::endl;
        std::cout << desc_option << std::endl;
        return 0;
    }
    return 0;
}
