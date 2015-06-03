#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include "manuelltagwindow.h"
#include "TagWidget.h"
#include "utils.h"

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
    _tags_container = new QWidget(ui->scrollArea);
    setupConnections();
    showTags();
}


ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::keyPressEvent(QKeyEvent *) {
}


void ManuellTagWindow::showImage() {
    _state = State::Image;
    _whole_image->setTags(_current_image.getCvMat(), &_current_desc.getTags());

    auto w = ui->scrollArea->takeWidget();
    ASSERT(w == ui->tags_container, "expected _tags_containter");
    ui->scrollArea->setWidget(_whole_image);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->show();
}

void ManuellTagWindow::showTags() {
    _state = State::Tags;
    auto & tags = _current_desc.getTags();
    qDebug() << ui->scrollArea->widget()->geometry();
    std::sort(tags.begin(), tags.end(), [](auto & t1, auto & t2) {
        return t1.getEllipse() < t2.getEllipse();
    });
    _tag_widgets.clear();
    for (auto & tag : tags) {
        TagWidgetPtr widget = std::make_shared<TagWidget>();
        widget->connect(widget.get(), &TagWidget::clicked,
                        widget.get(), &TagWidget::toggleTag);
        auto mat = tag.getSubimage(_current_image.getCvMat(), widget->border());
        widget->setTag(&tag, mat);
        _tag_widgets.push_back(widget);
    }
    arangeTagWidgets();
    if (ui->tags_container->layout() != _grid_layout) {
        delete ui->tags_container->layout();
        ui->tags_container->setLayout(_grid_layout);
    }
    // do not delete current widget in scrollArea
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(ui->tags_container);
    _whole_image->hide();
}

void ManuellTagWindow::arangeTagWidgets() {
    int col = 0;
    int row = 0;
    int width = ui->scrollArea->geometry().width();
    int col_width = (_tag_widgets.front()->width() + _grid_layout->horizontalSpacing());
    int cols = width / col_width;

    for(auto & widget : _tag_widgets) {
        if (col == cols) { col = 0; row++; }
        _grid_layout->addWidget(widget.get(), row, col);
        col++;
    }
}

void ManuellTagWindow::loadImage(unsigned long idx) {
    if (idx >= _images_with_proposals.size()) {
        QMessageBox box;
        box.setWindowTitle("DONE!");
        box.setText("You are done! Feel very very happy! :-)");
        box.exec();
        return;
    }
    _image_idx = idx;
    _current_image = Image(_images_with_proposals.at(_image_idx));
    _current_desc = _images_with_proposals.at(_image_idx);
}

void ManuellTagWindow::resizeEvent(QResizeEvent * ) {
    if(_state == State::Tags) {
        arangeTagWidgets();
    }
}

void ManuellTagWindow::loadNextImage() {
    loadImage(_image_idx + 1);
}

void ManuellTagWindow::loadLastImage() {
    if (_image_idx == 0) { return; }
    loadImage(_image_idx - 1);
}

Image &ManuellTagWindow::currentImage() {
    return _current_image;
}
void ManuellTagWindow::setupConnections() {
    ui->push_next->connect(ui->push_next, &QPushButton::clicked, this, &ManuellTagWindow::next);
    ui->push_next->connect(ui->push_back, &QPushButton::clicked, this, &ManuellTagWindow::back);
}
void ManuellTagWindow::next() {
    if (_state == State::Image) {
        loadNextImage();
        showTags();
    } else if(_state == State::Tags) {
        showImage();
    }
}
void ManuellTagWindow::back() {
    if (_state == State::Image) {
        showTags();
    } else if(_state == State::Tags) {
        loadLastImage();
        showImage();
    }
}
}
}
