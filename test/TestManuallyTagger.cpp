
#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

#include "catch.hpp"
#include "ManuallyTagger.h"

using namespace deeplocalizer::tagger;
namespace io = boost::filesystem;

TEST_CASE( "ManuallyTagger ", "[ManuallyTagger]" ) {
    std::vector<ImageDescription> image_descrs{
        ImageDescription{
            "testdata/with_5_tags.jpeg",
            std::vector<Tag>{
                Tag{
                    cv::Rect{},
                    pipeline::Ellipse{}
                }
            }
        }
    };

    SECTION( " it can be constructed with a list of image descriptions" ) {
        GIVEN( "not existing images" ) {
            THEN(" it will throw an exception") {
                std::vector<ImageDescription>wrong_paths{
                    ImageDescription("noexistend.png")
                };
                REQUIRE_THROWS(new ManuallyTagger(wrong_paths));
            }
        }
        GIVEN( " some paths to existend images" ) {
            THEN(" it will construct a instance with valid default members") {
                ManuallyTagger tagger(image_descrs);
                auto pimgs = tagger.getProposalImages();
                REQUIRE(std::equal(pimgs.cbegin(), pimgs.cend(),
                                   image_descrs.cbegin()));
            }
        }
    }

    SECTION( "sends signals" ) {
        ManuallyTagger * tagger = new ManuallyTagger(image_descrs);
        GIVEN("an index out of range") {
            THEN("it will emit the outOfRange signal") {
                bool outOfRangeEmitted = false;
                auto c = tagger->connect(tagger, &ManuallyTagger::outOfRange, [&](unsigned long idx){
                    REQUIRE(idx == ULONG_MAX);
                    outOfRangeEmitted = true;
                });
                tagger->loadImage(ULONG_MAX);
                REQUIRE(outOfRangeEmitted);
                tagger->disconnect(c);
            }
        }
        GIVEN("the first image") {
             THEN("it will emit the firstImage signal") {
                 bool firstImageEmitted = false;
                 auto c = tagger->connect(tagger, &ManuallyTagger::firstImage, [&]() {
                     firstImageEmitted = true;
                 });
                 tagger->loadImage(0);
                 REQUIRE(firstImageEmitted);
                 tagger->disconnect(c);
             }
        }
        GIVEN("the last image") {
            THEN("it will emit the lastImage signal") {
                bool lastImageEmitted = false;
                auto c = tagger->connect(tagger, &ManuallyTagger::lastImage, [&]() {
                    lastImageEmitted = true;
                });
                tagger->loadImage(0);
                REQUIRE(lastImageEmitted);
                tagger->disconnect(c);
            }
        }

        GIVEN("a call to the loadImage slot") {
            THEN("it will emit an loadedImage signal as soon the image is loaded") {
                bool loadedImageEmitted = false;
                auto c = tagger->connect(tagger, &ManuallyTagger::loadedImage,
                                         [&](ImageDescription * desc, Image * img) {
                    REQUIRE(*desc == tagger->getProposalImages().at(0));
                    REQUIRE(*img == Image(*desc));
                    loadedImageEmitted = true;
                });
                tagger->loadImage(0);
                REQUIRE(loadedImageEmitted);
                tagger->disconnect(c);
            }
        }
    }
}

int main( int argc, char** const argv )
{
    QCoreApplication * qapp = new QCoreApplication(argc, argv);
    int result = Catch::Session().run(argc, argv);
    return result;
}
