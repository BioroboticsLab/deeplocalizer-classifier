

#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

#include "catch.hpp"
#include "ManuellyTagger.h"
#include "ProposalGenerator.h"

using namespace deeplocalizer::tagger;
using boost::optional;

static int exit_code = 0;

void registerQuit(ProposalGenerator * gen) {
    gen->connect(gen,
                 &ProposalGenerator::finished, QCoreApplication::instance(),
                 [=]() {
                      QCoreApplication::instance()->exit(exit_code);
                 }, Qt::QueuedConnection);
}

TEST_CASE( "ProposalGenerator", "[ProposalGenerator]" ) {
    std::vector<QString> image_paths = {QString("testdata/with_5_tags.jpeg")};

    SECTION( "runs images through the pipeline" ) {
        auto gen = std::shared_ptr<ProposalGenerator>(new ProposalGenerator(image_paths));
        registerQuit(gen.get());

        GIVEN( "it is finished with processing an image with bees" ) {
            THEN ( "it will give you tags proposals") {
                gen->connect(gen.get(), &ProposalGenerator::finished, [&]() {
                    REQUIRE(not gen->getProposalImages().empty() );
                    const auto & img = gen->getProposalImages().front();
                    REQUIRE(img->getTags().size() > 0);
                });
            }
            THEN ( "the todo list will be empty") {
                gen->connect(gen.get(), &ProposalGenerator::finished, [&]() {
                    REQUIRE( gen->getBeforePipelineImages().empty() );
                });
            }
        }
        gen->processPipeline();
    }
}

int main( int argc, char** const argv )
{
    QCoreApplication * qapp = new QCoreApplication(argc, argv);
    exit_code = Catch::Session().run(argc, argv);
    return qapp->exec();
}
