#ifndef ETHERCAT_MESSAGE_H
#define ETHERCAT_MESSAGE_H

#include <QObject>


//NOTE:这种有继承的需要单独用一个cpp和h文件定义，否则会出错
class EtherCAT_Message : public QObject
{
    Q_OBJECT
public:
    explicit EtherCAT_Message(QObject *parent = nullptr);
    void Set_StatusMessage(QString message, int interval);

    void Set_BottomMessage(QString message);

    void Set_MasterStop();
signals:
    void StatusMessage_change(QString message,int interval);//状态栏信息
    void BottomMessage_change(QString message);//bottom Text message
    void MasterStop_Signal();//stop master
public slots:
};

#endif // ETHERCAT_MESSAGE_H
