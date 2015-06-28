#include <lmdb.h>
#include <iomanip>
#include "utils.h"
#include "Dataset.h"

namespace deeplocalizer {
namespace tagger {

namespace io = boost::filesystem;

boost::optional<Dataset::Format> Dataset::parseFormat(const std::string &str) {
    if(str == "images") {
        return Dataset::Format::Images;
    } else if (str == "all") {
        return Dataset::Format::All;
    } else if (str == "lmdb") {
        return Dataset::Format::LMDB;
    } else {
        return boost::optional<Format>();
    }
}
}
}
