#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include "manuelltagwindow.h"
#include "TagWidget.h"
#include "utils.h"

using boost::optional;
namespace deeplocalizer {
namespace tagger {

ManuellTagWindow::ManuellTagWindow(ManuellyTagger * tagger) :
    QMainWindow(nullptr),
    _tagger(tagger)
{
    init();
}
ManuellTagWindow::ManuellTagWindow(std::deque<ImageDescription> && descriptions) :
    QMainWindow(nullptr),
    _tagger(new ManuellyTagger(std::move(descriptions)))
{
    init();
}

void ManuellTagWindow::init() {
    ui = new Ui::ManuellTagWindow;
    _grid_layout = new QGridLayout;
    ui->setupUi(this);
    _whole_image = new WholeImageWidget(ui->scrollArea);
    _tags_container = new QWidget(ui->scrollArea);
    _backAct = new QAction(this);
    _nextAct = new QAction(this);

    setupConnections();
    setupShortcuts();
    _state = State::Tags;
    _tagger->loadImage(0);
}
ManuellTagWindow::~ManuellTagWindow()
{
    delete ui;
}

void ManuellTagWindow::keyPressEvent(QKeyEvent *) {
}


void ManuellTagWindow::showImage() {
    _state = State::Image;
    _whole_image->setTags(_image->getCvMat(), &_desc->getTags());
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(_whole_image);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->show();
}

void ManuellTagWindow::showTags() {
    _state = State::Tags;
    auto & tags = _desc->getTags();
    std::sort(tags.begin(), tags.end(), [](auto & t1, auto & t2) {
        return t1.getEllipse() < t2.getEllipse();
    });
    _tag_widgets.clear();
    for (auto & tag : tags) {
        TagWidgetPtr widget = std::make_shared<TagWidget>();
        widget->connect(widget.get(), &TagWidget::clicked,
                        widget.get(), &TagWidget::toggleTag);
        auto mat = tag.getSubimage(_image->getCvMat(), widget->border());
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

void ManuellTagWindow::resizeEvent(QResizeEvent * ) {
    if(_state == State::Tags) {
        arangeTagWidgets();
    }
}


void ManuellTagWindow::setupShortcuts() {
    _backAct->setShortcut(QKeySequence(Qt::Key_Shift + Qt::Key_Enter));
    this->addAction(_backAct);
    _nextAct->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Space)});
    this->addAction(_nextAct);
}
void ManuellTagWindow::setupConnections() {
    this->connect(ui->push_next, &QPushButton::clicked, _nextAct, &QAction::trigger);
    this->connect(ui->push_back, &QPushButton::clicked, _backAct, &QAction::trigger);
    this->connect(_nextAct, &QAction::triggered, this, &ManuellTagWindow::next);
    this->connect(_backAct, &QAction::triggered, this, &ManuellTagWindow::back);
    this->connect(_tagger, &ManuellyTagger::outOfRange, [](int) {
        QMessageBox box;
        box.setWindowTitle("DONE!");
        box.setText("You are done! Feel very very happy! :-)");
        box.exec();
    });
}
void ManuellTagWindow::next() {
    if (_state == State::Image) {
        _next_state = State::Tags;
        _tagger->loadNextImage();
    } else if(_state == State::Tags) {
        ManuellTagWindow::eraseNegativeTags();
        showImage();
    }
}
void ManuellTagWindow::back() {
    if (_state == State::Image) {
        showTags();
    } else if(_state == State::Tags) {
        _next_state = State::Image;
        _tagger->loadLastImage();
    }
}
void ManuellTagWindow::setImage(ImageDescription *desc, Image *img) {
    _desc = desc;
    _image = img;
    if (_next_state == State::Image) {
        showImage();
    } else {
        showTags();
    }

}

void ManuellTagWindow::eraseNegativeTags() {
    auto & tags = _desc->getTags();
    tags.erase(std::remove_if(tags.begin(), tags.end(), [](const auto & tag) {
        return not tag.isTag();
    }), tags.end());
}

}
}
