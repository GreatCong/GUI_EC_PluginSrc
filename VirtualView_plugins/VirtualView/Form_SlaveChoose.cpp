#include "Form_SlaveChoose.h"
#include "ui_Form_SlaveChoose.h"

Form_SlaveChoose::Form_SlaveChoose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_SlaveChoose)
{
    ui->setupUi(this);

    this->Init_cores();
}

Form_SlaveChoose::~Form_SlaveChoose()
{
    delete ui;

    this->Destroy_cores();
}

bool Form_SlaveChoose::Init_cores()
{
    this->get_LineEdit_SlaveNamePtr()->setPlaceholderText("Slave Name");

    return true;
}

bool Form_SlaveChoose::Destroy_cores()
{
    return true;
}

QComboBox *Form_SlaveChoose::get_ComboBox_SlaveIndexPtr()
{
    return ui->comboBox_slaveChoose;
}

QLineEdit *Form_SlaveChoose::get_LineEdit_SlaveNamePtr()
{
   return ui->lineEdit_slaveName;
}
