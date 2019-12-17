#include "Pack_plugins.h"
#include <QGridlayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>

#include <QDebug>

///
/// \brief 构造函数
/// \param parent
///
Pack_plugin::Pack_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("Pack Tool");
    QPixmap pixmap(":/Controls/Resource/Control/Tool_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

///
/// \brief 析构函数
///
Pack_plugin::~Pack_plugin(){
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
bool Pack_plugin::Init_Cores()
{
    this->Init_Slots();//初始化信号槽

    m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_installVer_c)->clear();
    m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_compileVer_c)->clear();

    if(m_versionModel.qtVersionList().size() > 0){
        m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_installVer_c)->addItems(m_versionModel.qtVersionList());
        m_versionModel.setQtVersion(m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_installVer_c)->currentText());//会在槽的回调中对compile进行加载

        m_userWidget->get_PushButtonPtr(Form_Pack::pushButton_create_p)->setEnabled(true);
    }
    else{
        QMessageBox::warning(this->get_UIWidgetPtr(),"warning","QT not found!");
        m_userWidget->get_PushButtonPtr(Form_Pack::pushButton_create_p)->setEnabled(false);
    }

   return true;
}

///
/// \brief 销毁应用
/// \return
///
bool Pack_plugin::Destroy_Cores()
{
    return true;
}

///
/// \brief 初始化对象
/// \return
///
bool Pack_plugin::Init_Object()
{
    m_userWidget = new Form_Pack();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(nullptr);
    set_MessageObj(nullptr);

    return true;
}

///
/// \brief 初始化信号槽
/// \return
///
bool Pack_plugin::Init_Slots()
{
    connect(m_userWidget,SIGNAL(sig_comboBox_changed(int,QString)),this,SLOT(packWidget_comboBox_changed(int,QString)));
    connect(m_userWidget,SIGNAL(sig_pushButton_changed(int)),this,SLOT(packWidget_pushButton_changed(int)));
    connect(m_userWidget,SIGNAL(sig_dragEvent(QString)),this,SLOT(packWidget_dragEvent(QString)));

    return true;
}

///
/// \brief 得到新对象
/// \param parent
/// \return
///
EtherCAT_UserApp *Pack_plugin::get_NewAppPtr(QObject *parent)
{
    return new Pack_plugin(parent);
}

/********************* Slot begin **************************/

///
/// \brief Pack_plugin::packWidget_comboBox_changed
/// \param choose
/// \param str
///
void Pack_plugin::packWidget_comboBox_changed(int choose, const QString str)
{
    switch(choose){
       case Form_Pack::comboBox_qt_installVer_c:
        m_versionModel.setQtVersion(str);
        m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_compileVer_c)->clear();
        //qDebug() << m_versionModel.compilerVersionList();
        m_userWidget->get_ComboBoxPtr(Form_Pack::comboBox_qt_compileVer_c)->addItems(m_versionModel.compilerVersionList());
        break;
    case Form_Pack::comboBox_qt_compileVer_c:
        m_versionModel.setCompilerVersion(str);
        break;
    default:
        break;
    }
}

///
/// \brief Pack_plugin::packWidget_pushButton_changed
/// \param choose
///
void Pack_plugin::packWidget_pushButton_changed(int choose)
{
    bool ret = false;

    switch(choose){
       case Form_Pack::pushButton_create_p:
        ret = m_versionModel.create();
        if(ret){
                QMessageBox::information(this->get_UIWidgetPtr(),"INFO","Create OK!");
        }
        else{
             QMessageBox::critical(this->get_UIWidgetPtr(),"INFO","Create ERROR!");
        }
        break;
    default:
        break;
    }
}

///
/// \brief Pack_plugin::packWidget_dragEvent
/// \param str
///
void Pack_plugin::packWidget_dragEvent(const QString str)
{
    m_versionModel.setExeFile(str);
}

/********************* Slot end **************************/

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
