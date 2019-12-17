#ifndef ETHERCAT_USERAPP_H
#define ETHERCAT_USERAPP_H

#include <QWidget>
#include <QtPlugin>
#include <QObject>
#include <QList>
#include <QIcon>
#include <QApplication>
#include <QStyle>

#include "EtherCAT_Message.h"

typedef struct{
    QString slave_name;
    int inputs_offset;//如果返回-1,则表示没有input变量
    int outputs_offset;//如果返回-1,则表示没有output变量
    int inputs_Bits; //对于char型指针，要除上2
    int outputs_Bits;//对于char型指针，要除上2
}Master_Address_t;

class Ethercat_Callback
{
public:
    Ethercat_Callback()
    {
      m_slaveCount = 0; m_Master_addressBase = nullptr; m_isAppScan_allow_read = false;
      m_appScan_readTimeout = 1000; m_slaveChoose_index = 0; m_isSlaveChoose_AppAllow = true;
      m_VirtualSlaveCount = 0; m_VirtualSlaveChoose_index = 0;
    }
    virtual ~Ethercat_Callback(){}
public:
    virtual void Master_AppLoop_callback() = 0;
    virtual void Master_AppStart_callback() = 0;
    virtual void Master_AppStop_callback() = 0;
    virtual void Master_AppScan_callback(){}

    virtual void Master_ReleaseAddress(){ m_Master_addressBase = nullptr; }
    virtual int Master_setAdressBase(char* address){ m_Master_addressBase = address; return 0; }

    virtual int Master_getSlaveCount() {return m_slaveCount;}
    virtual void Master_setSlaveCount(int num) {m_slaveCount = num;}

    virtual int Master_getSlaveChooseIndex() {return m_slaveChoose_index;}
    virtual void Master_setSlaveChooseIndex(int num) {m_slaveChoose_index = num;}

    virtual bool Master_getSlaveChoose_AppAllow() {return m_isSlaveChoose_AppAllow;}
    virtual void Master_setSlaveChoose_AppAllow(bool isAllow) {m_isSlaveChoose_AppAllow = isAllow;}

    virtual bool Master_getAppScan_allowRead(){return m_isAppScan_allow_read;}
    virtual void Master_setAppScan_allowRead(bool isAllowRead){m_isAppScan_allow_read = isAllowRead;}

    virtual int Master_getAppScan_ReadTimeout(){return m_appScan_readTimeout;}
    virtual void  Master_setAppScan_ReadTimeout(int timeout){m_appScan_readTimeout = timeout;}

    virtual int Master_getVirtualSlaveCount() {return m_VirtualSlaveCount;}
    virtual void Master_setVirtualSlaveCount(int num) {m_VirtualSlaveCount = num;}

    virtual int Master_getVirtualSlaveChooseIndex() {return m_VirtualSlaveChoose_index;}
    virtual void Master_setVirtualSlaveChooseIndex(int num) {m_VirtualSlaveChoose_index = num;}

    //未测试函数
    virtual const QList<Master_Address_t> Master_getAddressList() {return m_Master_addressList;}
    virtual void Master_setAddressList(const QList<Master_Address_t> addrList) {m_Master_addressList = addrList;}
    //未测试函数结束
protected:
    int m_slaveCount;
    char *m_Master_addressBase;
    QList<Master_Address_t> m_Master_addressList;
    bool m_isAppScan_allow_read;
    int m_appScan_readTimeout;
    int m_VirtualSlaveCount;
    int m_VirtualSlaveChoose_index;

    int m_slaveChoose_index;
    bool m_isSlaveChoose_AppAllow;
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
    EtherCAT_UserApp()
    { _UIWidget = nullptr;_EC_callback = nullptr; _EC_message = nullptr;
      m_appIcon = QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton);
      m_appName = "UserApp"; _PropertyWidget = nullptr;m_isUsePropertyWidget = false;
    }
    virtual ~EtherCAT_UserApp(){}
public:
    virtual QWidget *get_UIWidgetPtr() {return _UIWidget;}
    virtual Ethercat_Callback *get_CallbackPtr() {return _EC_callback;}
    virtual bool Init_Cores() = 0;
    virtual bool Destroy_Cores() = 0;
    virtual bool Init_Object() = 0;
    virtual bool Init_Slots() { return true;}
    virtual EtherCAT_Message *get_MessageObj(){return _EC_message;}
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr) = 0;

    virtual QString get_appPath(){ return m_appPath;}
    virtual void set_appPath(const QString appPath){ m_appPath = appPath; }
    virtual QIcon get_AppIcon(){ return m_appIcon; }
    virtual void set_AppIcon(const QIcon icon) { m_appIcon = icon; }
    virtual QString get_AppName(){ return m_appName;}
    virtual void set_AppName(const QString appame){ m_appName = appame; }
    virtual void set_UIWidgetPtr(QWidget *widget) {_UIWidget = widget;}
    virtual void set_CallbackPtr(Ethercat_Callback *callback) { _EC_callback = callback;}
    virtual void set_MessageObj(EtherCAT_Message *message) { _EC_message = message;}

    virtual QWidget* get_PropertyWidgetPtr(){ return _PropertyWidget;}
    virtual void set_PropertyWidgetPtr(QWidget *widget){ _PropertyWidget = widget; }
    virtual bool get_isUsePropertyWidget(){ return m_isUsePropertyWidget;}
    virtual void set_isUsePropertyWidget(bool isUse){ m_isUsePropertyWidget = isUse; }
protected:
    virtual int Load_setting(const QString &path) { Q_UNUSED(path); return 0; }
    virtual int Save_setting(const QString &path) { Q_UNUSED(path); return 0; }

    QWidget *_UIWidget;
    Ethercat_Callback *_EC_callback;
    EtherCAT_Message *_EC_message;

    QWidget *_PropertyWidget;
    bool m_isUsePropertyWidget;

    QString m_appPath;
    QIcon m_appIcon;
    QString m_appName;
};

#define UserApp_iid "DRE.My_Interface_UserAppWidget"//定义ID

Q_DECLARE_INTERFACE(EtherCAT_UserApp, UserApp_iid) //定义接口

#endif // ETHERCAT_USERAPP_H
