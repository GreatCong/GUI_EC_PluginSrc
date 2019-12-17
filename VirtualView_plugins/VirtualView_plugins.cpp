#include "VirtualView_plugins.h"
#include "EtherCAT_UserApp.h"
#include <QGridLayout>
#include <QPushButton>

VirtualView_plugin::VirtualView_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("VirtualView Interface");
    QPixmap pixmap(":/Controls/Resource/Control/VirtualView_icon.png");
    this->set_AppIcon(QIcon(pixmap));

    this->set_isUsePropertyWidget(true);//设置可以访问主窗口的左边的属性框
}

bool VirtualView_plugin::Init_Object()
{
    m_userWidget = new Form_VirtualView();
    m_master_Message = new EtherCAT_Message();
    m_userCallback = new VirtualViewApp_Callback();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(m_userCallback);
    set_MessageObj(m_master_Message);

    this->set_PropertyWidgetPtr(m_userWidget->get_GraphicsScenePtr()->get_WidgetObjectController());

    return true;
}

EtherCAT_UserApp *VirtualView_plugin::get_NewAppPtr(QObject *parent)
{
    return new VirtualView_plugin(parent);
}

VirtualView_plugin::~VirtualView_plugin(){
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

bool VirtualView_plugin::Init_Cores()
{
   m_userWidget->get_GraphicsViewPtr()->set_appPath(this->get_appPath());
   connect(m_userWidget->get_GraphicsViewPtr()->get_MessageObj(),SIGNAL(MasterStop_Signal()),this,SLOT(Control_MasterStop_Signal()));
   connect(m_userWidget->get_GraphicsViewPtr()->get_MessageObj(),SIGNAL(StatusMessage_change(QString,int)),this,SLOT(Control_StatusMessage_change(QString,int)));
   connect(m_userWidget->get_GraphicsViewPtr()->get_MessageObj(),SIGNAL(BottomMessage_change(QString)),this,SLOT(Control_BottomMessage_change(QString)));

   m_userCallback->set_graphicsViewPtr(m_userWidget->get_GraphicsViewPtr());//传递view的指针，用于ethercat的回调处理

   return true;
}

bool VirtualView_plugin::Destroy_Cores()
{
    return true;
}


/************* master Slot begin **************************/

void VirtualView_plugin::Control_StatusMessage_change(QString message,int interval){
   this->m_master_Message->Set_StatusMessage(message,interval);
}

void VirtualView_plugin::Control_BottomMessage_change(QString message){
    this->m_master_Message->Set_BottomMessage(message);
}

void VirtualView_plugin::Control_MasterStop_Signal(){
    this->m_master_Message->Set_MasterStop();
}

/************  master Slots end ***************/


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
