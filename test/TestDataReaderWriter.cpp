
#define CATCH_CONFIG_MAIN

#include <boost/filesystem.hpp>
#include <QPainter>

#include "ManuallyTagger.h"
#include "catch.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <utils.h>
#include <DataReader.h>
#include <caffe/util/io.hpp>
#include "TrainsetGenerator.h"

using namespace deeplocalizer::tagger;
using boost::optional;
using boost::none;
namespace io = boost::filesystem;

TEST_CASE( "TestDataReaderWriter", "" ) {
    Tag tag{cv::Rect{20, 20, TAG_WIDTH, TAG_HEIGHT}, none};
    if(not io::exists("tagger_images.txt")) {
        io::current_path(io::current_path() / "testdata");
    }
    std::vector<ImageDesc> cam2_descs =
            ImageDesc::fromPathFile("tagger_images.txt", ManuallyTagger::IMAGE_DESC_EXT);
    ImageDesc cam2_desc = cam2_descs.front();
    io::path unique_path = io::unique_path("/tmp/test_deeplocalizer/%%%%%%/");
    io::create_directories(unique_path);
    SECTION("Reader / Writer") {
        GIVEN("a ImageDesc") {
            THEN("Writer will save the samples and Reader will load it.") {
                TrainsetGenerator gen;
                std::vector<TrainDatum> data;
                gen.trueSamples(cam2_desc, data);
                gen.wrongSamples(cam2_desc, data);
                auto tags = cam2_desc.getTags();
                {
                    LMDBWriter lmdb((unique_path / "lmdb").string());
                    REQUIRE(not data.empty());
                    lmdb.write(data, Dataset::Train);
                }
                {
                    LMDBReader lmdb((unique_path / "lmdb" / "train").string());
                    std::vector<caffe::Datum> caffe_data = lmdb.read(size_t(20));
                    REQUIRE(caffe_data.at(0).encoded());
                    cv::Mat mat = caffe::DecodeDatumToCVMatNative(caffe_data.at(0));
                    REQUIRE(mat.cols == TAG_WIDTH);
                    REQUIRE(mat.rows == TAG_HEIGHT);
                }
                {
                    ImageWriter imageWriter((unique_path / "images").string());
                    REQUIRE(not data.empty());
                    imageWriter.write(data, Dataset::Train);
                }
                {
                    ImageReader imageReader((unique_path / "images" / "train"
                                             / "train.txt").string());
                    std::vector<caffe::Datum> caffe_data = imageReader.read(size_t(20));
                    REQUIRE(caffe_data.at(0).encoded());
                    cv::Mat mat = caffe::DecodeDatumToCVMatNative(caffe_data.at(0));
                    REQUIRE(mat.cols == TAG_WIDTH);
                    REQUIRE(mat.rows == TAG_HEIGHT);
                }
            }
        }
        io::remove_all(unique_path);
    }
}

