
#include "ManuallyTagger.h"

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "pipeline/Localizer.h"
#include "serialization.h"
#include "utils.h"

namespace deeplocalizer {
namespace tagger {

using boost::optional;
namespace io = boost::filesystem;


const std::string ManuallyTagger::DEFAULT_SAVE_PATH = "tagger_progress.binary";


ManuallyTagger::ManuallyTagger() {  }

ManuallyTagger::ManuallyTagger(const std::vector<ImageDesc> & descriptions,
                               const std::string & save_path) :
    _save_path(save_path)
{
    for(auto & descr : descriptions ) {
        ASSERT(io::exists(descr.filename),
               "Could not open file " << descr.filename);
        _image_descs.push_back(std::make_shared<ImageDesc>(descr));
    }
}

ManuallyTagger::ManuallyTagger(const std::vector<ImageDescPtr> & descriptions,
                               const std::string & save_path) :
    _save_path(save_path)
{
    for(auto & descr : descriptions ) {
        ASSERT(io::exists(descr->filename),
               "Could not open file " << descr->filename);
        _image_descs.push_back(descr);
    }
}

ManuallyTagger::ManuallyTagger(std::vector<ImageDescPtr> && descriptions,
                               const std::string & save_path) :
    _image_descs(std::move(descriptions)),
    _save_path(save_path)
{ }

void ManuallyTagger::save() const {
    save(savePath());
}
void ManuallyTagger::save(const std::string & path) const {
    safe_serialization(path, boost::serialization::make_nvp("tagger", *this));
}

std::unique_ptr<ManuallyTagger> ManuallyTagger::load(const std::string & path) {
    std::ifstream is(path);
    boost::archive::binary_iarchive archive(is);
    ManuallyTagger * tagger;
    archive >> boost::serialization::make_nvp("tagger", tagger);
    return std::unique_ptr<ManuallyTagger>(tagger);
}
void ManuallyTagger::loadNextImage() {
    loadImage(_image_idx + 1);
}

void ManuallyTagger::loadLastImage() {
    if (_image_idx == 0) { return; }
    loadImage(_image_idx - 1);
}

void ManuallyTagger::loadImage(unsigned long idx) {
    if (idx >= _image_descs.size()) {
        emit outOfRange(idx);
        return;
    }
    _image_idx = idx;
    _image = std::make_shared<Image>(*_image_descs.at(_image_idx));
    _desc = _image_descs.at(_image_idx);
    emit loadedImage(_desc, _image);
    if (_image_idx == 0) { emit firstImage(); }
    if (_image_idx + 1 == _image_descs.size()) { emit lastImage(); }
}
void ManuallyTagger::doneTagging(unsigned long idx) {
    if (_done_tagging.size() <= idx) {
        qWarning() << "[doneTagging] index " << idx << " exceeded size of images "
            << _done_tagging.size();
    }
    _image_descs.at(idx)->save();
    save();
    _done_tagging.at(idx) = true;
}
}
}
