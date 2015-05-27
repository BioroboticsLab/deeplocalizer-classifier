#include <QtWidgets/qapplication.h>
#include <QKeyEvent>
#include "manuelltagwindow.h"

using namespace deeplocalizer::tagger;

ManuellTagWindow::ManuellTagWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManuellTagWindow)
{
    _grid_layout = new QGridLayout;
    this->paths.push_back("test/testdata/Cam_0_20140804152006_3.jpeg");
    // this->paths.push_back("test/testdata/with_5_tags.jpeg");
    this->localizer = ManuellyLocalizer(paths);
    ui->setupUi(this);
    this->ui->imagetag_center->setStyleSheet(
            "QLabel {border: 10px solid yellow}\n"
            "QLabel[is_tag=\"yes\"] {border: 10px solid green}\n"
            "QLabel[is_tag=\"no\"] {border: 10px solid red}"
    );
    auto img = this->localizer.nextImage();
    assert(img.is_initialized());
    this->loadImage(img.get());
}


ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::loadImage(Image & img) {
    tag_center = img.nextTag();
    assert(tag_center.is_initialized());
    ui->imagetag_center->setTag(tag_center.get());
}

void ManuellTagWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key::Key_D:
            isNotaTag();
            this->showNextTag();
            break;
        case Qt::Key::Key_F:
            isATag();
            this->showNextTag();
            break;
    }

}

void ManuellTagWindow::isNotaTag() {
    assert(tag_center.is_initialized());
    auto & tag = tag_center.get();
    tag->setIsTag(false);
}

void ManuellTagWindow::isATag() {
    assert(tag_center.is_initialized());
    auto & tag = tag_center.get();
    tag->setIsTag(true);
}

void ManuellTagWindow::showNextTag() {
    Image & img = this->localizer.getCurrentImage();
    for(tag_center = img.nextTag(); !tag_center;) {
        auto opt_img = localizer.nextImage();
        if (opt_img) {
            tag_center = opt_img.get().nextTag();
        } else {
            // TODO: save dataset
            QApplication::quit();
            return;
        }
    }
    ui->imagetag_center->setTag(tag_center.get());
}
void ManuellTagWindow::showImage() {

}
void ManuellTagWindow::showLatest(std::vector<std::shared_ptr<Tag>> tags /*={}*/) {
    int col = 0;
    int row = 0;
    int cols = this->geometry().width() / 100;
    if (tags.empty()) {
        tags = localizer.getCurrentImage().getTags();
    }
    delete ui->latestScrollArea->layout();
    for (auto tag : tags) {
        ImageTagWidget * widget = new ImageTagWidget;
        // widget->connect(widget, &ImageTagWidget::clicked, tag);
        widget->setTag(tag);
        if (col == cols) {
            col = 0;
            row++;
        }
        _grid_layout->addWidget(widget, row, col);
        col++;
    }
    delete ui->latestTags->layout();
    ui->latestTags->setLayout(_grid_layout);
    ui->imagetag_center->hide();
}

