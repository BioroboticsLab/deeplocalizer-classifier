#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <ui_manuelltagwindow.h>

#include "ManuellyTagger.h"


namespace deeplocalizer {
namespace tagger {


class ManuellTagWindow;

class ManuellTagWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit ManuellTagWindow(QWidget *parent = 0);
    ~ManuellTagWindow();
    void loadImage(Image & img);
    void isATag();
    void isNotaTag();
    void showImage();
    void showTags(std::vector<Tag> & tags);
    void showNextTag();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::ManuellTagWindow *ui;
    QGridLayout * _grid_layout;
    std::vector<QString> paths;
    std::unique_ptr<ManuellyTagger> tagger;
    Tag * tag_center;
    std::shared_ptr<Image> _current_image;
};
}
}
#endif // MANUELLTAGWINDOW_H
