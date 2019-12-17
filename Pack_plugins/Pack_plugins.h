#ifndef USERAPP_PLUGIN_H
#define USERAPP_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "Form_Pack.h"

#include "VersionModel.h"

class Pack_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "Pack_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    Form_Pack *m_userWidget;
    virtual ~Pack_plugin();
public:
    Pack_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual bool Init_Slots();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private:
    VersionModel m_versionModel;
private slots:
    void packWidget_comboBox_changed(int choose,const QString str);
    void packWidget_pushButton_changed(int choose);
    void packWidget_dragEvent(const QString str);
};

#endif // USERAPP_PLUGIN_H
