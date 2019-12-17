#ifndef APPUSER_MESSAGE_H
#define APPUSER_MESSAGE_H

#include "EtherCAT_Message.h"

class AppUser_Message : public EtherCAT_Message
{
    Q_OBJECT
    Q_PROPERTY(bool isAppRun READ get_isAppRun WRITE set_isAppRun NOTIFY sig_isAppRun_Changed)
    Q_PROPERTY(bool isHideRight READ get_isHideRight WRITE set_isHideRight NOTIFY sig_isHideRight_Changed)
public:
    AppUser_Message(QObject *parent=nullptr);
    virtual ~AppUser_Message();

    bool get_isAppRun() const { return m_isAppRun;}
    void set_isAppRun(bool isRun) { m_isAppRun = isRun; emit sig_isAppRun_Changed(isRun);}

    bool get_isHideRight() const { return m_isHideRight;}
    void set_isHideRight(bool isHide) { m_isHideRight = isHide;emit sig_isHideRight_Changed(isHide);}

private:
    bool m_isAppRun;
    bool m_isHideRight;
signals:
    void sig_isAppRun_Changed(bool isRun);
    void sig_isHideRight_Changed(bool isHide);
};

#endif // APPUSER_MESSAGE_H
