
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <QApplication>
#include <QMainWindow>
#include "ImageTagWidget.h"

using namespace std;

TEST_CASE( "ImageTagWidget", "[gui]" ) {
    SECTION( "it can be displayed" ) {
        char argv1[] = "program name";
        char * argv[] = {argv1, NULL};
        int argc = sizeof(argv) / sizeof(char*) - 1;
        QApplication app(argc, argv);
        ImageTagWidget * image_tag_widget = new ImageTagWidget;
        QMainWindow  * mainWindow = new QMainWindow;
        mainWindow->setCentralWidget(image_tag_widget);
        mainWindow->show();
        app.exec();
    }
}

