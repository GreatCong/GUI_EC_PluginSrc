#ifndef VIRTUALVIEW_PLUGIN_H
#define VIRTUALVIEW_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "Form_VirtualView.h"
#include "VirtualViewApp_Callback.h"

class VirtualView_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "VirtualView_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    Form_VirtualView *m_userWidget;
    VirtualViewApp_Callback *m_userCallback;
    virtual  ~VirtualView_plugin();

    EtherCAT_Message *m_master_Message;
public:
    VirtualView_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private slots:
    void Control_StatusMessage_change(QString message,int interval);
    void Control_BottomMessage_change(QString message);
    void Control_MasterStop_Signal();
};

#endif // VIRTUALVIEW_PLUGIN_H
