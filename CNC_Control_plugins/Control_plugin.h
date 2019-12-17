#ifndef CONTROL_PLUGIN_H
#define CONTROL_PLUGIN_H

//#include <QGenericPlugin>
#include <QObject>
#include "EtherCAT_UserApp.h"

#include <QObject>
#include "My_MotorApp_Callback.h"
#include "Form_ControlTab.h"

class Control_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "CNC_Control_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)

public:
    Control_plugin(QObject *parent = nullptr);
    ~Control_plugin();

    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp* get_NewAppPtr(QObject *parent = nullptr);
private:

    Form_ControlTab *user_form_controlTab;
    My_MotorApp_Callback *m_motorApp_callback;//Ethercat应用回调
    EtherCAT_Message *m_messageObj;

    QQueue<Gcode_segment> *m_GcodeSegment_Q;
    GcodeParser *gp_t;
    QString m_GcodePath;
    QString m_GcodePath_full;

    QThread *GcodeSendThread;
    bool controlTab_isTheta_display;
    bool controlTab_isLoadFileSafe;//也就是读取文件的时候，从站恢复到init状态
    QString m_settingPath;
    int m_SlaveChoose_indexLast;

    int Gcode_load(QString &fileName);
    void Set_BottomMessage(QString message);
    void Set_StatusMessage(QString message, int interval);
    void Set_MasterStop();

protected:
    int Load_setting(const QString &path);
    int Save_setting(const QString &path);
signals:
//    void StatusMessage_change(QString message,int interval);//状态栏信息
//    void BottomMessage_change(QString message);//bottom Text message
//    void MasterStop_Signal();//stop master
public slots:

private slots:
    void Control_OpenGcode_clicked();
    void Control_ReloadGcode_clicked();
    void Control_SendGcode_clicked();

    void MotorCallback_GcodeLineChange(int line);
    void MotorCallback_GcodePositionChange(QVector3D pos);
    void MotorCallback_GcodeThetaChange(QVector3D theta);

    void ControlTab_checkThetaDis_stateChange(int arg);
    void ControlTab_checkModeCalibrate_stateChange(int arg);
    void ControlTab_checkLoadFileSafe_stateChange(int arg);
    void ControlTab_jog_clicked(int button);
    void ControlTab_MasterIndex_currentIndexChanged(int index);

    void MotorCallback_MasterQuit_sig(bool isQuit);
    void MotorCallback_MasterScan_sig();
    void MotorCallback_BottomMsgChange_sig(QString message);
    void ControlTab_keyPressEvent(QKeyEvent *event);
};

#endif // USERAPP_PLUGIN_H
