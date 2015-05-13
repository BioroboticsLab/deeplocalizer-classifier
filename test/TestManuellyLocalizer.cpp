
#define CATCH_CONFIG_MAIN

#include <boost/format.hpp>

#include "catch.hpp"
#include "ManuellyLocalizer.h"

using namespace std;
using namespace pipeline;
using namespace deep_localizer::manually;

TEST_CASE( "ManuellyLocalizer ", "[ManuellyLocalizer]" ) {
    vector<string> image_paths;

    SECTION( " it can be constructed with a list of image names" ) {
        GIVEN( " none existend files" ) {
            THEN(" it will throw an exception") {
                image_paths.push_back("no existend image");
                REQUIRE_THROWS(new ManuellyLocalizer(image_paths));
                image_paths.pop_back();
            }
        }
        GIVEN( " some paths to existend images" ) {
            THEN(" it will construct a instance with valid default members") {
                image_paths.push_back("testdata/Cam_0_20140804152006_3.jpeg");
                ManuellyLocalizer localizer(image_paths);
                REQUIRE( localizer.getImagePaths() == image_paths );
                REQUIRE( image_paths.size() == 1 );
            }
        }
    }

    SECTION( " it has gives you tags" ) {
        image_paths.push_back("testdata/Cam_0_20140804152006_3.jpeg");
        ManuellyLocalizer localizer(image_paths);
        REQUIRE( localizer.getImagePaths() == image_paths );
        int i = 0;
        boost::optional<Tag> opt_tag;

        while(opt_tag = localizer.next_tag()) i++;
        // Find at least 50 Tags candidates
        REQUIRE( i > 20 );
    }
}

