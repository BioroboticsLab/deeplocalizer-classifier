#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <ui_ManuallyTaggerWindow.h>

#include "ManuallyTagger.h"
#include "WholeImageWidget.h"
#include "TagWidget.h"


namespace deeplocalizer {
namespace tagger {


class ManuallyTagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManuallyTagWindow(std::vector<ImageDescPtr> && _image_desc);
    explicit ManuallyTagWindow(std::unique_ptr<ManuallyTagger> tagger);
    ~ManuallyTagWindow();
public slots:
    void next();
    void back();
    void scroll();
    void scrollBack();
    void setImage(ImageDescPtr desc, ImagePtr img);
protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent * );

private:
    enum class State{Tags, Image};
    Ui::ManuallyTaggerWindow *ui;
    std::unique_ptr<ManuallyTagger> _tagger;
    QWidget * _tags_container;
    QGridLayout * _grid_layout;
    WholeImageWidget * _whole_image;
    std::vector<TagWidgetPtr> _tag_widgets;
    State _state = State::Tags;
    State _next_state = State::Tags;
    ImageDescPtr  _desc;
    ImagePtr  _image;
    QAction * _nextAct;
    QAction * _backAct;
    QAction * _scrollAct;
    QAction * _scrollBackAct;
    std::deque<std::shared_ptr<QPushButton>> _image_names;

    void init();
    void showImage();
    void showTags();
    void arangeTagWidgets();
    void setupConnections();
    void setupActions();
    void eraseNegativeTags();
};
}
}
#endif // MANUELLTAGWINDOW_H
