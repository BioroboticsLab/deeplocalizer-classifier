

#include "Image.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Tag.h"

using namespace deeplocalizer::tagger;
using boost::optional;

TEST_CASE( "ImageDescription", "[ImageDescription]" ) {
    std::vector<Tag> tag_vec{
            Tag(cv::Rect{}, pipeline::Ellipse{}),
            Tag(cv::Rect{}, optional<pipeline::Ellipse>{})
    };
    SECTION("creation") {
        GIVEN("a filename") {
            QString a_filename{"some_image.png"};
            THEN("a image description can be constructed") {
                ImageDescription descr{a_filename};
                REQUIRE(descr.filename == a_filename);
            }
        }
        GIVEN("a filename and a tag vector") {
            QString a_filename{"some_image.png"};
            THEN("a image description can be constructed") {
                ImageDescription descr{a_filename, tag_vec};
                REQUIRE(descr.filename == a_filename);
                REQUIRE(descr.getTags() == tag_vec);
            }
        }

    }
}
