
#define CATCH_CONFIG_MAIN

#include <boost/filesystem.hpp>
#include <QPainter>

#include "ManuallyTagger.h"
#include "catch.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <utils.h>
#include "TrainsetGenerator.h"

using namespace deeplocalizer::tagger;
using boost::optional;
using boost::none;
namespace io = boost::filesystem;

TEST_CASE( "TrainsetGenerator", "" ) {
    Tag tag{cv::Rect{20, 20, TAG_WIDTH, TAG_HEIGHT}, none};
    if(not io::exists("tagger_images.txt")) {
        io::current_path(io::current_path() / "testdata");
    }
    ImageDesc img_desc{"one_tag_at_center.jpeg"};
    std::vector<ImageDesc> cam2_descs =
            ImageDesc::fromPathFile("tagger_images.txt", ManuallyTagger::IMAGE_DESC_EXT);
    ImageDesc cam2_desc = cam2_descs.front();
    std::vector<ImageDesc> img_descs{img_desc};
    Image img{img_desc};
    SECTION("trueSamples") {
        TrainsetGenerator gen;
        std::vector<TrainDatum> data;
        auto tags = cam2_desc.getTags();
        long n_yes = std::count_if(tags.cbegin(), tags.cend(),
                                   std::mem_fn(&Tag::isYes));
        GIVEN("a ImageDesc") {
            THEN("it will transform yes tags and randomly generate no tags") {
                gen.trueSamples(cam2_desc, data);
                REQUIRE(not data.empty());
                size_t n_true_samples = data.size();
                REQUIRE(n_true_samples == n_yes * gen.samples_per_tag);
                REQUIRE(std::all_of(data.begin(), data.end(), [](const auto & d) {
                    return d.tag().isTag() == IsTag::Yes;
                }));


                gen.wrongSamples(cam2_desc, data);
                REQUIRE(not data.empty());
                size_t n_wrong_samples = data.size() - n_true_samples;
                REQUIRE(n_wrong_samples == n_yes * gen.samples_per_tag);
            }
        }
        Image cam2_img(cam2_desc);

        QImage qimage = cvMatToQImage(cam2_img.getCvMat());
        QPainter painter(&qimage);

        for(auto & d: data) {
            // pick only one 1/8th of the tags. It would be otherwise to dense.
            if(rand() % 8 == 0) {
                d.draw(painter);
            }
        }
        qimage.save("test_trainset_generator.jpeg");
    }
}
