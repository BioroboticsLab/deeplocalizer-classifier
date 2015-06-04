#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <QThread>
#include <source/utils.h>

#include "catch.hpp"
#include "Image.h"
#include "ProposalGenerator.h"
#include "deeplocalizer.h"

namespace io = boost::filesystem;
using boost::optional;
using boost::serialization::make_nvp;
using namespace deeplocalizer::tagger;

static int exit_code = 0;

void registerQuit(ProposalGenerator * gen) {
    gen->connect(gen,
                 &ProposalGenerator::finished, QCoreApplication::instance(),
                 [=]() {
                     QCoreApplication::instance()->exit(exit_code);
                 }, Qt::QueuedConnection);
}


TEST_CASE( "Serialization", "[serialize]" ) {
    ImageDescription img("image_path.jpeg");
    Tag tag(cv::Rect(0, 0, 10, 20), optional<pipeline::Ellipse>());
    auto uniquePath = io::unique_path("/tmp/%%%%%%%%%%%.xml");
    SECTION( "ImageDescription" ) {
        INFO(uniquePath.string());
        GIVEN( "an image" ) {
            THEN("it can be serialized and deserialized") {
                {
                    std::ofstream os{uniquePath.string()};
                    REQUIRE(os.good());
                    boost::archive::xml_oarchive oa(os);
                    oa << BOOST_SERIALIZATION_NVP(img);
                }
                {
                    std::ifstream is{uniquePath.string()};
                    REQUIRE(is.good());
                    ImageDescription load_img;
                    boost::archive::xml_iarchive ia(is);
                    ia >> make_nvp("img", load_img);
                    REQUIRE(load_img.filename == "image_path.jpeg");
                }
            }
        }
        GIVEN( "an vector of image descriptions" ) {
            std::deque<ImageDescription> images{img};
            THEN("it can be saved and loaded") {
                ImageDescription::saves(uniquePath.string(), &images);
                auto loaded_imgs = ImageDescription::loads(uniquePath.string());
                REQUIRE(loaded_imgs->size() == 1);
                REQUIRE(loaded_imgs->at(0) == img);
            }
        }
        io::remove(uniquePath);
    }
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
    exit_code = Catch::Session().run(argc, argv);
    return exit_code;
}
