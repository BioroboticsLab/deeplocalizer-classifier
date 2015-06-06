
#include <QCoreApplication>
#include <boost/program_options.hpp>
#include <chrono>

#include "ProposalGenerator.h"
#include "deeplocalizer.h"

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
            ("out,o", po::value<std::string>(), "Write proposals to this file")
            ("pathfile", po::value<std::vector<std::string>>(), "File with image paths");

    positional_opt.add("pathfile", 1);
}

void printProgress(double progress) {
    using std::cout;
    int width = 40;
    duration<double> elapsed = system_clock::now() - start_time;
    unsigned long progress_chars = std::lround(width*progress);
    auto crosses = std::string(progress_chars, '#');
    auto spaces = std::string(width-progress_chars, ' ');

    cout << "\r " << static_cast<int>(progress*100) << "% ["
        << crosses << spaces << "] ";
    if(progress > 0.05) {
        auto eta = elapsed / progress - elapsed;
        auto h = duration_cast<hours>(eta).count();
        auto m = duration_cast<minutes>(eta).count() - 60 * h;
        auto s = duration_cast<seconds>(eta).count() - 60 * m;
        cout << "eta ";
        if (h)
            cout << h << "h ";
        if (h || m)
            cout << m << "m ";
        cout << s << "s";
    }
    cout << "          " << std::flush;
}

int run(QCoreApplication & qapp,
        std::string output_file,
        std::string pathfile

) {
    auto images_todo = ImageDesc::fromPathFile(pathfile);
    std::vector<ImageDesc> images_done;
    if (io::exists(output_file)) {
        auto deque_images_done = ImageDesc::loads(output_file);
        images_done = std::vector<ImageDesc>(deque_images_done->cbegin(),
                                                    deque_images_done->cend());
        for(auto & d : images_done) {
            images_todo.erase(
                std::remove_if(
                    images_todo.begin(), images_todo.end(),
                    [&d](auto & t) {
                        return d.filename == t.filename;
                    }),
                images_todo.end());
        }
        std::cout << "Found " << images_done.size() << " image in "  << output_file << std::endl;
        std::cout << std::endl;
        if (images_todo.empty()) {
            std::cout << "Nothing to do, images are allready processed" << std::endl;
            return 0;
        }
    }
    auto gen = new ProposalGenerator(images_todo, images_done);
    std::function<void()>save_fn = std::bind(&ProposalGenerator::saveProposals,
                                             gen, output_file);

    gen->connect(gen, &ProposalGenerator::progress, &printProgress);
    gen->connect(gen, &ProposalGenerator::progress, save_fn);
    gen->connect(gen, &ProposalGenerator::finished, save_fn);
    gen->connect(gen, &ProposalGenerator::finished,
                &qapp, &QCoreApplication::quit, Qt::QueuedConnection);

    printProgress(0);
    start_time = system_clock::now();
    gen->processPipeline();
    return qapp.exec();
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
    if(vm.count("pathfile") && vm.count("out")) {
        auto out = vm.at("out").as<std::string>();
        auto pathfile = vm.at("pathfile").as<std::vector<std::string>>().at(0);
        return run(qapp, out, pathfile);
    } else {
        std::cout << "No pathfile or output directory given." << std::endl;
        printUsage();
        return 1;
    }
}
