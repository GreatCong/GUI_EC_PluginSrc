#ifndef GCODEPARSER_H
#define GCODEPARSER_H

#include <QObject>
#include <QVector3D>
#include <cmath>
#include "GcodePreprocessorUtils.h"
#include <QQueue>

class Gcode_segment{
public:
    typedef enum{
        No_Mcode = -1,
        Invalid_Mcode = -2,
        MotorTool_On = 100,
        MotorTool_Off = 101,
        MotorTool1_On = 102,
        MotorTool1_Off = 103,
        LIMIT_Wait_X = 201,//等待X轴的限位信号
        LIMIT_Wait_Y,
        LIMIT_Wait_Z,
        LIMIT_Wait_A,
        LIMIT_Wait_B,
        LIMIT_Wait_C,

        RobotChange = 500,//切换机器人
        RobotChange1 = 501,//切换机器人1
        RobotChange2 = 502,
        RobotChange3 = 503,
        RobotChange4,
        RobotChange5,
        RobotChange6,
        RobotChange7,
        RobotChange8,
        RobotChange9,
        RobotChange10,
        RobotChange11,
        RobotChange12,
        RobotChange13,
        RobotChange14,
        RobotChange15,
        RobotChange16,
        RobotChange17,
        RobotChange18,
        RobotChange19,
        RobotChange20,//520,
        RobotChange_end,//521
        COMMENT_CODE = -1000,//注释行，保证行号对齐
        EndParse_CODE = -1001,//表示G代码解析完毕
        SpeedOnly_CODE = -1002,//只有F的G代码行
        SpindleOnly_CODE = -1003,//只有S的G代码行
        DwellOnly_CODE = -1004, //只有P的G代码行
        G90_CODE = -1005,//G90(设置绝对坐标系模式)
        G91_CODE = -1006,//G91(设置相对坐标系模式)
        G92_CODE = -1007 //G92(设置坐标系偏移基地址)

    }McodeDef_enum;

    ~Gcode_segment(){ //测试内存泄漏情况
//        qDebug() <<"~Gcode_segment"<< data_xyz;
    }

    QVector3D data_xyz;
    QVector3D data_offsetBase;
    int line;
    int Mcode;
    int others;
    double speed_step;
    double speed_spindle;
    int index_robot;
    bool is_inAbsoluteMode;

    Gcode_segment(){
        line = 0;
        Mcode = No_Mcode;
        speed_step = 0;
        speed_spindle = 0;
        index_robot = 0;
        is_inAbsoluteMode = true;//默认是绝对坐标系
        data_offsetBase = QVector3D(0,0,0);//相对坐标下的偏移坐标
    }
};

class GcodeParser : public QObject
{
    Q_OBJECT
public:
    explicit GcodeParser(QObject *parent = nullptr);

    int addCommand(QString command);
    int addCommand(const QStringList &args);
    int processCommand(const QStringList &args);
    int handleGCode(float code, const QStringList &args);
    int addLinearPointSegment(const QVector3D &nextPoint, bool fastTraverse);
    QQueue<Gcode_segment> *getGodeQueue();
    void clearQueue();
    int handleMCode(float code);

    void setCurrentPoint(const QVector3D point);
    void init_machineList();
private:
    // Current state
    bool m_isMetric;
    bool m_inAbsoluteMode;
    bool m_inAbsoluteIJKMode;
    float m_lastGcodeCommand;
    QVector3D m_currentPoint;
    int m_commandNumber;
    double m_lastSpeed;//步进电机转速
    double m_traverseSpeed;//快速移动（G0）的转速
    double m_lastSpindleSpeed;//主轴电机的转速
    bool m_isChangeRobot;

    QQueue<Gcode_segment> *m_Gcode_Q;
    QList<Gcode_segment*> m_List_Machine;

    void check_MachineList(int robotIndex);
signals:

public slots:
};

#endif // GCODEPARSER_H
