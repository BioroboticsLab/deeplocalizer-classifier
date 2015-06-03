
#include "WholeImageWidget.h"

#include "utils.h"
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include "PipelineWorker.h"

namespace deeplocalizer {
namespace tagger {
using boost::optional;

WholeImageWidget::WholeImageWidget(QScrollArea *parent) : QWidget(parent) {
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    this->setMinimumSize(this->sizeHint());
    _parent = parent;
    init();
}

WholeImageWidget::WholeImageWidget(QScrollArea * parent, cv::Mat mat, std::vector<Tag> * tags) :
        QWidget(parent)
{

    _parent = parent;
    this->setTags(mat, tags);
    init();
}

void WholeImageWidget::init() {
    _thread = new QThread;
    _worker = new PipelineWorker;
    _worker->moveToThread(_thread);
    connect(_thread, &QThread::finished, _thread, &QThread::deleteLater);
    connect(_thread, &QThread::finished, _worker, &QThread::deleteLater);
    connect(_worker, &PipelineWorker::tagWithEllipseReady, this,
            &WholeImageWidget::tagProcessed, Qt::QueuedConnection);
    _thread->start();
}
void WholeImageWidget::createTag(int x, int y) {
    Tag && tag = Tag(cv::Rect(x - TAG_WIDTH / 2, y - TAG_HEIGHT / 2, TAG_WIDTH, TAG_HEIGHT),
                     optional<pipeline::Ellipse>());
    tag.setIsTag(true);
    qDebug() << "created new tag " << tag.getBoundingBox().x << ", " << tag.getBoundingBox().y;
    _newly_added_tags.emplace_back(tag);
    findEllipse(tag);
}

void WholeImageWidget::findEllipse(const Tag &tag) {
    QMetaObject::invokeMethod(_worker, "findEllipse", Qt::QueuedConnection,
                              Q_ARG(cv::Mat, tag.getSubimage(_mat)), Q_ARG(Tag, tag));
}
void WholeImageWidget::tagProcessed(Tag tag) {
    qDebug() << "tag processed" << tag.getBoundingBox().x;
    tag.setIsTag(true);
    _newly_added_tags.erase(
            std::remove_if(_newly_added_tags.begin(),
                           _newly_added_tags.end(),
                           [&tag](auto & t){
                               return t.getId() != tag.getId();
                           })
    );
    _tags->push_back(tag);
    repaint();
}

void WholeImageWidget::keyPressEvent(QKeyEvent * ) {
}

void WholeImageWidget::paintEvent(QPaintEvent *) {
    qDebug() << "paint: " << _newly_added_tags.size();
    _pixmap = cvMatToQPixmap(_mat);
    _painter.begin(this);

    _painter.scale(_scale, _scale);
    _painter.drawPixmap(0, 0, _pixmap);
    for(auto & t: *_tags) {
        t.draw(_painter);
    }
    for(auto & t: _newly_added_tags) {
        t.draw(_painter);
    }
    _painter.end();
}
void adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
void WholeImageWidget::wheelEvent(QWheelEvent * event) {
    //if(event->buttony() == Qt::LeftButton) {
    double factor;
    if (event->delta() > 0) {
        factor =1.25;
    } else {
        factor =0.8;
    }
    _scale *= factor;
    setMinimumSize(sizeHint());
    setFixedSize(sizeHint());
    adjustScrollBar(_parent->verticalScrollBar(), factor);
    adjustScrollBar(_parent->horizontalScrollBar(), factor);
    this->repaint();
}

void WholeImageWidget::mousePressEvent(QMouseEvent * event) {
    qDebug() << "mouse pressed";
    auto pos = event->pos() / _scale;
    auto maybe_tag = getTag(pos.x(), pos.y());
    if (maybe_tag) {
        _tags->erase(std::remove_if(_tags->begin(), _tags->end(),
                    [&maybe_tag](auto & t){
                        return t.getId() == maybe_tag.get().getId();
                    }));
    } else {
        createTag(pos.x(), pos.y());
    }
    repaint();
}

boost::optional<Tag &> WholeImageWidget::getTag(int x, int y) {
    for(auto & tag : *_tags) {
        if(tag.getBoundingBox().contains(cv::Point(x, y))) {
            return tag;
        }
    }
    return boost::optional<Tag &>();
}
void WholeImageWidget::setTags(cv::Mat mat, std::vector<Tag> * tags) {
    _mat = mat;
    _pixmap = cvMatToQPixmap(mat);
    _tags = tags;
    setFixedSize(sizeHint());
}

QSize WholeImageWidget::sizeHint() const {
    int width = static_cast<int>(_mat.cols*_scale);
    int height = static_cast<int>(_mat.rows*_scale);
    return QSize(width, height);
}
}
}
