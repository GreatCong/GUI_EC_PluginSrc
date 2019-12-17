#include "Robot_setting_plugin.h"
#include <QGridlayout>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QDebug>

///
/// \brief 构造函数
/// \param parent
///
Robot_setting_plugin::Robot_setting_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("Robot_setting Tool");
    QPixmap pixmap(":/Controls/Resource/Control/Tool_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

///
/// \brief 析构函数
///
Robot_setting_plugin::~Robot_setting_plugin(){
    //NOTE:用get_UIWidgetPtr函数，不能用user_form_controlTab这样的
    if(this->get_UIWidgetPtr()){
       delete this->get_UIWidgetPtr();
    }

    if(this->get_CallbackPtr()){
        delete this->get_CallbackPtr();
    }

    if(this->get_MessageObj()){
        delete this->get_MessageObj();
    }
}

///
/// \brief 初始化应用
/// \return
///
bool Robot_setting_plugin::Init_Cores()
{    
    this->Init_Slots();

    m_serial=new QSerialPort();
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    m_serial_handle.set_serialPtr(m_serial);

    if(false == m_excel_obj.readExcel(this->get_appPath()+"/../Machine_libs/Robot_DH.xlsx")){//读取excel
       QMessageBox::warning(this->get_UIWidgetPtr(),"Warning","excel path is invalid!");
       return false;
    }

    m_userWidget->get_TableWidgePtr(Form_Robot_Setting::tableWidget_robotMessage)->clearContents();
    m_userWidget->get_TableWidgePtr(Form_Robot_Setting::tableWidget_robotMessage)->setRowCount(m_excel_obj.get_Array_robotParameter().size());

    int robot_index = 0;
    QTableWidgetItem *tableItem_tmp = nullptr;
    foreach (Robot_parameter_s robot_param, m_excel_obj.get_Array_robotParameter()) {
        tableItem_tmp = new QTableWidgetItem(QString::number(robot_param.value_id));
        tableItem_tmp->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_userWidget->get_TableWidgePtr(Form_Robot_Setting::tableWidget_robotMessage)->setItem(robot_index,0,tableItem_tmp);

        tableItem_tmp = new QTableWidgetItem(robot_param.value_name);
        tableItem_tmp->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        m_userWidget->get_TableWidgePtr(Form_Robot_Setting::tableWidget_robotMessage)->setItem(robot_index++,1,tableItem_tmp);
    }

    return true;
}

///
/// \brief 销毁应用
/// \return
///
bool Robot_setting_plugin::Destroy_Cores()
{
    if(m_serial->isOpen()){
        m_serial->close();
    }

    delete m_serial;

    return true;
}

///
/// \brief 初始化对象
/// \return
///
bool Robot_setting_plugin::Init_Object()
{
    m_userWidget = new Form_Robot_Setting();
    m_EtherCAT_msg = new EtherCAT_Message();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(nullptr);
    set_MessageObj(m_EtherCAT_msg);

    return true;
}

///
/// \brief 初始化信号槽
/// \return
///
bool Robot_setting_plugin::Init_Slots()
{
    connect(m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_Serial_open_p),SIGNAL(clicked(bool)),this,SLOT(Serial_open_handle(bool)));
    connect(m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_Serial_search_p),SIGNAL(clicked(bool)),this,SLOT(Serial_search_handle(bool)));
    connect(m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_get_MSG_p),SIGNAL(clicked(bool)),this,SLOT(Serial_getMSG_handle(bool)));
    connect(m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_set_MSG_p),SIGNAL(clicked(bool)),this,SLOT(Serial_setMSG_handle(bool)));

    return true;
}

///
/// \brief 得到新的对象
/// \param parent
/// \return
///
EtherCAT_UserApp *Robot_setting_plugin::get_NewAppPtr(QObject *parent)
{
    return new Robot_setting_plugin(parent);
}

/************************** SLot begin *********************************/

///
/// \brief 打开串口
/// \param isClick
///
void Robot_setting_plugin::Serial_open_handle(bool isClick)
{
    Q_UNUSED(isClick);
    if(m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_Serial_open_p)->text() == QString::fromLocal8Bit("打开串口")){
        if(m_userWidget->get_ComboBoxPtr(Form_Robot_Setting::comboBox_Serial_name_c)->count() > 0){
            QString port_name = m_userWidget->get_ComboBoxPtr(Form_Robot_Setting::comboBox_Serial_name_c)->currentText();
            m_serial->setPortName(port_name);
            if(m_serial->open(QIODevice::ReadWrite)){
               m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_Serial_open_p)->setText(QString::fromLocal8Bit("关闭串口"));
            }
            else{
                QMessageBox::critical(m_userWidget,"Serial Error","Serial Port is invalid!");
            }
        }

    }
    else{
        if(m_serial->isOpen()){
            m_serial->close();
            m_userWidget->get_PushButtonPtr(Form_Robot_Setting::pushButton_Serial_open_p)->setText(QString::fromLocal8Bit("打开串口"));
        }
    }
}

///
/// \brief 查找串口
/// \param isClick
///
void Robot_setting_plugin::Serial_search_handle(bool isClick)
{
    Q_UNUSED(isClick);

    m_userWidget->get_ComboBoxPtr(Form_Robot_Setting::comboBox_Serial_name_c)->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite)){
            m_userWidget->get_ComboBoxPtr(Form_Robot_Setting::comboBox_Serial_name_c)->addItem(serial.portName());
             serial.close();
        }
    }
}

///
/// \brief 读配置数据
/// \param isClick
///
void Robot_setting_plugin::Serial_getMSG_handle(bool isClick)
{
    Q_UNUSED(isClick);
    Robot_setting_t robot_setting;

    if(!m_serial_handle.serial_isOpen()){
        QMessageBox::warning(this->get_UIWidgetPtr(),"Warning","Serial is not open!");
        return;
    }

    if(m_serial_handle.get_version(&(robot_setting.ver_str))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_version].setText(robot_setting.ver_str);
    }

    if( m_serial_handle.get_robot_id(&(robot_setting.robot_id))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_robot_id].setText(QString::number(robot_setting.robot_id));
    }

    if( m_serial_handle.get_limitEnable_mask(&(robot_setting.limit_enable_mask))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitEnable_mask].setText(QString::number(robot_setting.limit_enable_mask));
    }

    if( m_serial_handle.get_limitRunDir_mask(&(robot_setting.limit_runDir_mask))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitRunDir_mask].setText(QString::number(robot_setting.limit_runDir_mask));
    }

    if( m_serial_handle.get_limitGoHomeSpeed(&(robot_setting.limit_goHomeSpeed))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitGoHomeSpeed].setText(QString::number(robot_setting.limit_goHomeSpeed));
    }

    if( m_serial_handle.get_limitMode(&(robot_setting.limit_mode))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitMode].setText(QString::number(robot_setting.limit_mode));
    }

    if( m_serial_handle.get_limitAngle(0,&(robot_setting.limit_angle[0]))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_X].setText(QString::number(robot_setting.limit_angle[0]));
    }

    if( m_serial_handle.get_limitAngle(1,&(robot_setting.limit_angle[1]))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_Y].setText(QString::number(robot_setting.limit_angle[1]));
    }

    if( m_serial_handle.get_limitAngle(2,&(robot_setting.limit_angle[2]))){
        m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_Z].setText(QString::number(robot_setting.limit_angle[2]));
    }

    m_EtherCAT_msg->Set_BottomMessage("Read robot param OK!");
}

///
/// \brief 写配置数据
/// \param isClick
///
void Robot_setting_plugin::Serial_setMSG_handle(bool isClick)
{
    Q_UNUSED(isClick);

    bool ret = false;
    Robot_setting_t robot_setting;

    if(!m_serial_handle.serial_isOpen()){
        QMessageBox::warning(this->get_UIWidgetPtr(),"Warning","Serial is not open!");
        return;
    }

    QString str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_robot_id].text();
    robot_setting.robot_id = str_temp.toInt();
    ret = m_serial_handle.set_robot_id(robot_setting.robot_id);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitEnable_mask].text();
    robot_setting.limit_enable_mask = str_temp.toInt();
    ret &= m_serial_handle.set_limitEnable_mask(robot_setting.limit_enable_mask);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitRunDir_mask].text();
    robot_setting.limit_runDir_mask = str_temp.toInt();
    ret &= m_serial_handle.set_limitRunDir_mask(robot_setting.limit_runDir_mask);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitMode].text();
    robot_setting.limit_mode = str_temp.toInt();
    ret &= m_serial_handle.set_limitMode(robot_setting.limit_mode);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitGoHomeSpeed].text();
    robot_setting.limit_goHomeSpeed = str_temp.toInt();
    ret &= m_serial_handle.set_limitGoHomeSpeed(robot_setting.limit_goHomeSpeed);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_X].text();
    robot_setting.limit_angle[0] = str_temp.toFloat();
    ret &= m_serial_handle.set_limitAngle(0,robot_setting.limit_angle[0]);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_Y].text();
    robot_setting.limit_angle[1] = str_temp.toFloat();
    ret &= m_serial_handle.set_limitAngle(1,robot_setting.limit_angle[1]);

    str_temp = m_userWidget->get_array_serialItemPtr()[Form_Robot_Setting::serialItem_limitAngle_Z].text();
    robot_setting.limit_angle[2] = str_temp.toFloat();
    ret &= m_serial_handle.set_limitAngle(2,robot_setting.limit_angle[2]);

    ret &= m_serial_handle.write_to_flash();

    if(ret){
        QMessageBox::information(m_userWidget,"Robot_setting_plugin","write OK!");
    }
    else{
        QMessageBox::critical(m_userWidget,"Robot_setting_plugin","write Error!");
    }

}

/************************** SLot end *********************************/

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
