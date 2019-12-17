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

        RobotChange = 500,//切换机器人
        RobotChange1 = 501,//切换机器人1
        RobotChange2 = 502,
        RobotChange3 = 503,
        COMMENT_CODE = -1000,//注释行，保证行号对齐
        EndParse_CODE = -1001//表示G代码解析完毕
    }McodeDef_enum;

    QVector3D data_xyz;
    int line;
    int Mcode;
    int others;
    double speed_step;
    double speed_spindle;

    Gcode_segment(){
        line = 0;
        Mcode = No_Mcode;
        speed_step = 0;
        speed_spindle = 0;
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
signals:

public slots:
};

#endif // GCODEPARSER_H
