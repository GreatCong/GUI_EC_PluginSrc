#include "Serial_Handle.h"
#include <QDebug>

///
/// \brief 构造函数
/// \param parent
///
Serial_Handle::Serial_Handle(QObject *parent):QObject(parent)
{
    m_serial = nullptr;
}

///
/// \brief 析构函数
///
Serial_Handle::~Serial_Handle()
{

}

///
/// \brief 设置serial的指针
/// \param serial
///
void Serial_Handle::set_serialPtr(QSerialPort *serial)
{
    if(m_serial){
        disconnect(m_serial,SIGNAL(readyRead()),this,SLOT(Serial_readyRead_handle()));
    }

    m_serial = serial;
    connect(m_serial,SIGNAL(readyRead()),this,SLOT(Serial_readyRead_handle()));
}

///
/// \brief Serial_Handle::get_robot_id
/// \param id
/// \return
///
bool Serial_Handle::get_robot_id(int *id)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_id;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,4);
    if(receive_data[0] == CMD_Package_head && receive_data[3] == CMD_Package_tail){
        *id = receive_data[2];
        return true;
    }

    *id = 0;
    return false;
}

///
/// \brief Serial_Handle::set_robot_id
/// \param id
/// \return
///
bool Serial_Handle::set_robot_id(int id)
{
    uint8_t send_data[5] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Set_id;
    send_data[3] = (char)id;
    send_data[4] = CMD_Package_tail;

    m_serial->write((char*)send_data,5);

    return true;
}

///
/// \brief Serial_Handle::get_limitRunDir_mask
/// \param mask
/// \return
///
bool Serial_Handle::get_limitRunDir_mask(int *mask)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_limitRunDir_mask;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读2s

    m_serial->read((char*)receive_data,4);
    if(receive_data[0] == CMD_Package_head && receive_data[3] == CMD_Package_tail){
        *mask = receive_data[2];
        return true;
    }

    *mask = 0;
    return false;
}

///
/// \brief Serial_Handle::set_limitRunDir_mask
/// \param mask
/// \return
///
bool Serial_Handle::set_limitRunDir_mask(int mask)
{
    uint8_t send_data[5] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Set_limitRunDir_mask;
    send_data[3] = (char)mask;
    send_data[4] = CMD_Package_tail;

    m_serial->write((char*)send_data,5);

    return true;
}

///
/// \brief Serial_Handle::get_limitEnable_mask
/// \param mask
/// \return
///
bool Serial_Handle::get_limitEnable_mask(int *mask)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_limitEnable_mask;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,4);
    if(receive_data[0] == CMD_Package_head && receive_data[3] == CMD_Package_tail){
        *mask = receive_data[2];
        return true;
    }

    *mask = 0;
    return false;
}

///
/// \brief Serial_Handle::set_limitEnable_mask
/// \param mask
/// \return
///
bool Serial_Handle::set_limitEnable_mask(int mask)
{
    uint8_t send_data[5] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Set_limitEnable_mask;
    send_data[3] = (char)mask;
    send_data[4] = CMD_Package_tail;

    m_serial->write((char*)send_data,5);

    return true;
}

///
/// \brief Serial_Handle::get_limitMode
/// \param mode
/// \return
///
bool Serial_Handle::get_limitMode(int *mode)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_limitMode;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,4);
    if(receive_data[0] == CMD_Package_head && receive_data[3] == CMD_Package_tail){
        *mode = receive_data[2];
        return true;
    }

    *mode = 0;
    return false;
}

///
/// \brief Serial_Handle::set_limitMode
/// \param mode
/// \return
///
bool Serial_Handle::set_limitMode(int mode)
{
    uint8_t send_data[5] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Set_limitMode;
    send_data[3] = (char)mode;
    send_data[4] = CMD_Package_tail;

    m_serial->write((char*)send_data,5);

    return true;
}

///
/// \brief Serial_Handle::get_limitGoHomeSpeed
/// \param speed
/// \return
///
bool Serial_Handle::get_limitGoHomeSpeed(int *speed)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_limitGoHome_speed;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,5);
    if(receive_data[0] == CMD_Package_head && receive_data[4] == CMD_Package_tail){
        *speed = (uint16_t)((receive_data[2] << 8) | receive_data[3]);//高位在前，低位在后
        return true;
    }

    *speed = 0;
    return false;
}

///
/// \brief Serial_Handle::set_limitGoHomeSpeed
/// \param speed
/// \return
///
bool Serial_Handle::set_limitGoHomeSpeed(int speed)
{
    uint8_t send_data[6] = {0};
    uint16_t data_tmp =0;
    data_tmp = (uint16_t)speed;

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Set_limitGoHome_speed;
    send_data[3] = (uint8_t)(data_tmp>>8);//高位在前，低位在后
    send_data[4] = (uint8_t)data_tmp;
    send_data[5] = CMD_Package_tail;

    m_serial->write((char*)send_data,6);

    return true;
}

///
/// \brief Serial_Handle::get_limitAngle
/// \param index
/// \param data
/// \return
///
bool Serial_Handle::get_limitAngle(int index, float *data)
{
    uint8_t send_data[5] = {0};
    uint8_t receive_data[5] = {0};
    uint16_t data_tmp =0;

    send_data[0] = CMD_Package_head;
    send_data[1] = 2;
    send_data[2] = CMD_Get_limitAngle;
    send_data[3] = (uint8_t)index;
    send_data[4] = CMD_Package_tail;

    m_serial->write((char*)send_data,5);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,5);
    if(receive_data[0] == CMD_Package_head && receive_data[4] == CMD_Package_tail){
        data_tmp = (uint16_t)((receive_data[2] << 8) | receive_data[3]);//高位在前，低位在后
        *data = (float)data_tmp / 10.0;

        return true;
    }

    *data = 0;
    return false;
}

///
/// \brief Serial_Handle::set_limitAngle
/// \param index
/// \param data
/// \return
///
bool Serial_Handle::set_limitAngle(int index, float data)
{
    uint8_t send_data[7] = {0};
    uint16_t data_tmp =0;
    data_tmp = (uint16_t)std::lroundf(data*10);//放大10倍

    send_data[0] = CMD_Package_head;
    send_data[1] = 4;
    send_data[2] = CMD_Set_limitAngle;
    send_data[3] = index;
    send_data[4] = (uint8_t)(data_tmp>>8);//高位在前，低位在后
    send_data[5] = (uint8_t)data_tmp;
    send_data[6] = CMD_Package_tail;

    m_serial->write((char*)send_data,7);

    return true;
}

///
/// \brief Serial_Handle::write_to_flash
/// \return
///
bool Serial_Handle::write_to_flash()
{
    uint8_t send_data[4] = {0};

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_WR_Flash;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    return true;
}

///
/// \brief Serial_Handle::get_version
/// \param str
/// \return
///
bool Serial_Handle::get_version(QString *str)
{
    uint8_t send_data[4] = {0};
    uint8_t receive_data[5] = {0};
    int ver_major = 0;
    int ver_minor = 0;

    send_data[0] = CMD_Package_head;
    send_data[1] = 1;
    send_data[2] = CMD_Get_version;
    send_data[3] = CMD_Package_tail;

    m_serial->write((char*)send_data,4);

    m_serial->waitForReadyRead(1000);//等待可读

    m_serial->read((char*)receive_data,5);
    if(receive_data[0] == CMD_Package_head && receive_data[4] == CMD_Package_tail){
        ver_major = receive_data[2];
        ver_minor = receive_data[3];

        *str = "Version "+QString::number(ver_major)+"."+QString::number(ver_minor);
        return true;
    }

    *str = "Version = NULL";
    return false;
}

///
/// \brief Serial_Handle::serial_isOpen
/// \return
///
bool Serial_Handle::serial_isOpen()
{
    return m_serial->isOpen();
}

/****************** SLOT begin **********************************/
///
/// \brief 串口的事件
///
void Serial_Handle::Serial_readyRead_handle()
{
//    qDebug() << m_serial->readAll();
}

/****************** SLOT end **********************************/
