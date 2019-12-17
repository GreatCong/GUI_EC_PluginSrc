#ifndef ETHERCAT_USERAPP_H
#define ETHERCAT_USERAPP_H

#include <Qwidget>
#include <QtPlugin>
#include <QObject>
#include <QList>

#include "Commmon/EtherCAT_Message.h"

typedef struct{
    int inputs_offset;//如果返回-1,则表示没有input变量
    int outputs_offset;//如果返回-1,则表示没有output变量
}Master_Address_t;

class Ethercat_Callback
{
public:
    Ethercat_Callback() { m_slaveCount = 0; m_Master_addressBase = nullptr; }
public:
    virtual void Master_AppLoop_callback() = 0;
    virtual void Master_AppStart_callback() = 0;
    virtual void Master_AppStop_callback() = 0;

    virtual void Master_ReleaseAddress(){ m_Master_addressBase = nullptr; }
    virtual int Master_setAdressBase(char* address){ m_Master_addressBase = address; return 0; }

    virtual int Master_getSlaveCount() {return m_slaveCount;}
    virtual void Master_setSlaveCount(int num) {m_slaveCount = num;}

    //未测试函数
    virtual const QList<Master_Address_t> Master_getAddressList() {return m_Master_addressList;}
    virtual void Master_setAddressList(const QList<Master_Address_t> addrList) {m_Master_addressList = addrList;}
    //未测试函数结束
protected:
    int m_slaveCount;
    char *m_Master_addressBase;
    QList<Master_Address_t> m_Master_addressList;
};

//class EtherCAT_Message : public QObject
//{
//    Q_OBJECT
//public:
////    explicit EtherCAT_Message(QObject *parent = nullptr);
//    void Set_StatusMessage(QString message, int interval)
//    {
//        emit StatusMessage_change(message,interval);//发出自定义信号
//    }

//    void Set_BottomMessage(QString message)
//    {
//        emit BottomMessage_change(message);//发出自定义信号
//    }

//    void Set_MasterStop()
//    {
//        emit MasterStop_Signal();//发出自定义信号
//    }
//signals:
//    void StatusMessage_change(QString message,int interval);//状态栏信息
//    void BottomMessage_change(QString message);//bottom Text message
//    void MasterStop_Signal();//stop master
//};


//此对象主要包含3个部分 界面UI、操作回调callback和消息对象
class EtherCAT_UserApp
{
public:
    EtherCAT_UserApp() { _UIWidget = nullptr;_EC_callback = nullptr; _EC_message = nullptr;}
public:
    virtual QWidget *get_UIWidgetPtr() {return _UIWidget;}
    virtual Ethercat_Callback *get_CallbackPtr() {return _EC_callback;}
    virtual void Init_Cores() = 0;
    virtual void Destroy_Cores() = 0;
    virtual EtherCAT_Message *get_MessageObj(){return _EC_message;}
protected:
    virtual void set_UIWidgetPtr(QWidget *widget) {_UIWidget = widget;}
    virtual void set_CallbackPtr(Ethercat_Callback *callback) { _EC_callback = callback;}
    virtual void set_MessageObj(EtherCAT_Message *message) { _EC_message = message;}
    virtual int Load_setting(const QString &path) { Q_UNUSED(path); return 0; }
    virtual int Save_setting(const QString &path) { Q_UNUSED(path); return 0; }

    QWidget *_UIWidget;
    Ethercat_Callback *_EC_callback;
    EtherCAT_Message *_EC_message;
};

#define UserApp_iid "DRE.My_Interface_UserAppWidget"//定义ID

Q_DECLARE_INTERFACE(EtherCAT_UserApp, UserApp_iid) //定义接口

#endif // ETHERCAT_USERAPP_H
