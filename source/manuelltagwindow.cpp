#include <QtWidgets/qapplication.h>
#include <QKeyEvent>
#include "manuelltagwindow.h"
#include "TagWidget.h"

using boost::optional;
namespace deeplocalizer {
namespace tagger {

ManuellTagWindow::ManuellTagWindow(std::deque<ImageDescription> && _image_desc) :
    QMainWindow(nullptr),
    ui(new Ui::ManuellTagWindow),
    _grid_layout(new QGridLayout),
    _current_image(Image(_image_desc.front())),
    _current_desc(_image_desc.front()),
    _images_with_proposals(_image_desc)
{
    ui->setupUi(this);
    _whole_image = new WholeImageWidget(ui->scrollArea);
}


ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::keyPressEvent(QKeyEvent *) {
}


void ManuellTagWindow::showImage() {
    _whole_image->setTags(_current_image.getCvMat(), _current_desc.getTags());

    ui->scrollArea->setWidget(_whole_image);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);

    ui->scrollArea->show();
}

void ManuellTagWindow::showTags(std::vector<Tag> & tags /*={}*/) {
    int col = 0;
    int row = 0;
    int cols = this->geometry().width() / 100;
    if (tags.empty()) {
        tags = _images_with_proposals.at(0).getTags();
    }
    // delete ui->latestScrollArea->layout();
    std::sort(tags.begin(), tags.end(), [](auto & t1, auto & t2) {
        return t1.getEllipse() < t2.getEllipse();
    });

    for (auto tag : tags) {
        TagWidget * widget = new TagWidget;
        widget->connect(widget, &TagWidget::clicked,
                        widget, &TagWidget::toggleTag);
        widget->setTag(&tag, this->currentImage().getCvMat());
        if (col == cols) {
            col = 0;
            row++;
        }
        _grid_layout->addWidget(widget, row, col);
        col++;
    }
    delete ui->latestTags->layout();
    ui->latestTags->setLayout(_grid_layout);
    _whole_image->hide();
}

void ManuellTagWindow::nextImage() {
    ++_image_idx;
    _current_image = Image(_images_with_proposals.at(_image_idx));
    _current_desc = _images_with_proposals.at(_image_idx);
}
Image &ManuellTagWindow::currentImage() {
    return _current_image;
}
}
}
