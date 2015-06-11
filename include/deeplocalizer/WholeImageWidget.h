#ifndef DEEP_LOCALIZER_WHOLEIMAGEWIDGET_H
#define DEEP_LOCALIZER_WHOLEIMAGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QThread>
#include <QPainter>

#include <opencv2/core/core.hpp>
#include <boost/optional/optional.hpp>
#include <QtGui/qpainter.h>
#include "Image.h"


namespace deeplocalizer {
namespace tagger {
class Tag;

class ImageDesc;

class PipelineWorker;

class WholeImageWidget : public QWidget {
Q_OBJECT

public:
    WholeImageWidget(QScrollArea * parent);
    WholeImageWidget(QScrollArea * parent, cv::Mat mat, std::vector<Tag> * tags);
    void setTags(cv::Mat mat, std::vector<Tag> * tags);
    virtual QSize sizeHint() const;
public slots:
    Tag createTag(int x, int y);
    void tagProcessed(Tag tag);
signals:
    void imageFinished();
protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    virtual void paintEvent(QPaintEvent *);
private:
    QScrollArea *_parent;
    cv::Mat _mat;
    QPixmap _pixmap;
    QPainter _painter;
    double _scale = 0.8;
    std::vector<Tag> * _tags;
    std::list<Tag> _newly_added_tags;
    std::set<unsigned long> _deleted_Ids;
    PipelineWorker *_worker;
    QThread *_thread;

    void init();
    boost::optional<Tag &> getTag(int x, int y);
    void findEllipse(Tag &&tag);

    template<typename T>
    void eraseTag(const unsigned long id, T& tags) {
        std::cout << "eraseTag: " << id << ", " << QThread::currentThreadId() << std::endl;
        _deleted_Ids.insert(id);
        tags.erase(std::remove_if(tags.begin(), tags.end(),
                                  [id](auto & t){
                                      return t.id() == id;
                                  }),
                   tags.end()
        );
    }

    virtual ~WholeImageWidget() {
        _thread->quit();
        _thread->wait();
        delete _thread;
    }
};
}
}

#endif //DEEP_LOCALIZER_WHOLEIMAGEWIDGET_H
