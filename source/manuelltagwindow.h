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
    ~ManuellTagWindow();
    Image & currentImage();
public slots:
    void next();
    void back();
    void loadNextImage();
    void loadLastImage();
    void loadImage(unsigned long idx);

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent * );

private:
    enum class State{Tags, Image};
    Ui::ManuellTagWindow *ui;
    QWidget * _tags_container;
    QGridLayout * _grid_layout;
    WholeImageWidget * _whole_image;
    std::vector<TagWidgetPtr> _tag_widgets;
    unsigned long _image_idx = 0;
    Image _current_image;
    ImageDescription & _current_desc;
    State _state;
    std::deque<ImageDescription> _images_with_proposals;
    std::deque<ImageDescription> _images_verified;

    void showImage();
    void showTags();
    void arangeTagWidgets();
    void setupConnections();
};
}
}
#endif // MANUELLTAGWINDOW_H
