#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <source/utils.h>

#include "catch.hpp"
#include "Image.h"
#include "ProposalGenerator.h"
#include "deeplocalizer.h"

namespace io = boost::filesystem;
using boost::optional;
using boost::serialization::make_nvp;
using namespace deeplocalizer::tagger;

TEST_CASE( "Serialization", "[serialize]" ) {
    ImageDesc img("image_path.jpeg");
    Tag tag(cv::Rect(0, 0, 10, 20), optional<pipeline::Ellipse>());
    auto uniquePath = io::unique_path("/tmp/%%%%%%%%%%%.xml");
    SECTION( "Tag" ) {
        INFO(uniquePath.string());
        GIVEN( "a tag" ) {
            THEN("it can be serialized and deserialized") {
                {
                    std::ofstream os{uniquePath.string()};
                    REQUIRE(os.good());
                    boost::archive::xml_oarchive oa(os);
                    oa << BOOST_SERIALIZATION_NVP(tag);
                }
                {
                    std::ifstream is{uniquePath.string()};
                    Tag loaded_tag;
                    REQUIRE(is.good());
                    boost::archive::xml_iarchive ia(is);
                    ia >> make_nvp("tag", loaded_tag);
                    REQUIRE(loaded_tag == tag);
                }
            }
        }
        io::remove(uniquePath);
    }
}

int main( int argc, char** const argv )
{
    QCoreApplication * qapp = new QCoreApplication(argc, argv);
    deeplocalizer::registerQMetaTypes();
    int exit_code = Catch::Session().run(argc, argv);
    return exit_code;
}
