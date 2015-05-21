
#include "manuelltagwindow.h"
#include <QApplication>

using namespace deeplocalizer::tagger;
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ManuellTagWindow window;
    window.show();
    return app.exec();
}
