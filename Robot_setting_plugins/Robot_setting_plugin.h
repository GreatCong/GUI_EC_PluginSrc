#ifndef USERAPP_PLUGIN_H
#define USERAPP_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "Form_Robot_Setting.h"
#include "Serial_Handle.h"

#include "Excel_Object.h"

#include <QSerialPort>

class Robot_setting_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "Robot_setting_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    Form_Robot_Setting *m_userWidget;
    virtual ~Robot_setting_plugin();
public:
    Robot_setting_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual bool Init_Slots();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private:
    QSerialPort *m_serial;
    Serial_Handle m_serial_handle;

    Excel_Object m_excel_obj;
    EtherCAT_Message *m_EtherCAT_msg;
private slots:
    void Serial_open_handle(bool isClick);
    void Serial_search_handle(bool isClick);
    void Serial_getMSG_handle(bool isClick);
    void Serial_setMSG_handle(bool isClick);
};

#endif // USERAPP_PLUGIN_H
