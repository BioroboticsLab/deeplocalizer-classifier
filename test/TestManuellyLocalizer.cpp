
#define CATCH_CONFIG_MAIN

#include <boost/format.hpp>

#include "catch.hpp"
#include "ManuellyLocalizer.h"

using namespace std;
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
                auto & img = localizer.getCurrentImage();
                auto & previous_tag = img.nextTag().get();
                boost::optional<Tag &> opt_tag;
                while(opt_tag = img.nextTag()) {
                    const Tag & tag = opt_tag.get();
                    cout << tag.getBoundingBox() << endl;
                    REQUIRE(tag.getBoundingBox() !=  previous_tag.getBoundingBox());
                    previous_tag = tag;
                }
            }
        }
    }
}

