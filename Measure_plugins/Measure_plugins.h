#ifndef MEASURE_PLUGIN_H
#define MEASURE_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "form_plot.h"

class Measure_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "Measure_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:
    QWidget *m_userWidget;
    virtual ~Measure_plugin();
public:
    Measure_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
};

#endif // USERAPP_PLUGIN_H
