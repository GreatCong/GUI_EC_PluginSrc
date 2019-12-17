#ifndef USERAPP_PLUGIN_H
#define USERAPP_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "form_plot.h"

#include "MyMessageObj.h"

class UserApp_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "User_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    Form_plot *m_userWidget;
    virtual  ~UserApp_plugin();
    MyMessageObj *m_msg;
public:
    UserApp_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private slots:
    void sig_msgTest(int data);
};

#endif // USERAPP_PLUGIN_H
