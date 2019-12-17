#include "MyMessageObj.h"

MyMessageObj::MyMessageObj(QObject *parent):EtherCAT_Message(parent)
{
  x= -1;
  check_num = false;
}
