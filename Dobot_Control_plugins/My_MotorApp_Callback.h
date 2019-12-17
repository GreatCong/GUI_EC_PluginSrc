#ifndef MY_MOTORAPP_CALLBACK_H
#define MY_MOTORAPP_CALLBACK_H

//#include "EtherCAT_UserApp.h"
#include "EtherCAT_UserApp.h"
#include <QQueue>
#include <QThread>

#include "GcodeParser.h"
#include "Dobot_Motion.h"


class My_MotorApp_Callback : public QObject,public Ethercat_Callback
{
    Q_OBJECT
public:
    explicit My_MotorApp_Callback(QObject *parent = nullptr);
    ~My_MotorApp_Callback();
    virtual void Master_AppLoop_callback();
    virtual void Master_AppStart_callback();
    virtual void Master_AppStop_callback();
    virtual void Master_AppScan_callback();
    virtual int Master_setAdressBase(char* address);
    virtual void Master_ReleaseAddress();

    virtual void Gcode_setAddress(QQueue<Gcode_segment> *q_addr);
    virtual void Gcode_ReleaseAddress();

//    const int16_t* output_ptr;
//    uint16_t* input_ptr;
//    uint32_t* input_MotorStep_ptr;
    int loop_count[Dobot_Motion::AXIS_N];
    bool m_sys_reset;

    ARM_Struct *m_ARM_Motion_test;
    ARM_Struct *m_PositionInit;//笛卡尔坐标系为(0,0,0)的角度

    int Planner_BufferLine(float *target, const Gcode_segment &userData, int robot_index);

    typedef enum{
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    }step_axis_3;

    typedef enum{
        AXIS_1,
        AXIS_2,
        AXIS_3,
        AXIS_4,
        AXIS_5,
        AXIS_6,
        AXIS_TOOL1
    }step_axis_6;

    typedef enum{
        motor1_step,
        motor2_step,
        motor3_step,
        motor4_step,
        motor5_step,
        motor6_step
    }step32_enum;

    typedef enum{
        io_output,
        step_setting,
        step_fre,
        motor_dir
    }step16_enum;

    typedef enum{
        step_enable,
        step_AutoRun_start,
        step_AutoRun_stop,

        step_wait_limitX = 8,
        step_wait_limitY,
        step_wait_limitZ,
        step_wait_limitA,
        step_wait_limitB,
        step_wait_limitC
    }step_setting_enum;

    typedef enum{
        motor_MSG,
        error_MSG,
        Output_machine_param,
        Output_limit_state,
        AD_CH1,
        AD_CH2,
        AD_CH3
    }output_MSG_enum;

    typedef enum{
        Planner_OK,
        Planner_EmptyBlock,
        Planner_ErrorBlack
    }planner_enum;

    void Arm_motion_reset();
    void Control_QueueClear();
    void set_RenewST_Ready(bool isReady);
    bool is_InputPtr_Release();
    void set_Mode_Calibrate(bool mode);
private:

    Dobot_Motion m_DobotMotion;//Dobot的正反解

    void Motor_Reset();
//    int Planner_BufferLine(float *target, int userData);
    QQueue<Gcode_segment> *m_GcodeSegment_Q;
    bool isRun;
    int m_robot_index;//机器人的编号
    int m_robot_LastIndex;//机器人的编号
    bool m_isMode_calibrate;
    void GcodeSendThread_Func(Dobot_Motion &dobotMotion);
    void Send_BottomMessage_change(QString message);
public slots:
    void GcodeSendThread_run();
signals:
    void Gcode_lineChange(int line);
    void Gcode_PositionChange(QVector3D pos);//发送正解信息
    void Gcode_ThetaChange(QVector3D theta);//发送反解信息
    void Master_QuitSignal(bool isQuit);
    void Master_ScanSignal();
    void Control_BottomMessage_change(QString message);//在底部显示区显示错误等信息
};

#endif // MY_MOTORAPP_CALLBACK_H
