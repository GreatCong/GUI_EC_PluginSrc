#include "userapp_plugin.h"
#include <QGridLayout>
#include <QPushButton>

UserApp_plugin::UserApp_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("User_App example");
//    QPixmap pixmap(":/Controls/Resource/Control/App_icon.png");
//    this->set_AppIcon(QIcon(pixmap));
}

bool UserApp_plugin::Init_Object()
{
    m_userWidget = new Form_plot();
    m_msg = new MyMessageObj();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(nullptr);
    set_MessageObj(m_msg);

    return true;
}

EtherCAT_UserApp *UserApp_plugin::get_NewAppPtr(QObject *parent)
{
    return new UserApp_plugin(parent);
}

void UserApp_plugin::sig_msgTest(int data)
{
    if(data == 1){
        m_userWidget->getTimerPtr()->start(1000);
    }
    else if(data == 0){
        m_userWidget->getTimerPtr()->stop();
    }
}

UserApp_plugin::~UserApp_plugin(){
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

bool UserApp_plugin::Init_Cores()
{
    connect(m_msg,SIGNAL(sig_x(int)),this,SLOT(sig_msgTest(int)));
   return true;
}

bool UserApp_plugin::Destroy_Cores()
{
    return true;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
