

#include "Image.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace deeplocalizer::tagger;
using boost::optional;

namespace io = boost::filesystem;

TEST_CASE( "ImageDesc", "[ImageDesc]" ) {
    std::vector<Tag> tag_vec{
            Tag(cv::Rect{}, pipeline::Ellipse{}),
            Tag(cv::Rect{}, optional<pipeline::Ellipse>{})
    };
    auto uniquePath = io::unique_path("/tmp/%%%%%%%%%%%.binary");
    SECTION("creation") {
        GIVEN("a filename") {
            QString a_filename{"some_image.png"};
            THEN("a image description can be constructed") {
                ImageDesc descr{a_filename};
                REQUIRE(descr.filename == a_filename);
            }
        }
        GIVEN("a filename and a tag vector") {
            QString a_filename{"some_image.png"};
            THEN("a image description can be constructed") {
                ImageDesc descr{a_filename, tag_vec};
                REQUIRE(descr.filename == a_filename);
                REQUIRE(descr.getTags() == tag_vec);
            }
        }
    }
    SECTION( "Serialization" ) {
        GIVEN( "an image description" ) {
            THEN("it can be saved and loaded") {
                QString filename = QString::fromStdString(uniquePath.string());
                ImageDesc desc{filename, tag_vec};
                desc.save();
                ImageDescPtr loaded_desc = ImageDesc::load(desc.save_path());
                REQUIRE(desc.filename== loaded_desc->filename);
            }
        }
        io::remove(uniquePath);
    }
}
