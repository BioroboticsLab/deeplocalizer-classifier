
#define CATCH_CONFIG_RUNNER

#include <QCoreApplication>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

#include "catch.hpp"
#include "ManuellyTagger.h"

using namespace deeplocalizer::tagger;
namespace io = boost::filesystem;

TEST_CASE( "ManuellyTagger ", "[ManuellyTagger]" ) {
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
                REQUIRE_THROWS(new ManuellyTagger(wrong_paths));
            }
        }
        GIVEN( " some paths to existend images" ) {
            THEN(" it will construct a instance with valid default members") {
                ManuellyTagger tagger(image_descrs);
                auto pimgs = tagger.getProposalImages();
                REQUIRE(std::equal(pimgs.cbegin(), pimgs.cend(),
                                   image_descrs.cbegin()));
            }
        }
    }

    SECTION( "gives you images descriptions" ) {
        ManuellyTagger gen(image_descrs);
        GIVEN( "one image with bees" ) {
            THEN ( "it will give you only one image") {
                auto img_descr = gen.nextImageDescr();
                REQUIRE(img_descr == image_descrs.at(0));
            }
        }
        GIVEN( "multiple images" ) {
            THEN ( "it will give you multiple images") {
                int n = 5;
                std::vector<ImageDescription> n_descrs;
                cv::Mat mat{cv::Size{64, 64}, CV_8U, cv::Scalar(0)};
                for(int i = 0; i < n; i++) {
                    io::path path = io::unique_path("/tmp/%%%%%%%%%%%%.png");
                    cv::imwrite(path.string(), mat);
                    auto qpath = QString::fromStdString(path.string());
                    n_descrs.push_back(ImageDescription{qpath});
                }

                ManuellyTagger n_path_loc(n_descrs);

                for(int i = 0; i < n; i++) {
                    INFO("AT i: " << i);
                    auto img = n_path_loc.nextImageDescr();
                    REQUIRE(img.filename.toStdString() ==
                            n_descrs.at(i).filename.toStdString());
                    io::remove(img.filename.toStdString());
                }
                // REQUIRE_THROWS(n_path_loc.nextImageDescr());
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
