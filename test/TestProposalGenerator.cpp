

#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <QTimer>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

#include "catch.hpp"
#include "ManuallyTagger.h"
#include "ProposalGenerator.h"
#include "deeplocalizer.h"

using namespace deeplocalizer::tagger;
namespace io = boost::filesystem;
using boost::optional;

static int exit_code = 1;
static int argc= 1;
static char ** argv;

static ProposalGenerator * gen;

void registerQuit(ProposalGenerator * gen) {
    gen->connect(gen,
                 &ProposalGenerator::finished, QCoreApplication::instance(),
                 [&]() {
                      QCoreApplication::instance()->exit(exit_code);
                 }, Qt::QueuedConnection);
}

TEST_CASE( "ProposalGenerator", "[ProposalGenerator]" ) {
    QCoreApplication * qapp = new QCoreApplication(argc, argv);
    std::vector<std::string> image_paths = {"testdata/with_5_tags.jpeg"};
    gen = new ProposalGenerator(image_paths);
    deeplocalizer::registerQMetaTypes();
    QTimer * timer = new QTimer(qapp);
    qapp->connect(timer, &QTimer::timeout, qapp,
            std::bind(&QCoreApplication::exit, exit_code),
            Qt::QueuedConnection);
    timer->start(20000);
    ImageDesc img("image_path.jpeg");
    Tag tag(cv::Rect(0, 0, 10, 20), optional<pipeline::Ellipse>());
    auto uniquePath = io::unique_path("/tmp/%%%%%%%%%%%.xml");
    registerQuit(gen);
    SECTION( "runs images through the pipeline" ) {
        bool finishedEmitted = false;
        GIVEN("it is finished with processing an image with bees") {
            THEN ("it will give you tags proposals "
                          "and the todo list will be empty") {
                gen->connect(gen, &ProposalGenerator::finished, [&]() {
                                 REQUIRE(not gen->getProposalImages().empty());
                                 auto &img = gen->getProposalImages().front();
                                 REQUIRE(img.getTags().size() > 0);
                                 finishedEmitted = true;
                });
                gen->connect(gen, &ProposalGenerator::finished, [&]() {
                    REQUIRE(gen->getBeforePipelineImages().empty());
                });
            }
        }
        gen->processPipeline();
        qapp->exec();
        REQUIRE(finishedEmitted);
    }
    SECTION("serialization") {
        bool finishedEmitted = false;
        GIVEN("an image with tags") {
            THEN(" the tags can be saved and reloaded") {
                gen->connect(gen, &ProposalGenerator::finished, [&]() {
                    std::cout << "finished called" << std::endl;
                    auto load_imgs = ImageDesc::fromPaths(image_paths);

                    REQUIRE(gen->getProposalImages().size() ==
                            load_imgs.size());
                    for (unsigned int i = 1;
                         i < gen->getProposalImages().size(); i++) {
                        auto gen_img = gen->getProposalImages().at(i);
                        auto load_img = load_imgs.at(i);
                        REQUIRE(gen_img == load_img);
                    }
                    io::remove(uniquePath);
                    finishedEmitted = true;
                });
                gen->processPipeline();
            }
        }
        gen->processPipeline();
        qapp->exec();
        REQUIRE(finishedEmitted);
    }
}

int main( int _argc, char** const _argv )
{
    argc = _argc;
    argv = _argv;
    exit_code = Catch::Session().run(argc, argv);
    return exit_code;
}
