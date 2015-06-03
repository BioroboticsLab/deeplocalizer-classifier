#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <ui_manuelltagwindow.h>

#include "ManuellyTagger.h"
#include "WholeImageWidget.h"
#include "TagWidget.h"


namespace deeplocalizer {
namespace tagger {


class ManuellTagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManuellTagWindow(std::deque<ImageDescription> && _image_desc);
    explicit ManuellTagWindow(ManuellyTagger * tagger);
    ~ManuellTagWindow();
public slots:
    void next();
    void back();
    void setImage(ImageDescription * desc, Image * img);
protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent * );

private:
    enum class State{Tags, Image};
    Ui::ManuellTagWindow *ui;
    ManuellyTagger * _tagger;
    QWidget * _tags_container;
    QGridLayout * _grid_layout;
    WholeImageWidget * _whole_image;
    std::vector<TagWidgetPtr> _tag_widgets;
    State _state = State::Tags;
    State _next_state = State::Tags;
    ImageDescription * _desc;
    Image * _image;

    void init();
    void showImage();
    void showTags();
    void arangeTagWidgets();
    void setupConnections();
    void eraseNegativeTags();
};
}
}
#endif // MANUELLTAGWINDOW_H
