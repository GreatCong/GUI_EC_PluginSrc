#include "EtherCAT_Message.h"

EtherCAT_Message::EtherCAT_Message(QObject *parent) : QObject(parent)
{

}

void EtherCAT_Message::Set_StatusMessage(QString message, int interval)
{
    emit StatusMessage_change(message,interval);//发出自定义信号
}

void EtherCAT_Message::Set_BottomMessage(QString message)
{
    emit BottomMessage_change(message);//发出自定义信号
}

void EtherCAT_Message::Set_MasterStop()
{
    emit MasterStop_Signal();
}
