#include "Form_ControlTab.h"
#include "ui_Form_ControlTab.h"

#include <QMessageBox>
#include <QDebug>

Form_ControlTab::Form_ControlTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_ControlTab)
{
    ui->setupUi(this);

    Init_Cores();

//    ui->lcdNumber_Axis_X->setDecMode();
//    ui->lcdNumber_Axis_X->setDigitCount(10);
//    ui->lcdNumber_Axis_X->display(1000.9987);
    m_isSlaveChoose_init = false;
}

Form_ControlTab::~Form_ControlTab()
{
    Destroy_Cores();

    delete ui;
}

void Form_ControlTab::Init_Cores(){
    get_LineEditGcode(Gcode_filePath_e)->setPlaceholderText(tr("File Path"));
    get_LineEditGcode(Jog_step_e)->setPlaceholderText(tr("Step"));
    get_LineEditGcode(Gcode_command_e)->setPlaceholderText(tr("Command"));

    get_LineEditGcode(Jog_step_e)->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    get_LineEditGcode(Jog_step_e)->setText(tr("1"));

    get_GroupPtr(Groups_jog_g)->setEnabled(false);//失能jog模式
    get_ButtonGcode(Form_ControlTab::Gcode_sendFile_b)->setEnabled(false);

    get_TableGcode()->setColumnCount(3);
    get_TableGcode()->setHorizontalHeaderLabels(QStringList()<<tr("#")<<tr("Gcode")<<tr("State"));
    get_TableGcode()->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置等宽
    get_TableGcode()->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);//对第1列单独设置根据内容设置宽度
    get_TableGcode()->verticalHeader()->setVisible(false); //设置垂直头不可见

    ui->lcdNumber_Axis_X->setDecMode();
    ui->lcdNumber_Axis_X->setDigitCount(8);//设置显示位数
    ui->lcdNumber_Axis_Y->setDecMode();
    ui->lcdNumber_Axis_Y->setDigitCount(8);
    ui->lcdNumber_Axis_Z->setDecMode();
    ui->lcdNumber_Axis_Z->setDigitCount(8);

}

void Form_ControlTab::Destroy_Cores(){

}

QTableWidget *Form_ControlTab::get_TableGcode()
{
    return ui->tableWidget_gcode;
}

QPushButton *Form_ControlTab::get_ButtonGcode(gcode_button_choose choose)
{
    switch(choose){
    case Gcode_openFile_b:
        return ui->pushButton_gcodeFileOpen;
        break;
    case Gcode_reloadFile_b:
        return ui->pushButton_gcodeFileReload;
        break;
    case Gcode_sendFile_b:
        return ui->pushButton_gcodeFileSend;
        break;
    case Gcode_reset_b:
        return ui->pushButton_gcodeFileReset;
        break;
    case Gcode_commandSend_b:
        return ui->pushButton_commendSend;
        break;
    default:
        QMessageBox::critical(this,tr("Form_ControlTab Button"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QLineEdit *Form_ControlTab::get_LineEditGcode(gcode_lineEdit_choose choose)
{
    switch(choose){
    case Gcode_filePath_e:
        return ui->lineEdit_gcodeFilePath;
        break;
    case Jog_step_e:
        return ui->lineEdit_jogStep;
        break;
    case Gcode_command_e:
        return ui->lineEdit_command;
        break;
    case Jog_Speed_e:
        return ui->lineEdit_jogSpeed;
        break;
    case Jog_PosX_e:
        return ui->lineEdit_posX;
        break;
    case Jog_PosY_e:
        return ui->lineEdit_PosY;
        break;
    case Jog_PosZ_e:
        return ui->lineEdit_PosZ;
        break;
    default:
        QMessageBox::critical(this,tr("Form_ControlTab Edit"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QGroupBox *Form_ControlTab::get_GroupPtr(group_choose choose)
{
    switch(choose){
    case Groups_parameter_g:
        return ui->groupBox_Parameter;
        break;
    case Groups_jog_g:
        return ui->groupBox_Jog;
        break;
    case Groups_Master_g:
        return ui->groupBox_Master;
        break;
    default:
        QMessageBox::critical(this,tr("Form_ControlTab Group"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QCheckBox *Form_ControlTab::get_CheckBoxPtr(checkBox_choose choose)
{
    switch(choose){
    case check_isThetaDis_c:
        return ui->checkBox_Is_Theta_Display;
        break;
    case check_isModeCalibrate_c://校准模式
        return ui->checkBox_Is_CalibrateMode;
        break;
    case check_isLoadFileSafe_c:
        return ui->checkBox_Is_LoadFileSafe;
        break;
    default:
        QMessageBox::critical(this,tr("Form_ControlTab CheckBox"),tr("Return NULL!"));
        return NULL;
        break;
    }

}

QComboBox *Form_ControlTab::get_ComboBoxPtr(comboBox_choose choose)
{
    switch(choose){
    case comboBox_SlaveIndex_com:
        return ui->comboBox_SlaveIndex;
        break;
    default:
        QMessageBox::critical(this,tr("Form_ControlTab ComboBox"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

void Form_ControlTab::set_LCDnumber_Display(Axis_num num, double data)
{
    switch(num){
    case Axis_X:
        ui->lcdNumber_Axis_X->display(data);
        break;
    case Axis_Y:
        ui->lcdNumber_Axis_Y->display(data);
        break;
    case Axis_Z:
        ui->lcdNumber_Axis_Z->display(data);
        break;
    default:
        break;
    }
}


void Form_ControlTab::on_toolButton_jogX_positive_clicked()
{
    emit Jog_ButtonDown(Jog_AxisX_P_b);
}

void Form_ControlTab::on_toolButton_jogX_negtive_clicked()
{
   emit Jog_ButtonDown(Jog_AxisX_N_b);
}

void Form_ControlTab::on_toolButton_jogY_positive_clicked()
{
    emit Jog_ButtonDown(Jog_AxisY_P_b);
}

void Form_ControlTab::on_toolButton_jogY_negtive_clicked()
{
   emit Jog_ButtonDown(Jog_AxisY_N_b);
}

void Form_ControlTab::on_toolButton_jogZ_positive_clicked()
{
   emit Jog_ButtonDown(Jog_AxisZ_P_b);
}

void Form_ControlTab::on_toolButton_jogZnegtive_clicked()
{
  emit Jog_ButtonDown(Jog_AxisZ_N_b);
}

void Form_ControlTab::on_toolButton_jogHome_clicked()
{
    emit Jog_ButtonDown(Jog_Home_b);
}

void Form_ControlTab::on_toolButton_jogHalt_clicked()
{
    emit Jog_ButtonDown(Jog_Halt_b);
}

void Form_ControlTab::keyPressEvent(QKeyEvent *event){
    emit Key_EventSignal(event);
}
