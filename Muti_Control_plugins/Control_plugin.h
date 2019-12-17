#ifndef CONTROL_PLUGIN_H
#define CONTROL_PLUGIN_H

//#include <QGenericPlugin>
#include <QObject>
#include "EtherCAT_UserApp.h"

#include "My_MotorApp_Callback.h"
#include "Form_ControlTab.h"
#include "Excel_Object.h"
#include "Form_Robot_Parameter.h"
#include "AppUser_Message.h"

class Control_plugin : public QObject,EtherCAT_UserApp
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID UserApp_iid FILE "Muti_Control_plugins.json")
#endif // QT_VERSION >= 0x050000

    Q_INTERFACES(EtherCAT_UserApp)

public:
    Control_plugin(QObject *parent = nullptr);

    virtual ~Control_plugin();
    virtual bool Init_Cores();
    virtual bool Destroy_Cores();
    virtual bool Init_Object();
    virtual EtherCAT_UserApp *get_NewAppPtr(QObject *parent = nullptr);
private:
    int Gcode_load(QString &fileName);

    void Set_BottomMessage(QString message);
    void Set_StatusMessage(QString message, int interval);
    void Set_MasterStop();

    bool Init_messageSlots();
    bool Init_Slots();

    int Load_setting(const QString &path);
    int Save_setting(const QString &path);
private:
    Form_ControlTab *user_form_controlTab;
    My_MotorApp_Callback *m_motorApp_callback;//Ethercat应用回调
    AppUser_Message *m_userMessage;

    QQueue<Gcode_segment> *m_GcodeSegment_Q;
    GcodeParser *gp_t;
    QString m_GcodePath;
    QString m_GcodePath_full;

    QThread *GcodeSendThread;
    bool controlTab_isTheta_display;
    bool controlTab_isLoadFileSafe;
    QString m_settingPath;
    int m_SlaveChoose_indexLast;

    Excel_Object m_excel_obj;
    Form_Robot_Parameter *m_Form_Robot_Parameter;
    QList<Form_Robot_Parameter *> m_Form_Robot_Parameter_list;//用于防止内存泄漏的
private slots:
    void Control_OpenGcode_clicked();
    void Control_ReloadGcode_clicked();
    void Control_SendGcode_clicked();
    void Control_SendCommand_clicked();

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
    void ControlTab_keyPressEvent(QKeyEvent *event);
    void MotorCallback_BottomMsgChange_sig(QString message);

    void MotorCallback_Robot_ID_handler(QVector<int> array_robot_id,QVector<int> array_robot_index);
    void MotorCallback_Robot_PosInit_handler(QVector3D pos);
    void MotorCallback_Robot_WarningBox_handler(QString message);

    void messageSig_isAppRun_Changed(bool isRun);
    void messageSig_isHideRight_Changed(bool isHide);
};

#endif // USERAPP_PLUGIN_H
