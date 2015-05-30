#include <QtWidgets/qapplication.h>
#include <QKeyEvent>
#include "manuelltagwindow.h"
#include "ImageTagWidget.h"

using namespace deeplocalizer::tagger;
using boost::optional;

ManuellTagWindow::ManuellTagWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManuellTagWindow)
{
    _grid_layout = new QGridLayout;
    this->tagger = std::unique_ptr<ManuellyTagger>(
        new ManuellyTagger(
            std::vector<ImageDescription>{
                ImageDescription{"test/testdata/Cam_0_20140804152006_3.jpeg"}
            }
        )
    );
    ui->setupUi(this);

    this->ui->imagetag_center->setStyleSheet(
            "QLabel {border: 10px solid yellow}\n"
            "QLabel[is_tag=\"yes\"] {border: 10px solid green}\n"
            "QLabel[is_tag=\"no\"] {border: 10px solid red}"
    );
    _current_image_descr = this->tagger->nextImageDescr();
    _current_image = Image(_current_image_descr);
}


ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::keyPressEvent(QKeyEvent *) {
}


void ManuellTagWindow::showImage() {
}

void ManuellTagWindow::showTags(std::vector<Tag> & tags /*={}*/) {
    int col = 0;
    int row = 0;
    int cols = this->geometry().width() / 100;
    if (tags.empty()) {
        tags = _current_image_descr.getTags();
    }
    delete ui->latestScrollArea->layout();
    std::sort(tags.begin(), tags.end(), [](auto & t1, auto & t2) {
        return t1.getEllipse() < t2.getEllipse();
    });

    for (auto tag : tags) {
        ImageTagWidget * widget = new ImageTagWidget;
        widget->connect(widget, &ImageTagWidget::clicked,
                        widget, &ImageTagWidget::toggleTag);
        widget->setTag(&tag, _current_image.getCvMat());
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

