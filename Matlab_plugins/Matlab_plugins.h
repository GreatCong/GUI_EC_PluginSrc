#ifndef MATLAB_PLUGIN_H
#define MATLAB_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "Form_MatlabApp.h"

#include "Callback_Matlab.h"

class Matlab_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "Matlab_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    Form_MatlabApp *m_userWidget;
    Callback_Matlab *m_callback_Matlab;
    EtherCAT_Message *m_master_Message;

    virtual  ~Matlab_plugin();
public:
    Matlab_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
};

#endif // MATLAB_PLUGIN_H
