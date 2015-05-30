
#include "ManuellyTagger.h"

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "pipeline/Localizer.h"
#include "serialization.h"
#include "utils.h"

using namespace deeplocalizer::tagger;
using boost::optional;
namespace io = boost::filesystem;
ManuellyTagger::ManuellyTagger() {  }

ManuellyTagger::ManuellyTagger(const std::vector<ImageDescription> & img_descr)
{
    for(const auto & descr : img_descr ) {
        ASSERT(io::exists(descr.filename.toStdString()),
               "Could not open file " << descr.filename.toStdString());
        _images_with_proposals.push_back(descr);
    }
}

ImageDescription ManuellyTagger::nextImageDescr() {
    ASSERT(_images_with_proposals.size() > 0,
           "no more images left to classify. Feel Good, you are done!");
    ImageDescription img = _images_with_proposals.front();
    _images_with_proposals.pop_front();
    return img;
}

void ManuellyTagger::addVerified(ImageDescription && img) {
    _images_verified.push_back(img);
}

void ManuellyTagger::save(const std::string & path) const {
    std::ofstream os(path);
    boost::archive::xml_oarchive archive(os);
    archive << boost::serialization::make_nvp("tagger", *this);
}

std::unique_ptr<ManuellyTagger> ManuellyTagger::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::xml_iarchive archive(is);
    ManuellyTagger * tagger = new ManuellyTagger;
    archive >> boost::serialization::make_nvp("tagger", tagger);
    return std::unique_ptr<ManuellyTagger>(tagger);
}
