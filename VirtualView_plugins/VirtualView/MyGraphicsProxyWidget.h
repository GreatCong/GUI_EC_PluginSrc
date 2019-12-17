#ifndef MYGRAPHICSPROXYWIDGET_H
#define MYGRAPHICSPROXYWIDGET_H

#include <QGraphicsProxyWidget>

#include "EtherCAT_UserApp.h"
#include "Form_SlaveChoose.h"
#include <QDebug>

class MyGraphicsProxyWidget : public QGraphicsProxyWidget
{
    Q_OBJECT
public:
//    MyGraphicsProxyWidget (QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    MyGraphicsProxyWidget (QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
    void SetGraphicsMode(GraphicsItemFlags);
    ~MyGraphicsProxyWidget();
public:
    void set_userAppPtr(EtherCAT_UserApp *userApp){ m_userApp = userApp; }
    EtherCAT_UserApp *get_userAppPtr(void){ return m_userApp; }

    void set_formSlaveChoosePtr(Form_SlaveChoose *userForm){ m_form_SlaveChoose = userForm; }
    Form_SlaveChoose *get_formSlaveChoosePtr(void){ return m_form_SlaveChoose; }

    void Init_form_SlaveChoose_data();

    bool Load_UserStyle(const QString &name);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);

    int type() const { return UserType + 1; }
private:
    void StartGraphicsMode();
    void StopGraphicsMode();
    bool IsGraphicsMode() const;

    EtherCAT_UserApp *m_userApp;
    Form_SlaveChoose *m_form_SlaveChoose;
    bool m_isInitSlaveChoose_data;
private:
    GraphicsItemFlags m_eWidgetFlag;
    GraphicsItemFlags m_eGraphicsFlag;

    QPointF m_centerPointF;
    bool m_bResizing;
    int m_Zoom_curPos;
    QPointF m_Zoom_pLast;

    int m_SlaveChoose_indexLast;

    typedef enum{
       Zoom_Pos_NULL = 0,
       Zoom_Pos_LeftTop = 11,
       Zoom_Pos_Top = 12,
       Zoom_Pos_RightTop = 13,
       Zoom_Pos_Left = 21,
       Zoom_Pos_Center = 22,
       Zoom_Pos_Right = 23,
       Zoom_Pos_LeftBottom = 31,
       Zoom_Pos_Bottom = 32,
       Zoom_Pos_RightBottom = 33,
    }Zoom_Pos_enum;

    int countRow(QPointF p);
    void setCursorType(int flag);
    int countFlag(QPointF p, int row);

private slots:
    void SlaveChoose_slaveIndex_changed(int index);
};

#endif // MYGRAPHICSPROXYWIDGET_H
