#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
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
    void scrollLeft();
    void scrollRight();
    void scrollTop();
    void scrollBottom();
    void scroll();
    void scrollBack();
    void changed();
    void save();
    void setImage(ImageDescPtr desc, ImagePtr img);
protected:
    void resizeEvent(QResizeEvent * );
private slots:
    void updateStatusBar();
    void setProgress(double progress);
private:
    enum class State{Tags, Image};
    Ui::ManuallyTaggerWindow *ui;

    QWidget * _tags_container;
    QGridLayout * _grid_layout;
    WholeImageWidget * _whole_image;
    std::vector<TagWidgetPtr> _tag_widgets;
    QProgressBar * _progres_bar;

    std::unique_ptr<ManuallyTagger> _tagger;
    State _state = State::Tags;
    State _next_state = State::Tags;
    ImageDescPtr  _desc;
    ImagePtr  _image;
    QTimer * _save_timer;
    bool _changed = false;


    void init();
    void showImage();
    void showTags();
    void arangeTagWidgets();
    void setupConnections();
    void setupActions();
    void setupUi();
    void eraseNegativeTags();
};
}
}
#endif // MANUELLTAGWINDOW_H
