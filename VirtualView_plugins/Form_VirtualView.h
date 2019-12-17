#ifndef FORM_VIRTUALVIEW_H
#define FORM_VIRTUALVIEW_H

#include <QWidget>

#include "MyGraphicsProxyWidget.h"
#include "MyGraphicsView.h"
#include "MyGraphicsScene.h"

namespace Ui {
class Form_VirtualView;
}

class Form_VirtualView : public QWidget
{
    Q_OBJECT

public:
    explicit Form_VirtualView(QWidget *parent = 0);
    ~Form_VirtualView();

    bool Init_cores();
    bool Destroy_cores();
    MyGraphicsView *get_GraphicsViewPtr();
    MyGraphicsScene *get_GraphicsScenePtr();
private slots:
    void on_pushButton_clearView_clicked();

    void on_pushButton_OpenProject_clicked();

    void on_pushButton_SaveProject_clicked();

private:
    MyGraphicsScene *m_pScene;
    MyGraphicsView *m_pView;
private:
    Ui::Form_VirtualView *ui;
};

#endif // FORM_VIRTUALVIEW_H
