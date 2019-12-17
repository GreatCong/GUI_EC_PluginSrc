#ifndef VIRTUALVIEWAPP_CALLBACK_H
#define VIRTUALVIEWAPP_CALLBACK_H

#include "EtherCAT_UserApp.h"
#include "MyGraphicsView.h"

class VirtualViewApp_Callback: public QObject,public Ethercat_Callback
{
   Q_OBJECT
public:
    VirtualViewApp_Callback(QObject *parent = nullptr);
    ~VirtualViewApp_Callback();
public:
    virtual void Master_AppLoop_callback();
    virtual void Master_AppStart_callback();
    virtual void Master_AppStop_callback();
    virtual void Master_AppScan_callback();

    virtual MyGraphicsView* get_graphicsViewPtr(){ return m_graphicsView; }
    virtual void set_graphicsViewPtr(MyGraphicsView *view){ m_graphicsView = view;}
private:
    MyGraphicsView *m_graphicsView;//保存了view中的所有回调
};

#endif // VIRTUALVIEWAPP_CALLBACK_H
