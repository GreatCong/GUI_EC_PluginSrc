#include "Matlab_plugins.h"
#include <QGridlayout>
#include <QPushButton>

Matlab_plugin::Matlab_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("Matlab Interface");
    QPixmap pixmap(":/Controls/Resource/Control/MATLAB_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

bool Matlab_plugin::Init_Object()
{
    m_userWidget = new Form_MatlabApp();
    m_callback_Matlab = new Callback_Matlab();
    m_master_Message = new EtherCAT_Message();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(m_callback_Matlab);
    set_MessageObj(m_master_Message);

    return true;
}

EtherCAT_UserApp *Matlab_plugin::get_NewAppPtr(QObject *parent)
{
    return new Matlab_plugin(parent);
}

Matlab_plugin::~Matlab_plugin(){
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

bool Matlab_plugin::Init_Cores()
{
   if(m_callback_Matlab->Init_cores() == false){
       m_master_Message->Set_BottomMessage("Callback init fail!");
       return false;
   }

   m_master_Message->Set_BottomMessage("Callback init OK!");

   return true;
}

bool Matlab_plugin::Destroy_Cores()
{
    m_callback_Matlab->Destroy_cores();

    return true;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
