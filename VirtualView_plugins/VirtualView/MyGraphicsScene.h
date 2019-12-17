#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include "objectcontroller/objectcontroller.h"

//自定义scene
class MyGraphicsScene :  public QGraphicsScene
{
public:
    MyGraphicsScene(QObject *parent = nullptr);
    virtual ~MyGraphicsScene();

    void set_status_AppRun(bool isRun){ m_isAppRun = isRun; }
    bool get_status_AppRun(){ return m_isAppRun; }

    ObjectController *get_WidgetObjectController(){ return m_widgetObjectController; }
private:
    bool m_isAppRun;
    ObjectController *m_widgetObjectController;
protected:
    // 左键：添加item  右键：移除item
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    // Backspace键移除item
    void keyPressEvent(QKeyEvent *event);

    //NOTE:这里有个坑
    /* QGraphicsScene要经过dragMoveEvent(QGraphicsSceneDragDropEvent *event)才能触发这个
    * dropEvent(QGraphicsSceneDragDropEvent *event)
    */
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
};

#endif // MYGRAPHICSSCENE_H
