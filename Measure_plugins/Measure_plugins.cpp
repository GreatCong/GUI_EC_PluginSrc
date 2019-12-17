#include "Measure_plugins.h"
#include <QGridlayout>
#include <QPushButton>

Measure_plugin::Measure_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("Measure example");
    QPixmap pixmap(":/Controls/Resource/Control/Measure_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

Measure_plugin::~Measure_plugin(){
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

bool Measure_plugin::Init_Cores()
{
   return true;
}

bool Measure_plugin::Destroy_Cores()
{
    return true;
}

bool Measure_plugin::Init_Object()
{
    m_userWidget = new Form_plot();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(nullptr);
    set_MessageObj(nullptr);

    return true;
}

EtherCAT_UserApp *Measure_plugin::get_NewAppPtr(QObject *parent)
{
    return new Measure_plugin(parent);
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Measure_plugins, Measure_plugin)
#endif // QT_VERSION < 0x050000
