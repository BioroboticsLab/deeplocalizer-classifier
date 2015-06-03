#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <ui_manuelltagwindow.h>

#include "ManuellyTagger.h"
#include "WholeImageWidget.h"


namespace deeplocalizer {
namespace tagger {


class ManuellTagWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManuellTagWindow(std::deque<ImageDescription> && _image_desc);
    ~ManuellTagWindow();

    void showImage();
    void showTags(std::vector<Tag> & tags);
    Image & currentImage();

protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::ManuellTagWindow *ui;
    QGridLayout * _grid_layout;
    WholeImageWidget * _whole_image;
    unsigned long _image_idx;
    Image _current_image;
    ImageDescription & _current_desc;
    std::deque<ImageDescription> _images_with_proposals;
    std::deque<ImageDescription> _images_verified;

    void nextImage();
};
}
}
#endif // MANUELLTAGWINDOW_H
