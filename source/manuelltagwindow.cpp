#include <QtWidgets/qapplication.h>
#include <QKeyEvent>
#include "manuelltagwindow.h"

using namespace deeplocalizer::tagger;

ManuellTagWindow::ManuellTagWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManuellTagWindow)
{
    this->paths.push_back("test/testdata/Cam_0_20140804152006_3.jpeg");
    this->localizer = ManuellyLocalizer(paths);
    ui->setupUi(this);
    this->ui->imagetag_center->setStyleSheet(
            "QLabel {border: 10px solid yellow}\n"
            "QLabel[is_tag=\"yes\"] {border: 10px solid green}\n"
            "QLabel[is_tag=\"no\"] {border: 10px solid red}"
    );
    auto img = this->localizer.getCurrentImage();
    this->loadImage(img);
}


ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::loadImage(Image & img) {
    tag_center = img.nextTag();
    tag_right = img.nextTag();
    assert(tag_center.is_initialized());
    assert(tag_right.is_initialized());

    this->setTag(*ui->imagetag_center, *tag_center);
    this->setTag(*ui->imagetag_right, *tag_right);
}
void ManuellTagWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key::Key_D:
            isNotaTag();
            this->nextTag();
            break;
        case Qt::Key::Key_F:
            isATag();
            this->nextTag();
            break;
    }

}

void ManuellTagWindow::isNotaTag() {
    assert(tag_center.is_initialized());
    auto & tag = tag_center.get();
    tag.setIsTag(false);
}

void ManuellTagWindow::isATag() {
    assert(tag_center.is_initialized());
    auto & tag = tag_center.get();
    tag.setIsTag(true);
}

void ManuellTagWindow::nextTag() {
    Image & img = this->localizer.getCurrentImage();
    assert(tag_center.is_initialized());
    assert(tag_right.is_initialized());
    tag_left = tag_center;
    tag_center = tag_right;
    tag_right = img.nextTag();

    this->setTag(*ui->imagetag_left, *tag_left);
    this->setTag(*ui->imagetag_center, *tag_center);
    this->setTag(*ui->imagetag_right, *tag_right);
}
void ManuellTagWindow::setTag(ImageTagWidget & tag_widget, const Tag & tag) {
    tag_widget.setCvMat(tag.getSubimage());
    if (tag.isTag()) {
        tag_widget.setProperty("is_tag", "yes");
    } else {
        tag_widget.setProperty("is_tag", "no");
    }
    tag_widget.style()->unpolish(&tag_widget);
    tag_widget.style()->polish(&tag_widget);
    tag_widget.update();
}
