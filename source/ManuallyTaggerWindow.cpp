#include <QApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include <QScrollBar>
#include "ManuallyTaggerWindow.h"
#include "utils.h"

using boost::optional;
namespace deeplocalizer {
namespace tagger {

ManuallyTagWindow::ManuallyTagWindow(ManuallyTagger * tagger) :
    QMainWindow(nullptr),
    _tagger(tagger)
{
    init();
}

ManuallyTagWindow::ManuallyTagWindow(std::deque<ImageDescription> && descriptions) :
    QMainWindow(nullptr),
    _tagger(new ManuallyTagger(std::move(descriptions)))
{
    init();
}

void ManuallyTagWindow::init() {
    ui = new Ui::ManuallyTaggerWindow;
    _grid_layout = new QGridLayout;
    ui->setupUi(this);
    _whole_image = new WholeImageWidget(ui->scrollArea);
    _tags_container = new QWidget(ui->scrollArea);
    setupActions();
    setupConnections();
    _state = State::Tags;
    _tagger->loadImage(0);
}

ManuallyTagWindow::~ManuallyTagWindow()
{
    delete ui;
}

void ManuallyTagWindow::keyPressEvent(QKeyEvent *) {
}


void ManuallyTagWindow::showImage() {
    _state = State::Image;
    _whole_image->setTags(_image->getCvMat(), &_desc->getTags());
    ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(_whole_image);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->show();
}

void ManuallyTagWindow::showTags() {
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

void ManuallyTagWindow::arangeTagWidgets() {
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

void ManuallyTagWindow::resizeEvent(QResizeEvent * ) {
    if(_state == State::Tags) {
        arangeTagWidgets();
    }
}

void ManuallyTagWindow::setupActions() {

    _backAct = new QAction(this);
    _nextAct = new QAction(this);
    _scrollAct = new QAction(this);
    _scrollBackAct = new QAction(this);

    _backAct->setShortcut(QKeySequence(Qt::Key_Shift + Qt::Key_Enter));
    this->addAction(_backAct);
    _nextAct->setShortcuts({QKeySequence(Qt::Key_Enter)});
    this->addAction(_nextAct);
     _scrollAct->setShortcut(QKeySequence(Qt::Key_Space));
    this->addAction(_scrollAct);
    _scrollBackAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Space));
    this->addAction(_scrollBackAct);

    this->connect(_nextAct, &QAction::triggered, this, &ManuallyTagWindow::next);
    this->connect(_backAct, &QAction::triggered, this, &ManuallyTagWindow::back);
    this->connect(_scrollAct, &QAction::triggered, this, &ManuallyTagWindow::scroll);
    this->connect(_scrollBackAct, &QAction::triggered, this, &ManuallyTagWindow::scrollBack);
}

void ManuallyTagWindow::setupConnections() {
    this->connect(ui->push_next, &QPushButton::clicked, _nextAct, &QAction::trigger);
    this->connect(ui->push_back, &QPushButton::clicked, _backAct, &QAction::trigger);
    this->connect(_tagger, &ManuallyTagger::loadedImage, this,
                  &ManuallyTagWindow::setImage);
    this->connect(_tagger, &ManuallyTagger::outOfRange, [](int) {
        QMessageBox box;
        box.setWindowTitle("DONE!");
        box.setText("You are done! Feel very very happy! :-)");
        box.exec();
    });
}
void ManuallyTagWindow::next() {
    if (_state == State::Image) {
        _next_state = State::Tags;
        _tagger->loadNextImage();
    } else if(_state == State::Tags) {
        ManuallyTagWindow::eraseNegativeTags();
        showImage();
    }
}
void ManuallyTagWindow::back() {
    if (_state == State::Image) {
        showTags();
    } else if(_state == State::Tags) {
        _next_state = State::Image;
        _tagger->loadLastImage();
    }
}
void ManuallyTagWindow::scroll() {
    QScrollBar * vert = ui->scrollArea->verticalScrollBar();
    QScrollBar * horz = ui->scrollArea->horizontalScrollBar();
    int next_horz = horz->value() + horz->pageStep() / 2;
    int next_vert = vert->value() + static_cast<int>(vert->pageStep() * 0.8);

    if(horz->value() == horz->maximum() && vert->value() == vert->maximum()) {
        next();
    } else if(horz->value() == horz->maximum()) {
        horz->setValue(0);
        if(next_vert > vert->maximum()) {
            vert->setValue(vert->maximum());
        } else {
            vert->setValue(next_vert);
        }
    } else if(next_horz > horz->maximum()) {
        horz->setValue(horz->maximum());
    } else {
        horz->setValue(next_horz);
    }
}

void ManuallyTagWindow::scrollBack() {
    QScrollBar * vert = ui->scrollArea->verticalScrollBar();
    QScrollBar * horz = ui->scrollArea->horizontalScrollBar();
    int last_horz = horz->value() - horz->pageStep() / 2;
    int last_vert = vert->value() - static_cast<int>(vert->pageStep() * 0.8);
    if(horz->value() == horz->minimum() && vert->value() == vert->minimum()) {
        back();
    } else if(horz->value() == horz->minimum()) {
        horz->setValue(horz->maximum());
        if(last_vert < vert->minimum()) {
            vert->setValue(vert->minimum());
        } else {
            vert->setValue(last_vert);
        }
    } else if(last_horz < horz->minimum()) {
        horz->setValue(horz->minimum());
    } else {
        horz->setValue(last_horz);
    }
}
void ManuallyTagWindow::setImage(ImageDescription *desc, Image *img) {
    _desc = desc;
    _image = img;
    if (_next_state == State::Image) {
        showImage();
    } else {
        showTags();
    }

}

void ManuallyTagWindow::eraseNegativeTags() {
    auto & tags = _desc->getTags();
    tags.erase(std::remove_if(tags.begin(), tags.end(), [](const auto & tag) {
        return not tag.isTag();
    }), tags.end());
}

}
}
