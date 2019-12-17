#include "AppUser_Message.h"

AppUser_Message::AppUser_Message(QObject *parent):EtherCAT_Message(parent)
{
    m_isAppRun = false;
    m_isHideRight = true;
}

AppUser_Message::~AppUser_Message()
{

}
