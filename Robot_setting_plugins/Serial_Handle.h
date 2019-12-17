#ifndef SERIAL_HANDLE_H
#define SERIAL_HANDLE_H

#include <QObject>
#include <QSerialPort>

typedef struct Robot_setting_t{
    QString ver_str;
    int robot_id;
    int limit_enable_mask;
    int limit_runDir_mask;
    int limit_mode;
    int limit_goHomeSpeed;
    float limit_angle[3];

    Robot_setting_t(){
        ver_str = "";
        limit_enable_mask = -1;
        limit_runDir_mask = -1;
        limit_mode = -1;
        limit_goHomeSpeed = -1;

        for(int i=0;i<3;i++){
            limit_angle[i] = -1.0;
        }
    }
}Robot_setting_t;

class Serial_Handle :public QObject
{
    Q_OBJECT
public:
public:
    explicit Serial_Handle(QObject *parent = nullptr);
    virtual ~Serial_Handle();

    typedef enum{
        CMD_Package_head = 0x55,
        CMD_Package_tail = 0xaa,

        CMD_NULL = 0x7f,
        CMD_Get_id, //0x80
        CMD_Set_id, //0x81
        CMD_WR_Flash, //0x82
        CMD_Get_version, //0x83
        CMD_Get_limitRunDir_mask,//0x84
        CMD_Set_limitRunDir_mask,//0x85
        CMD_Get_limitEnable_mask,//0x86
        CMD_Set_limitEnable_mask,//0x87
        CMD_Get_limitMode,//0x88
        CMD_Set_limitMode,//0x89
        CMD_Get_limitGoHome_speed,//0x8a
        CMD_Set_limitGoHome_speed,//0x8b
        CMD_Get_limitAngle,//0x8c
        CMD_Set_limitAngle//0x8d
    }Cmd_type_e;

    void set_serialPtr(QSerialPort *serial);

    bool get_robot_id(int *id);
    bool set_robot_id(int id);
    bool get_limitRunDir_mask(int *mask);
    bool set_limitRunDir_mask(int mask);
    bool get_limitEnable_mask(int *mask);
    bool set_limitEnable_mask(int mask);
    bool get_limitMode(int *mode);
    bool set_limitMode(int mode);
    bool get_limitGoHomeSpeed(int *speed);
    bool set_limitGoHomeSpeed(int speed);
    bool get_limitAngle(int index,float *data);
    bool set_limitAngle(int index,float data);
    bool write_to_flash();
    bool get_version(QString *str);

    bool serial_isOpen();
private:
    QSerialPort *m_serial;
private slots:
    void Serial_readyRead_handle();
};

#endif // SERIAL_HANDLE_H
