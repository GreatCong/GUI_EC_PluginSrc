#ifndef DRE2008OS_PLUGIN_H
#define DRE2008OS_PLUGIN_H

#include <QObject>
#include "EtherCAT_UserApp.h"
#include "form_plot.h"
#include "DRE2008_OS_Callback.h"
#include "AppUser_Message.h"

class DRE2008OS_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "DRE2008OS_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)
private:  
    virtual ~DRE2008OS_plugin();
private:
    int Check_SampleParameter(int OS_ch, int AD_ch, int sampleRate);
    bool Init_messageSlots();

    Form_plot *m_userWidget;
    DRE2008_OS_Callback *m_userCallback;
    AppUser_Message *m_userMessage;

    QTimer *m_timePlot;
    int m_plotDisplay_Num;
    bool m_isMaster_Run;
    bool m_isDisFFT_reset;
public:
    DRE2008OS_plugin(QObject *parent = 0);

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private slots:
    void plot_pushButton_PlotStart_clicked();
    void plot_pushButton_PlotStop_clicked();
    void plot_pushButton_DisplayReset_clicked();
    void user_timeout_handle();

    void plot_dial_SampleRate_valueChanged(int value);
    void plot_dial_DisplayNum_valueChanged(int value);
    void plot_combobox_ADchannel_currentIndexChanged(int index);
    void plot_combobox_OSchannel_currentIndexChanged(int index);
    void plot_combobox_SlaveIndex_currentIndexChanged(int index);

    void callback_Master_RunStateChanged(bool isRun);
    void callback_Master_ScanChanged();

    void messageSig_isAppRun_Changed(bool isRun);
    void messageSig_isHideRight_Changed(bool isHide);
    void messageSig_displayNum_Changed(int num);

};

#endif // USERAPP_PLUGIN_H
