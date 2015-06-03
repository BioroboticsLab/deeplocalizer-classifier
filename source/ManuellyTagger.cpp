
#include "ManuellyTagger.h"

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "pipeline/Localizer.h"
#include "serialization.h"
#include "utils.h"

namespace deeplocalizer {
namespace tagger {

using boost::optional;
namespace io = boost::filesystem;

ManuellyTagger::ManuellyTagger() {  }

ManuellyTagger::ManuellyTagger(std::deque<ImageDescription> && descriptions) :
    _image_descs(std::move(descriptions))
{ }

ManuellyTagger::ManuellyTagger(const std::vector<ImageDescription> & img_descr)
{
    for(const auto & descr : img_descr ) {
        ASSERT(io::exists(descr.filename.toStdString()),
               "Could not open file " << descr.filename.toStdString());
        _image_descs.push_back(descr);
    }
}

void ManuellyTagger::save(const QString & path) const {
    std::ofstream os(path.toStdString());
    boost::archive::xml_oarchive archive(os);
    archive << boost::serialization::make_nvp("tagger", *this);
}

std::unique_ptr<ManuellyTagger> ManuellyTagger::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::xml_iarchive archive(is);
    ManuellyTagger * tagger;
    archive >> boost::serialization::make_nvp("tagger", tagger);
    return std::unique_ptr<ManuellyTagger>(tagger);
}
void ManuellyTagger::loadNextImage() {
    loadImage(_image_idx + 1);
}

void ManuellyTagger::loadLastImage() {
    if (_image_idx == 0) { return; }
    loadImage(_image_idx - 1);
}

void ManuellyTagger::loadImage(unsigned long idx) {
    if (idx >= _image_descs.size()) {
        emit outOfRange(idx);
        return;
    }
    _image_idx = idx;
    _image = Image(_image_descs.at(_image_idx));
    _desc = &_image_descs.at(_image_idx);
    emit loadedImage(_desc, &_image);
    if (_image_idx == 0) { emit firstImage(); }
    if (_image_idx + 1 == _image_descs.size()) { emit lastImage(); }
}
}
}
