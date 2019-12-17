#include "Form_MatlabApp.h"
#include "ui_Form_MatlabApp.h"

///
/// \brief 构造函数
/// \param parent
///
Form_MatlabApp::Form_MatlabApp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_MatlabApp)
{
    ui->setupUi(this);

    this->Init_cores();
}

///
/// \brief 析构函数
///
Form_MatlabApp::~Form_MatlabApp()
{
    this->Destroye_cores();

    delete ui;
}

///
/// \brief 初始化应用
/// \return
///
bool Form_MatlabApp::Init_cores()
{
    return true;
}

///
/// \brief 销毁应用
/// \return
///
bool Form_MatlabApp::Destroye_cores()
{
    return true;
}
