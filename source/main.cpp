
#include "ManuellyLocalizer.h"

using namespace std;
using namespace deep_localizer::manually;

int main(int argc, char* argv[])
{
    string image_path = argv[1];
    vector<string> image_paths;
    image_paths.push_back(image_path);
    ManuellyLocalizer localizer(image_paths);
    localizer.next_tag();
    return 0;
}
