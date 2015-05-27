#ifndef MANUELLTAGWINDOW_H
#define MANUELLTAGWINDOW_H

#include <QMainWindow>
#include <ui_manuelltagwindow.h>

#include "ManuellyLocalizer.h"


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
    void showLatest(std::vector<std::shared_ptr<Tag>> tags={});
    void showNextTag();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::ManuellTagWindow *ui;
    QGridLayout * _grid_layout;
    std::vector< QString > paths;
    ManuellyLocalizer localizer;
    boost::optional<std::shared_ptr<Tag>> tag_center;
};
}
}
#endif // MANUELLTAGWINDOW_H
