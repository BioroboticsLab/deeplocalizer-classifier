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
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::ManuellTagWindow *ui;
    std::vector< QString > paths;
    ManuellyLocalizer localizer;
    boost::optional< Tag > tag_left;
    boost::optional< Tag > tag_center;
    boost::optional< Tag > tag_right;
    void setTag(ImageTagWidget & tag_widget, const Tag & tag);
    void nextTag();
};
}
}
#endif // MANUELLTAGWINDOW_H
