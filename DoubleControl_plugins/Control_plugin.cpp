#include "Control_plugin.h"

#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <QFileDialog>
#include <QKeyEvent>
#include <QSettings>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8") ///< 最好加上这句，否则乱码
#endif

//用于解析G代码
#define PROGRESSMINLINES 10000 //G代码文件的最大行数
#define PROGRESSSTEP     1000

Control_plugin::Control_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("Double Control");
    QPixmap pixmap(":/Controls/Resource/Control/App_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

Control_plugin::~Control_plugin(){
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

bool Control_plugin::Init_Object()
{
    user_form_controlTab = new Form_ControlTab();
    m_motorApp_callback = new My_MotorApp_Callback();

    m_messageObj = new EtherCAT_Message;

    set_UIWidgetPtr(user_form_controlTab);
    set_CallbackPtr(m_motorApp_callback);
    set_MessageObj(m_messageObj);//如果需要响应消息

    return true;
}

EtherCAT_UserApp *Control_plugin::get_NewAppPtr(QObject *parent)
{
    return new Control_plugin(parent);
}


bool Control_plugin::Init_Cores()
{
    m_settingPath = "./config_User.ini";
    Load_setting(m_settingPath);//加载设置
    m_motorApp_callback->Master_setSlaveCount(0);//赋初值
    m_SlaveChoose_indexLast = -1;

    controlTab_isTheta_display = user_form_controlTab->get_CheckBoxPtr(Form_ControlTab::check_isThetaDis_c)->checkState();
    controlTab_isLoadFileSafe = user_form_controlTab->get_CheckBoxPtr(Form_ControlTab::check_isLoadFileSafe_c)->checkState();

    connect(user_form_controlTab->get_ButtonGcode(Form_ControlTab::Gcode_openFile_b),SIGNAL(clicked(bool)),this,SLOT(Control_OpenGcode_clicked()));
    connect(user_form_controlTab->get_ButtonGcode(Form_ControlTab::Gcode_reloadFile_b),SIGNAL(clicked(bool)),this,SLOT(Control_ReloadGcode_clicked()));
    connect(user_form_controlTab->get_ButtonGcode(Form_ControlTab::Gcode_sendFile_b),SIGNAL(clicked(bool)),this,SLOT(Control_SendGcode_clicked()));
    connect(user_form_controlTab->get_CheckBoxPtr(Form_ControlTab::check_isThetaDis_c),SIGNAL(stateChanged(int)),this,SLOT(ControlTab_checkThetaDis_stateChange(int)));
    connect(user_form_controlTab->get_CheckBoxPtr(Form_ControlTab::check_isModeCalibrate_c),SIGNAL(stateChanged(int)),this,SLOT(ControlTab_checkModeCalibrate_stateChange(int)));
    connect(user_form_controlTab->get_CheckBoxPtr(Form_ControlTab::check_isLoadFileSafe_c),SIGNAL(stateChanged(int)),this,SLOT(ControlTab_checkLoadFileSafe_stateChange(int)));

    connect(user_form_controlTab->get_ComboBoxPtr(Form_ControlTab::comboBox_SlaveIndex_com),SIGNAL(currentIndexChanged(int)),SLOT(ControlTab_MasterIndex_currentIndexChanged(int)));

    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_X]));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setPlaceholderText(tr("X轴"));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Y]));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setPlaceholderText(tr("Y轴"));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Z]));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setPlaceholderText(tr("Z轴"));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_Speed_e)->setText(tr("1000"));
    user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_Speed_e)->setPlaceholderText(tr("速度"));

    m_GcodeSegment_Q = nullptr;
    gp_t = new GcodeParser();
//    m_GcodeSegment_Q = new QQueue<Gcode_segment>();
    GcodeSendThread = new QThread();
    m_motorApp_callback->moveToThread(GcodeSendThread);
    connect(GcodeSendThread,SIGNAL(started()),m_motorApp_callback,SLOT(GcodeSendThread_run()));//Gcode发送线程
    connect(m_motorApp_callback,SIGNAL(Gcode_lineChange(int)),this,SLOT(MotorCallback_GcodeLineChange(int)));//实现Gcode滚动效果

    connect(user_form_controlTab,SIGNAL(Jog_ButtonDown(int)),this,SLOT(ControlTab_jog_clicked(int)));//jog按钮组响应
    connect(m_motorApp_callback,SIGNAL(Gcode_PositionChange(QVector3D)),this,SLOT(MotorCallback_GcodePositionChange(QVector3D)));//设置显示的按钮
    connect(m_motorApp_callback,SIGNAL(Gcode_ThetaChange(QVector3D)),this,SLOT(MotorCallback_GcodeThetaChange(QVector3D)));//设置显示的按钮

    connect(m_motorApp_callback,SIGNAL(Master_QuitSignal(bool)),this,SLOT(MotorCallback_MasterQuit_sig(bool)));
    connect(m_motorApp_callback,SIGNAL(Master_ScanSignal()),this,SLOT(MotorCallback_MasterScan_sig()));
    connect(user_form_controlTab,SIGNAL(Key_EventSignal(QKeyEvent*)),this,SLOT(ControlTab_keyPressEvent(QKeyEvent*)));

    connect(m_motorApp_callback,SIGNAL(Control_BottomMessage_change(QString)),this,SLOT(MotorCallback_BottomMsgChange_sig(QString)));

    if(m_motorApp_callback->Master_getSlaveChoose_AppAllow() == false){
         user_form_controlTab->get_ComboBoxPtr(Form_ControlTab::comboBox_SlaveIndex_com)->setEnabled(false);
    }

    return true;
}

bool Control_plugin::Destroy_Cores()
{
   Save_setting(m_settingPath);//保存设置
   return true;
}

/*********************** Operation *************************/
void Control_plugin::Set_StatusMessage(QString message, int interval)
{
    _EC_message->Set_StatusMessage(message,interval);//发出自定义信号
}

void Control_plugin::Set_BottomMessage(QString message)
{
    _EC_message->Set_BottomMessage(message);//发出自定义信号
}

void Control_plugin::Set_MasterStop()
{
    _EC_message->Set_MasterStop();//发出自定义信号
}

int Control_plugin::Load_setting(const QString &path){

//    QFile file("./config.ini");
    QFile file(path);
    if(file.exists()){
        QSettings setting(path,QSettings::IniFormat);//读配置文件

//        QString str_3=setting.value("Login/account").toString();
//        qDebug() << str_3;
        QString setting_GcodePath = setting.value("Control_Plugin/GcodePath").toString();
//        QString setting_pluginDir =  setting.value("Path/PluginPath").toString();
        QDir dir;
        dir= QDir(setting_GcodePath);
        if(dir.exists()){
            m_GcodePath = setting_GcodePath;
        }
        else{
            QMessageBox::warning(get_UIWidgetPtr(),tr("Path Error!"),"GcodePath is Invalid,loading default path..");
        }

//        m_GcodePath = setting.value("Path/GcodePath").toString();
//        m_pluginDir = setting.value("Path/PluginPath").toString();

//        QString master_adapterName = setting.value("EtherCAT/Adapter_Name").toString();
//        QString master_adapterDesc = setting.value("EtherCAT/Adapter_Desc").toString();
//        user_form_generalTab->setMaster_adapterName(master_adapterName);
//        user_form_generalTab->setMaster_adapterDesc(master_adapterDesc);
//        //bind to master
//        user_form_generalTab->master->m_adapterDescSelect = master_adapterDesc;
//        user_form_generalTab->master->m_adapterNameSelect = master_adapterName;
//        qDebug() << m_GcodePath<<m_pluginDir;
    }
    else{
        m_GcodePath = "./";
//        qDebug() << "Load default setting!";
        Set_StatusMessage(tr("User:Load default setting!"),3000);
    }

    return 0;
}

int Control_plugin::Save_setting(const QString &path){

   QSettings setting(path,QSettings::IniFormat);//读配置文件

   setting.beginGroup(tr("Control_Plugin"));
   setting.setValue("GcodePath",m_GcodePath);//设置key和value，也就是参数和值
//   setting.setValue("PluginPath",m_pluginDir);
//   setting.setValue("remeber",true);
   setting.endGroup();//节点结束

    return 0;
}

int Control_plugin::Gcode_load(QString &fileName){
    if(!fileName.isEmpty()){
        //m_pluginDir = dir;
  //        qDebug() << m_pluginDir;
//        qDebug() << dir;
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(get_UIWidgetPtr(), tr("Control_plugin"), tr("Can't open file:\n") + fileName);
            return -1;
        }




//        // Set filename
//        m_programFileName = fileName;

        // Prepare text stream
        QTextStream textStream(&file);

        // Read lines
        QList<QString> data;
        while (!textStream.atEnd()) data.append(textStream.readLine());

//        qDebug() << data;

        //读取信息后，进行G代码的解析
        // Prepare parser
//        GcodeParser gp;
//        GcodeParser *gp_t = new GcodeParser();

        QProgressDialog progress(tr("Opening file..."), tr("Abort"), 0, data.count(), get_UIWidgetPtr());
        progress.setWindowModality(Qt::WindowModal);
        progress.setFixedSize(progress.sizeHint());
        if (data.count() > PROGRESSMINLINES) {//如果行数较大，就进行拆分
            progress.show();
            progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
        }

        QString command;
        QString stripped;
        QString trimmed;
        QList<QString> args;

//        GCodeItem item;
        gp_t->clearQueue();
        user_form_controlTab->get_TableGcode()->clearContents();
        int line_num = 0;
        QTableWidgetItem *tableItem;
        bool isCommentLine = false;
        while (!data.isEmpty())
        {
            command = data.takeFirst();

            // Trim command
            trimmed = command.trimmed();//移除字符串两端的空白字符

            if (!trimmed.isEmpty()) {
                // Split command
                stripped = GcodePreprocessorUtils::removeComment(command);
                if(stripped.isEmpty()){
                    isCommentLine = true;
                }
                else{
                    isCommentLine = false;
                }
//                qDebug() << stripped;
                args = GcodePreprocessorUtils::splitCommand(stripped);

//                gp.addCommand(args);//里面包含了handle
                gp_t->addCommand(args);//处理命令
//                m_GcodeSegment_Q = gp_t->getGodeQueue();
                if(isCommentLine){//空的表示是注释行
                    Gcode_segment segment;//插入M代码，让行对应
//                    segment.line = gp_t->getGodeQueue()->size();
                    segment.line =gp_t->getGodeQueue()->isEmpty()? 0:gp_t->getGodeQueue()->last().line+1;//这样第一行也可以有注释
                    segment.data_xyz = QVector3D(0,0,0);//注释的M代码位置无效
                    segment.Mcode = Gcode_segment::COMMENT_CODE;
                    gp_t->getGodeQueue()->enqueue(segment);

                }
//                while(!gp_t->getGodeQueue()->empty())
//                qDebug() << gp_t->getGodeQueue()->dequeue().data_xyz;
                  //添加到table中
                user_form_controlTab->get_TableGcode()->setRowCount(1+line_num);
                tableItem = new QTableWidgetItem(QString::number(line_num));
                tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                user_form_controlTab->get_TableGcode()->setItem(line_num,0,tableItem);

                tableItem = new QTableWidgetItem(trimmed);
                tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                user_form_controlTab->get_TableGcode()->setItem(line_num,1,tableItem);

                tableItem = new QTableWidgetItem(tr("Ready"));
                tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                user_form_controlTab->get_TableGcode()->setItem(line_num++,2,tableItem);

//                item.command = trimmed;
//                item.state = GCodeItem::InQueue;
//                item.line = gp.getCommandNumber();
//                item.args = args;

//                m_programModel.data().append(item);
            }

            if (progress.isVisible() && (data.count() % PROGRESSSTEP == 0)) {
                progress.setValue(progress.maximum() - data.count());
                qApp->processEvents();
                if (progress.wasCanceled()) break;
            }
        }
        progress.close();

        m_GcodeSegment_Q = gp_t->getGodeQueue();//传递指针
        //加一行，表示end
        user_form_controlTab->get_TableGcode()->setRowCount(1+line_num);
        tableItem = new QTableWidgetItem(QString::number(line_num));
        tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        user_form_controlTab->get_TableGcode()->setItem(line_num,0,tableItem);

        tableItem = new QTableWidgetItem(tr("End"));
        tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        user_form_controlTab->get_TableGcode()->setItem(line_num,1,tableItem);

        tableItem = new QTableWidgetItem(tr("Ready"));
        tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        user_form_controlTab->get_TableGcode()->setItem(line_num++,2,tableItem);

        Gcode_segment segment;//插入M代码，让行对应
//        segment.line = gp_t->getGodeQueue()->size();
        segment.line = gp_t->getGodeQueue()->last().line+1;
        segment.data_xyz = QVector3D(0,0,0);//M代码位置无效
        segment.Mcode = Gcode_segment::EndParse_CODE;
        gp_t->getGodeQueue()->enqueue(segment);


////实现滚动效果
//m_table_Gcode->selectRow(250);
//m_table_Gcode->scrollTo(m_table_Gcode->model()->index(250,1));

//        // Load lines
//        loadFile(data);
//        while(!gp_t->getGodeQueue()->empty()){
//            Gcode_segment xx = gp_t->getGodeQueue()->dequeue();
//            qDebug() << "xx:"<<xx.data_xyz << xx.line;
//        }

//        if(gp_t->getGodeQueue()->empty()){
//            qDebug() << "mainview_center Q empty!";
//        }
//        else{
//            qDebug() << "mainview_center Q No empty!";
//        }
    }

    return 0;
}

/************  Slots *******************/

void Control_plugin::Control_OpenGcode_clicked(){
//    if(user_form_generalTab->master->Master_getSlaveCount()>0){
//       Master_stop();//防止界面卡死
//       StatusMessage_change(tr("Stop Master..."),3000);
//    }
//    StatusMessage_change(tr("Stop Master..."),3000);
    if(m_motorApp_callback->Master_getSlaveCount() > 0){
        Set_MasterStop();
        Set_StatusMessage(tr("Stop Master..."),3000);
    }

    //WARNING:看看是否需要延时

    QString fileName = "";
    fileName  = QFileDialog::getOpenFileName(get_UIWidgetPtr(), tr("Open Gcode"), m_GcodePath,
                               tr("G-Code files (*.nc *.ncc *.ngc *.tap *.txt);;All files (*.*)"));//如果没有选择路径就会为空
    if(!fileName.isEmpty()){
        m_GcodePath_full = fileName;
        //分离path和fileName
        QFileInfo fileInfo = QFileInfo(fileName);
//        qDebug() << fileInfo.fileName() <<fileInfo.absolutePath();
        m_GcodePath = fileInfo.absolutePath();

        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Gcode_filePath_e)->setText(fileName);
        Set_BottomMessage(tr("OPen Gcode file OK!"));
    }

    Gcode_load(m_GcodePath_full);
}

void Control_plugin::Control_ReloadGcode_clicked(){
//    if(user_form_generalTab->master->Master_getSlaveCount()>0){
//       Master_stop();//防止界面卡死
//       StatusMessage_change(tr("Stop Master..."),3000);
//    }

    if(m_motorApp_callback->Master_getSlaveCount() > 0){
        Set_MasterStop();
        Set_StatusMessage(tr("Stop Master..."),3000);
    }

    int ret = Gcode_load(m_GcodePath_full);
    if(ret ==0){
        Set_BottomMessage(tr("Reload Gcode file OK!"));
    }
}

void Control_plugin::Control_SendGcode_clicked(){
   GcodeSendThread->start();//开始解析G代码线程
   m_motorApp_callback->set_RenewST_Ready(true);
}

void Control_plugin::MotorCallback_GcodeLineChange(int line){
//    qDebug() << line;
    //实现滚动效果
    user_form_controlTab->get_TableGcode()->selectRow(line);
    user_form_controlTab->get_TableGcode()->scrollTo(user_form_controlTab->get_TableGcode()->model()->index(line,QTableView::EnsureVisible));
    //改变状态
    int current_row = user_form_controlTab->get_TableGcode()->currentRow();
    QTableWidgetItem *tableItem = new QTableWidgetItem(tr("OK"));
    tableItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    user_form_controlTab->get_TableGcode()->setItem(current_row,2,tableItem);

}

void Control_plugin::MotorCallback_GcodePositionChange(QVector3D pos){
    if(!controlTab_isTheta_display){
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_X,pos.x());
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_Y,pos.y());
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_Z,pos.z());
    }
}

void Control_plugin::MotorCallback_GcodeThetaChange(QVector3D theta){
    if(controlTab_isTheta_display){
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_X,theta.x());
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_Y,theta.y());
        user_form_controlTab->set_LCDnumber_Display(Form_ControlTab::Axis_Z,theta.z());
    }
}

void Control_plugin::ControlTab_checkThetaDis_stateChange(int arg){
    controlTab_isTheta_display = arg;
}

void Control_plugin::ControlTab_checkModeCalibrate_stateChange(int arg)
{
  m_motorApp_callback->set_Mode_Calibrate(arg);
}

void Control_plugin::ControlTab_checkLoadFileSafe_stateChange(int arg)
{
  controlTab_isLoadFileSafe = arg;
}

void Control_plugin::ControlTab_jog_clicked(int button){
    QVector3D coor_temp;

    switch(button){
       case Form_ControlTab::Jog_AxisX_P_b://左
        coor_temp.setY(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(coor_temp.y()));

        break;
        case Form_ControlTab::Jog_AxisX_N_b://右
        coor_temp.setY(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(coor_temp.y()));
         break;
        case Form_ControlTab::Jog_AxisY_P_b://前
        coor_temp.setX(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(coor_temp.x()));
         break;
        case Form_ControlTab::Jog_AxisY_N_b://后
        coor_temp.setX(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(coor_temp.x()));
         break;
        case Form_ControlTab::Jog_AxisZ_P_b://上
        coor_temp.setZ(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(coor_temp.z()));
         break;
        case Form_ControlTab::Jog_AxisZ_N_b://下
        coor_temp.setZ(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(coor_temp.z()));
         break;
        case Form_ControlTab::Jog_Home_b:
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_X]));
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Y]));
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Z]));
        break;
        case Form_ControlTab::Jog_Halt_b:
        memset(m_motorApp_callback->loop_count,0,sizeof(m_motorApp_callback->loop_count));//stop
        m_motorApp_callback->Control_QueueClear();
        m_motorApp_callback->m_sys_reset = true;
        return;

        break;
        default:
            break;
    }

    m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_X] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat();//300;
    m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Y] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat();//200;
    m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Z] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat();//100;
    Gcode_segment segment;
    segment.speed_step = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_Speed_e)->text().toFloat();//设置步进电机的转速
    m_motorApp_callback->Planner_BufferLine(m_motorApp_callback->m_ARM_Motion_test->arm,segment,0);
    m_motorApp_callback->m_sys_reset = false;
}

void Control_plugin::MotorCallback_MasterScan_sig()
{
    if(m_SlaveChoose_indexLast == m_motorApp_callback->Master_getSlaveCount()){
        return;
    }
//    qDebug()<< "MotorCallback_MasterScan_sig";

    //NOTE:clear会调用设置item_index=-1的信号
    user_form_controlTab->m_isSlaveChoose_init = true;
    user_form_controlTab->get_ComboBoxPtr(Form_ControlTab::comboBox_SlaveIndex_com)->clear();
    if(m_motorApp_callback->Master_getSlaveCount() > 0){
        for(int i=0;i<m_motorApp_callback->Master_getSlaveCount();i++){
            user_form_controlTab->m_isSlaveChoose_init = true;
            user_form_controlTab->get_ComboBoxPtr(Form_ControlTab::comboBox_SlaveIndex_com)->addItem(QString::number(i));
        }
        int slaveChoose_tmp = m_motorApp_callback->Master_getSlaveChooseIndex();
        if( slaveChoose_tmp> 0 && slaveChoose_tmp < m_motorApp_callback->Master_getSlaveCount()){//如果已经做了选择
            user_form_controlTab->m_isSlaveChoose_init = true;
            user_form_controlTab->get_ComboBoxPtr(Form_ControlTab::comboBox_SlaveIndex_com)->setCurrentIndex(slaveChoose_tmp);
        }

        user_form_controlTab->m_isSlaveChoose_init = false;
    }

    m_SlaveChoose_indexLast = m_motorApp_callback->Master_getSlaveCount();

}

void Control_plugin::ControlTab_MasterIndex_currentIndexChanged(int index)
{
    if(user_form_controlTab->m_isSlaveChoose_init == false){
        m_motorApp_callback->Master_setSlaveChooseIndex(index);
    }
//    qDebug() << "ControlTab_MasterIndex_currentIndexChanged" <<  index;
}

void Control_plugin::MotorCallback_MasterQuit_sig(bool isQuit){
    if(isQuit){
        GcodeSendThread->quit();
        GcodeSendThread->wait();
        user_form_controlTab->get_GroupPtr(Form_ControlTab::Groups_jog_g)->setEnabled(false);
        user_form_controlTab->get_ButtonGcode(Form_ControlTab::Gcode_sendFile_b)->setEnabled(false);
        m_motorApp_callback->Gcode_ReleaseAddress();

        user_form_controlTab->get_GroupPtr(Form_ControlTab::Groups_Master_g)->setEnabled(true);//从站选择使能
    }
    else{
        user_form_controlTab->get_GroupPtr(Form_ControlTab::Groups_jog_g)->setEnabled(true);
        user_form_controlTab->get_ButtonGcode(Form_ControlTab::Gcode_sendFile_b)->setEnabled(true);
        m_motorApp_callback->Gcode_setAddress(m_GcodeSegment_Q);

        user_form_controlTab->get_GroupPtr(Form_ControlTab::Groups_Master_g)->setEnabled(false);//从站选择失能

        get_UIWidgetPtr()->setFocus();//如果用键盘测试的话，需要设置焦点
    }

}

void Control_plugin::MotorCallback_BottomMsgChange_sig(QString message)
{
    Set_BottomMessage(message);
}

void Control_plugin::ControlTab_keyPressEvent(QKeyEvent *event){
    if(m_motorApp_callback->is_InputPtr_Release()){
        return;
    }

    QVector3D coor_temp;
    Gcode_segment segment;

    switch(event->key()){
    case Qt::Key_Space:
        memset(m_motorApp_callback->loop_count,0,sizeof(m_motorApp_callback->loop_count));//stop
        m_motorApp_callback->Control_QueueClear();
        m_motorApp_callback->m_sys_reset = true;
        break;
    case Qt::Key_Control:
        m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_X] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat();//300;
        m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Y] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat();//200;
        m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Z] = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat();//100;
        segment.speed_step = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_Speed_e)->text().toFloat();//设置步进电机的转速
        m_motorApp_callback->Planner_BufferLine(m_motorApp_callback->m_ARM_Motion_test->arm,segment,0);
        m_motorApp_callback->m_sys_reset = false;
//        m_motorApp_callback->start();
        break;
     case Qt::Key_R:
            m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_X] = m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_X];
            m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Y] = m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Y];
            m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Z] = m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Z];
//            m_motorApp_callback->start();
            segment.speed_step = user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_Speed_e)->text().toFloat();//设置步进电机的转速
            m_motorApp_callback->Planner_BufferLine(m_motorApp_callback->m_ARM_Motion_test->arm,segment,0);
            m_motorApp_callback->m_sys_reset = false;

        break;
     case Qt::Key_S:
//        if(m_motorApp_callback->m_sys_reset){
//            while(!m_GcodeSegment_Q->empty()){
//                QVector3D data = m_GcodeSegment_Q->dequeue().data_xyz;
//                data_last = data;
//                qDebug() << data;
//                m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_X] = data.x();
//                m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Y] = data.y();
//                m_motorApp_callback->m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Z] = data.z();
//                m_motorApp_callback->Planner_BufferLine(m_motorApp_callback->m_ARM_Motion_test->arm,0);
//            }
//            m_motorApp_callback->m_sys_reset = false;
//            m_motorApp_callback->start();
//            m_motorApp_callback->loop_count[0] = 1;
//        }
            GcodeSendThread->start();
        break;
   case Qt::Key_Q:
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_X]));
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Y]));
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(m_motorApp_callback->m_PositionInit->arm[Dobot_Motion::AXIS_Z]));
        break;
    case Qt::Key_A:
        m_motorApp_callback->set_RenewST_Ready(true);
        break;
    case Qt::Key_Left://左
        coor_temp.setY(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(coor_temp.y()));
        break;
    case Qt::Key_Right://右
        coor_temp.setY(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosY_e)->setText(QString("%1").arg(coor_temp.y()));

        break;
    case Qt::Key_Up://前
        coor_temp.setX(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(coor_temp.x()));

        break;
    case Qt::Key_Down://后
        coor_temp.setX(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosX_e)->setText(QString("%1").arg(coor_temp.x()));

        break;
    case Qt::Key_Z://上
        coor_temp.setZ(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat()+user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(coor_temp.z()));

        break;
    case Qt::Key_X://下
        coor_temp.setZ(user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->text().toFloat()-user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_step_e)->text().toInt());
        user_form_controlTab->get_LineEditGcode(Form_ControlTab::Jog_PosZ_e)->setText(QString("%1").arg(coor_temp.z()));

        break;
    default:
        qDebug() << event->key();
        break;
    }
}

/************  Slots End ***************/



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(User_plugins, UserApp_plugin)
#endif // QT_VERSION < 0x050000
