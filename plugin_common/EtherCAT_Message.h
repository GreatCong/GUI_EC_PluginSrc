#ifndef ETHERCAT_MESSAGE_H
#define ETHERCAT_MESSAGE_H

#include <QObject>


//NOTE:这种有继承的需要单独用一个cpp和h文件定义，否则会出错
class EtherCAT_Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int AppID READ get_appID)//只读属性
    Q_PROPERTY(int SlaveIndex READ get_slaveIndex)//只读属性
    Q_PROPERTY(int VirtualSlaveIndex READ get_VirtualslaveIndex)//只读属性
public:
    explicit EtherCAT_Message(QObject *parent = nullptr);
    virtual ~EtherCAT_Message(){}

    void Set_StatusMessage(QString message, int interval);

    void Set_BottomMessage(QString message);

    void Set_MasterStop();

    int get_appID() const{ return m_appID; }
    void set_appID(int id)  { m_appID = id; }
    int get_slaveIndex() const{ return m_slaveIndex; }
    void set_slaveIndex(int index)  { m_slaveIndex = index; }
    int get_VirtualslaveIndex() const{ return m_VirtualslaveIndex; }
    void set_VirtualslaveIndex(int index)  { m_VirtualslaveIndex = index; }
protected:
    int m_appID;
    int m_slaveIndex;
    int m_VirtualslaveIndex;
signals:
    void StatusMessage_change(QString message,int interval);//状态栏信息
    void BottomMessage_change(QString message);//bottom Text message
    void MasterStop_Signal();//stop master
public slots:
};

#endif // ETHERCAT_MESSAGE_H
