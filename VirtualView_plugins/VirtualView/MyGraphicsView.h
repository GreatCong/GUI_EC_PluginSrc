#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H


#include <QObject>
#include <QGraphicsView>
#include "MyGraphicsProxyWidget.h"

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    MyGraphicsView(QWidget *parent=0);
    ~MyGraphicsView();

    virtual void set_appPath(const QString appPath){ m_appPath = appPath; }
    virtual QString get_appPath(){ return m_appPath; }
    virtual EtherCAT_Message *get_MessageObj(){return m_master_Message;}
    virtual void set_MessageObj(EtherCAT_Message *message) { m_master_Message = message;}

    virtual void Master_AppLoop_callback();
    virtual void Master_AppStart_callback();
    virtual void Master_AppStop_callback();
    virtual void Master_AppScan_callback();

    virtual void Master_ReleaseAddress(){ m_Master_addressBase = nullptr; }
    virtual int Master_setAdressBase(char* address){ m_Master_addressBase = address; return 0; }

    virtual int Master_getSlaveCount() {return m_slaveCount;}
    virtual void Master_setSlaveCount(int num) {m_slaveCount = num;}

    virtual const QList<Master_Address_t> Master_getAddressList() {return m_Master_addressList;}
    virtual void Master_setAddressList(const QList<Master_Address_t> addrList) {m_Master_addressList = addrList;}

    int get_appCount() { return m_appCount; }
    void set_appCount(int count) { m_appCount = count;}
private:
    QString m_appPath;
    EtherCAT_Message *m_master_Message;
//    MyGraphicsProxyWidget *m_itemProxyWidget;

    int m_slaveCount;
    char *m_Master_addressBase;
    QList<Master_Address_t> m_Master_addressList;
    int m_appCount;

    bool Plugins_Load(const QString &fileName, MyGraphicsProxyWidget *item_widget);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void Control_StatusMessage_change(QString message,int interval);
    void Control_BottomMessage_change(QString message);
    void Control_MasterStop_Signal();

};

#endif // MYGRAPHICSVIEW_H
