
#define CATCH_CONFIG_MAIN

#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>

#include "catch.hpp"
#include "ManuellyLocalizer.h"

using namespace std;
using namespace boost;
using namespace deeplocalizer::tagger;


TEST_CASE( "ManuellyLocalizer ", "[ManuellyLocalizer]" ) {
    vector<QString> image_paths = {QString("testdata/with_5_tags.jpeg")};

    SECTION( " it can be constructed with a list of image names" ) {
        GIVEN( " none existend files" ) {
            THEN(" it will throw an exception") {
                vector<QString> wrong_paths = {"noexistend.png"};
                REQUIRE_THROWS(new ManuellyLocalizer(wrong_paths));
            }
        }
        GIVEN( " some paths to existend images" ) {
            THEN(" it will construct a instance with valid default members") {
                ManuellyLocalizer localizer(image_paths);
                REQUIRE( localizer.getImagePaths() == image_paths );
                REQUIRE( image_paths.size() == 1 );
            }
        }
    }

    SECTION( "gives you images" ) {
        ManuellyLocalizer localizer(image_paths);
        GIVEN( "one image with bees" ) {
            THEN ( "it will give you only one image") {
                boost::optional<Image &> opt_image = localizer.nextImage();
                REQUIRE(opt_image.is_initialized());
                Image &img = opt_image.get();
                REQUIRE(img.filename == image_paths.at(0));
                opt_image = localizer.nextImage();
                REQUIRE(! opt_image.is_initialized());
            }
        }
        GIVEN( "multiple images" ) {
            THEN ( "it will give you multiple images") {
                int n = 5;
                std::vector<QString> n_paths;
                cv::Mat img(128, 128, CV_8U, cv::Scalar(0));
                for(int i = 0; i < n; i++) {
                    filesystem::path path = filesystem::unique_path();
                    path.replace_extension(".png");
                    INFO(path.string());
                    cv::imwrite(path.string(), img);
                    n_paths.push_back(QString::fromStdString(path.string()));
                }
                localizer = ManuellyLocalizer(n_paths);
                for(int i = 0; i < n; i++) {
                    INFO("AT i: " << i);
                    auto img = localizer.nextImage();
                    REQUIRE(img.is_initialized());
                }
                REQUIRE(! localizer.nextImage().is_initialized());
            }
        }
    }
    SECTION( "gives you tags" ) {
        ManuellyLocalizer localizer(image_paths);
        GIVEN( "an image with bees" ) {
            THEN ( "it will give you at least one tag") {
                REQUIRE(localizer.getImagePaths() == image_paths);
                boost::optional<Image &> opt_image = localizer.nextImage();
                REQUIRE(opt_image.is_initialized());
                Image &img = opt_image.get();
                auto opt_tag = img.nextTag();
                REQUIRE(opt_tag.is_initialized());
            }
            THEN ( "it will give you different tags") {
                auto & img = localizer.nextImage().get();
                auto & previous_tag = img.nextTag().get();
                optional<std::shared_ptr<Tag>> opt_tag;
                while((opt_tag = img.nextTag())) {
                    auto tag = opt_tag.get();
                    REQUIRE(tag->getBoundingBox() !=  previous_tag->getBoundingBox());
                    previous_tag = tag;
                }
            }
        }
    }
}

