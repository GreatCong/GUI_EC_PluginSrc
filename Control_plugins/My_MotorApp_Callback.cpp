#include "My_MotorApp_Callback.h"
#include <Qdebug>

///
/// \brief 构造初始化
/// \param parent
///
My_MotorApp_Callback::My_MotorApp_Callback(QObject *parent): QObject(parent),Ethercat_Callback()
{
    this->Master_ReleaseAddress();//初始化指针为NULL
    memset(loop_count,0,sizeof(loop_count));
    m_sys_reset = false;

    m_ARM_Motion_test = new ARM_Struct(CNC_Motion::AXIS_N);
    m_PositionInit = new ARM_Struct(CNC_Motion::AXIS_N);//笛卡尔坐标系为(0,0,0)的角度

    Arm_motion_reset();
    isRun = true;

    m_robot_index = 0;//机器人的编号
    m_robot_LastIndex = 0;
    m_isMode_calibrate = false;
}

///
/// \brief 析构函数
///
My_MotorApp_Callback::~My_MotorApp_Callback()
{
    this->Master_ReleaseAddress();
    memset(loop_count,0,sizeof(loop_count));
//    memset(&m_Stepper_control,0,sizeof(m_Stepper_control));
    m_sys_reset = false;
//    memset(&m_CNCMotion.m_sys_position,0,sizeof(m_CNCMotion.m_sys_position));
    isRun = false;
//    this->wait();
//    this->quit();//停止线程
    this->Gcode_ReleaseAddress();
}

///
/// \brief My_MotorApp_Callback::Master_AppLoop_callback
///
void My_MotorApp_Callback::Master_AppLoop_callback()
{
    //控制电平输出

//    if(m_Stepper_control.step_count == 0){//需要获取新的block
//        if(m_Stepper_block_Q->empty()){
//            return ;
//        }
//        else{
//            m_Stepper_control.exec_block = m_Stepper_block_Q->dequeue();
//            m_Stepper_control.step_count = m_Stepper_control.exec_block->step_event_count;//一共运行所有轴的最大步长
//            m_Stepper_control.counter[AXIS_X] = m_Stepper_control.counter[AXIS_Y] = m_Stepper_control.counter[AXIS_Z] = (m_Stepper_control.exec_block->step_event_count >> 1);

//            m_Stepper_control.Axis_steps[AXIS_X] = m_Stepper_control.exec_block->Axis_steps[AXIS_X];
//            m_Stepper_control.Axis_steps[AXIS_Y] = m_Stepper_control.exec_block->Axis_steps[AXIS_Y];
//            m_Stepper_control.Axis_steps[AXIS_Z] = m_Stepper_control.exec_block->Axis_steps[AXIS_Z];
//            *(input_ptr+step_setting) = 1;
//            *(input_ptr+step_fre) = 5000;//设置频率为1000Hz
//        }
//    }

//    m_Stepper_control.step_outbits = 0;//复位Step bits
//    //direction_bit通过预运行函数设定
//    m_Stepper_control.dir_outbits = m_Stepper_control.exec_block->direction_bits;//传递dir bits

//    //插补运动
//    m_Stepper_control.counter[AXIS_X] += m_Stepper_control.Axis_steps[AXIS_X];
//    if(m_Stepper_control.counter[AXIS_X] > m_Stepper_control.exec_block->step_event_count){
//        //设置方向
//        m_Stepper_control.step_outbits |= (1<<AXIS_X);
//        m_Stepper_control.counter[AXIS_X] -= m_Stepper_control.exec_block->step_event_count;
//        //根据方向更新位置信息
//        if(m_Stepper_control.exec_block->direction_bits & (1<<AXIS_X)) {
//            m_sys_position[AXIS_X]--;
//        }
//        else {
//            m_sys_position[AXIS_X]++;
//        }
//    }

//    m_Stepper_control.counter[AXIS_Y] += m_Stepper_control.Axis_steps[AXIS_Y];
//    if(m_Stepper_control.counter[AXIS_Y] > m_Stepper_control.exec_block->step_event_count){
//        //设置方向
//        m_Stepper_control.step_outbits |= (1<<AXIS_Y);
//        m_Stepper_control.counter[AXIS_Y] -= m_Stepper_control.exec_block->step_event_count;
//        //根据方向更新位置信息
//        if(m_Stepper_control.exec_block->direction_bits & (1<<AXIS_Y)) {
//            m_sys_position[AXIS_Y]--;
//        }
//        else {
//            m_sys_position[AXIS_Y]++;
//        }
//    }

//    m_Stepper_control.counter[AXIS_Z] += m_Stepper_control.Axis_steps[AXIS_Z];
//    if(m_Stepper_control.counter[AXIS_Z] > m_Stepper_control.exec_block->step_event_count){
//        //设置方向
//        m_Stepper_control.step_outbits |= (1<<AXIS_Z);
//        m_Stepper_control.counter[AXIS_Z] -= m_Stepper_control.exec_block->step_event_count;
//        //根据方向更新位置信息
//        if(m_Stepper_control.exec_block->direction_bits & (1<<AXIS_Z)) {
//            m_sys_position[AXIS_Z]--;
//        }
//        else {
//            m_sys_position[AXIS_Z]++;
//        }
//    }

//    //设置方向
//    *(input_ptr+motor_dir) = m_Stepper_control.dir_outbits;//dir
////        *(input_ptr+6) = 0;//dir4

//     //设置Step
//    *(input_MotorStep_ptr+motor1_step) = (m_Stepper_control.step_outbits & (1<<AXIS_X))>>AXIS_X;//step1
//    *(input_MotorStep_ptr+motor2_step) = (m_Stepper_control.step_outbits & (1<<AXIS_Y))>>AXIS_Y;//step2
//    *(input_MotorStep_ptr+motor3_step) = (m_Stepper_control.step_outbits & (1<<AXIS_Z))>>AXIS_Z;//step3

//    m_Stepper_control.step_count--;
////    qDebug() << m_Stepper_control.step_count;

//    if(m_sys_reset){//如果用户进行了复位
//       m_Stepper_control.step_count=0;
//    }

//    if(m_Stepper_control.step_count == 0){
//        delete m_Stepper_control.exec_block;
//        m_Stepper_control.exec_block = NULL;//释放内存
//        Motor_Reset();//防止Ethercat将循环的数值传递到从站
//         *(input_ptr+step_setting) = 0;
//    }
//    qDebug() << "input"<<*input_ptr << *(input_ptr+1);
//    qDebug() << "output"<<*output_ptr << *(output_ptr+1);
//    qDebug() << xxx;
//    *(input_ptr+step_setting) = 1;
//    *(input_ptr+step_fre) = 1000;
//    if(xx==0){
//        STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_AutoRun_start);
////        STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_stop);
//        *(input_ptr+motor_dir) = 1;
//        *(input_MotorStep_ptr+motor1_step) = 1000;
//        *(input_MotorStep_ptr+motor2_step) = 3000;
//        *(input_MotorStep_ptr+motor3_step) = 2000;
////        qDebug() << "step1";
//        xx = 1;
//    }

//    if(xx==1 && (*(output_ptr+error_MSG)==1)){
//        xx =2;
////        STEP_BIT_SetFalse(*(input_ptr+step_setting),(1<<step_AutoRun_start));
////        qDebug() << "step2";
//    }
//    if(xx==2 && (*(output_ptr+motor_MSG)==0)){
////        *(input_MotorStep_ptr+motor1_step) = 0;
////        *(input_MotorStep_ptr+motor2_step) = 0;
////        *(input_MotorStep_ptr+motor3_step) = 0;
//        uint16_t mask = (1<<AXIS_X)|(1<<AXIS_Y)|(1<<AXIS_Z);
//        STEP_BIT_Toggle(*(input_ptr+motor_dir),mask);
//        STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_AutoRun_stop);
//        STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_AutoRun_start);
//        xx=1;
////        qDebug() << "step3";
//    }
//    *(input_MotorStep_ptr+motor1_step) = 0xaabbcc01;
//    *(input_MotorStep_ptr+motor2_step) = 0xaabbcc02;
//    *(input_MotorStep_ptr+motor3_step) = 0xaabbcc03;
//    *(input_MotorStep_ptr+motor4_step) = 0xaabbcc04;
//    *(input_MotorStep_ptr+motor5_step) = 0xaabbcc05;
//    if(xx<100){
//        *(input_ptr+motor1_dir) = 0;
//         *(input_ptr+motor2_dir) = 0;
//         *(input_ptr+motor3_dir) = 1;
//    }
//    else if(xx<200){
//        *(input_ptr+motor1_dir) = 1;
//         *(input_ptr+motor2_dir) = 0;
//         *(input_ptr+motor3_dir) = 0;
//    }
//    else{
//        xx = 0;
//    }
//    xx++;
//    *(input_ptr+motor1_step) = 1;
//    *(input_ptr+motor2_step) = 0;
//    *(input_ptr+motor3_step) = 1;

    m_CNCMotion.m_Stepper_control->step_count = *(m_CNCMotion.get_Output_MotorCountPtr());
    if(m_sys_reset){//如果用户进行了复位
       m_CNCMotion.m_Stepper_control->step_count=0;
       STEP_BIT_SetTrue(*(m_CNCMotion.get_IutputPtr()+step_setting),1<<step_AutoRun_stop);//从站中stop判断在前
       Motor_Reset();
       *(m_CNCMotion.get_Iutput_MotorStepPtr()+AXIS_TOOL1) = 0;
    }   

    //BUG:如果步数比较小,有时候无法自动完成激励
//    if(m_RenewST_init && (*(output_ptr+error_MSG)==2)){//给一个循环运动的激励
//        m_RenewST_ready = true;
//        m_RenewST_init = false;
//    }
    if(m_CNCMotion.m_RenewST_init){//给一个循环运动的激励
        if((*(m_CNCMotion.get_OutputPtr()+motor_MSG)==2) || m_CNCMotion.m_McodeFlag){
            m_CNCMotion.m_RenewST_ready = true;
            m_CNCMotion.m_RenewST_init = false;
            m_CNCMotion.m_McodeFlag = false;
        }
    }


//    if( m_CNCMotion.m_Stepper_control->step_count== 0){//需要获取新的block
//        if(m_CNCMotion.m_Stepper_block_Q->empty()){
//            return ;
//        }
//        else{
//            m_CNCMotion.loopRun();
//        }
//    }
//    else{
//        Motor_Reset();
//     *(m_CNCMotion.get_IutputPtr()+step_setting) = 1;//只是使能，去掉自动运行的标志
//    }
    m_CNCMotion.loopRun();//根据新的block进行循环运动



//    qDebug() << m_Stepper_control.step_count << QString::number(*(input_ptr+step_setting),2);
//    qDebug() << "Dir:"<<*(output_ptr+motor_dir);
//    qDebug() <<"Step_count:"<<*(output_ptr+motor_MSG)<<"isAutoRun"<<*(output_ptr+error_MSG);
//    qDebug() << *(output_ptr);

}

///
/// \brief My_MotorApp_Callback::Master_AppStart_callback
///
void My_MotorApp_Callback::Master_AppStart_callback()
{
    //qDebug() << m_Master_addressList[0].inputs_offset << m_Master_addressList[0].outputs_offset;//inputs_offset=0x1a,outputs_offset = 0x00;
//    foreach (Master_Address_t addrs, m_Master_addressList) {
//        qDebug() << "input:0x" +QString::number(addrs.inputs_offset,16) << "output:0x" + QString::number(addrs.outputs_offset,16);
//    }

//    output_ptr = (int16_t*)(m_Master_addressBase+0x1a);
//    input_ptr = (uint16_t*)(m_Master_addressBase+0x00);
//    input_MotorStep_ptr = (uint32_t*)(m_Master_addressBase+0x06);

    const int16_t* output_ptr = NULL;
    uint16_t* input_ptr = NULL;
    uint32_t* input_MotorStep_ptr = NULL;
    const uint32_t* output_MotorCount_ptr = NULL;

    //更改根据从站编号获取偏移地址
    output_ptr = (int16_t*)(m_Master_addressBase+m_Master_addressList[this->Master_getSlaveChooseIndex()].inputs_offset + 0x04);
    input_ptr = (uint16_t*)(m_Master_addressBase+m_Master_addressList[this->Master_getSlaveChooseIndex()].outputs_offset);
    input_MotorStep_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[this->Master_getSlaveChooseIndex()].outputs_offset+0x08);
    output_MotorCount_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[this->Master_getSlaveChooseIndex()].inputs_offset);

    //设置dobot的IO地址
    m_CNCMotion.set_OutputPtr(output_ptr);
    m_CNCMotion.set_IutputPtr(input_ptr);
    m_CNCMotion.set_Iutput_MotorStepPtr(input_MotorStep_ptr);
    m_CNCMotion.set_Output_MotorCountPtr(output_MotorCount_ptr);

    //设定一个初始值,否则Ethercat会将循环的数值传递到从站
   Motor_Reset();
   *(m_CNCMotion.get_Iutput_MotorStepPtr()+AXIS_TOOL1) = 0;
//   xx=0;
   isRun = true;
//   if(m_GcodeSegment_Q->empty()){
//       qDebug() << "My_MotorApp_Callback Q empty!";
//   }
//   else{
//       qDebug() << "My_MotorApp_Callback Q No empty!";
//   }

   emit Master_QuitSignal(false);//线程停止

}

///
/// \brief My_MotorApp_Callback::Master_AppStop_callback
///
void My_MotorApp_Callback::Master_AppStop_callback()
{
//  Motor_Reset();//加上会报错
  this->Master_ReleaseAddress();//释放指针
  isRun = false;
//  this->wait();
//  this->quit();//停止线程
  Control_QueueClear();
  m_CNCMotion.m_Stepper_control->step_count = 0;
//  memset(&m_sys_position,0,sizeof(m_sys_position));
  Arm_motion_reset();//不能全部置0

  emit Master_QuitSignal(true);//线程停止
}

///
/// \brief My_MotorApp_Callback::Master_AppScan_callback
///
void My_MotorApp_Callback::Master_AppScan_callback(){
    emit Master_ScanSignal();
}

///
/// \brief My_MotorApp_Callback::Master_setAdressBase
/// \param address
/// \return
///
int My_MotorApp_Callback::Master_setAdressBase(char *address)
{
    m_Master_addressBase = address;

    return 0;
}

///
/// \brief My_MotorApp_Callback::Master_ReleaseAddress
///
void My_MotorApp_Callback::Master_ReleaseAddress()
{
    m_Master_addressBase = NULL;
    m_CNCMotion.release_IO_ptr();
}

///
/// \brief My_MotorApp_Callback::Gcode_setAddress
/// \param q_addr
///
void My_MotorApp_Callback::Gcode_setAddress(QQueue<Gcode_segment> *q_addr)
{
    m_GcodeSegment_Q = q_addr;
}

///
/// \brief My_MotorApp_Callback::Gcode_ReleaseAddress
///
void My_MotorApp_Callback::Gcode_ReleaseAddress()
{
    m_GcodeSegment_Q = nullptr;
}

///
/// \brief My_MotorApp_Callback::Motor_Reset
///
void My_MotorApp_Callback::Motor_Reset(){
//    //设置方向
//    *(input_ptr+motor_dir) = 0;//dir

//     //设置Step
//    *(input_MotorStep_ptr+motor1_step) = 0;//step1
//    *(input_MotorStep_ptr+motor2_step) = 0;//step2
//    *(input_MotorStep_ptr+motor3_step) = 0;//step3
    m_CNCMotion.Motor_Reset();
}

///
/// \brief My_MotorApp_Callback::Planner_BufferLine
/// \param target
/// \param userData
/// \param robot_index
/// \return
///
int My_MotorApp_Callback::Planner_BufferLine(float *target, const Gcode_segment &userData, int robot_index)
{
    //Q_UNUSED(userData);//后面再添加
    int res = Planner_OK;
    ARM_SendState *sendState = NULL;
    switch(robot_index){
    case 0:
    case 1:
         sendState= new ARM_SendState(CNC_Motion::AXIS_N);
        res = m_CNCMotion.planner_BufferLine(target,userData,sendState);

        if(sendState->sendPositionOK){
            QVector3D position_send = QVector3D(sendState->position_send->arm[0],sendState->position_send->arm[1],sendState->position_send->arm[2]);
            emit Gcode_PositionChange(position_send);//发送正解
        }

        if(sendState->sendThetaOK){
          QVector3D theta_send = QVector3D(sendState->theta_send->arm[0],sendState->theta_send->arm[1],sendState->theta_send->arm[2]);
          emit Gcode_ThetaChange(theta_send);//发送反解
        }
        break;
    case 2:
//        sendState= new ARM_SendState(CNC_Motion::AXIS_N);
//        res = m_CNCMotion1.planner_BufferLine(target,userData,sendState);
        break;
    case 3:
        break;
    default:
        break;
    }

    return res;
}

///
/// \brief My_MotorApp_Callback::Arm_motion_reset
///
void My_MotorApp_Callback::Arm_motion_reset(){
    m_CNCMotion.Arm_motion_reset(false);
    *m_PositionInit = m_CNCMotion.get_PositionInit();
}

///
/// \brief My_MotorApp_Callback::Control_QueueClear
///
void My_MotorApp_Callback::Control_QueueClear()
{
    m_CNCMotion.m_Stepper_block_Q->clear();
}

///
/// \brief My_MotorApp_Callback::set_RenewST_Ready
/// \param isReady
///
void My_MotorApp_Callback::set_RenewST_Ready(bool isReady){
    m_CNCMotion.m_RenewST_ready = isReady;
}

///
/// \brief 检查IO地址是否释放
/// \return
///
bool My_MotorApp_Callback::is_InputPtr_Release()
{
    if(m_CNCMotion.get_IutputPtr() == NULL){
        return true;
    }
    else{
        return false;
    }
}

void My_MotorApp_Callback::set_Mode_Calibrate(bool mode){
    m_isMode_calibrate = mode;
}

///
/// \brief My_MotorApp_Callback::GcodeSendThread_Func
/// \param dobotMotion
///
void My_MotorApp_Callback::GcodeSendThread_Func(CNC_Motion &dobotMotion){
    if(dobotMotion.m_Stepper_control->step_count == 0){//需要获取新的block
        if(dobotMotion.m_Stepper_block_Q->empty() && dobotMotion.m_RenewST_ready){
             dobotMotion.m_RenewST_ready = false;
             if(m_GcodeSegment_Q){
                 if(!m_GcodeSegment_Q->empty()){
                     Gcode_segment segment = m_GcodeSegment_Q->dequeue();
                     QVector3D data = segment.data_xyz;
                     int Mcode = segment.Mcode;
//                         qDebug() << data;
//                         qDebug() << "Mcode:"<<Mcode;

                     emit Gcode_lineChange(segment.line);//发射Gcode行号改变的信号

                     if(Mcode >Gcode_segment::RobotChange){
                         m_robot_LastIndex = m_robot_index;
                         m_robot_index = Mcode-Gcode_segment::RobotChange;
                     }
//                         qDebug() << "m_robot_index="<<m_robot_index;

                     m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_X] = data.x();
                     m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Y] = data.y();
                     m_ARM_Motion_test->arm[My_MotorApp_Callback::AXIS_Z] = data.z();
                     int ret = Planner_BufferLine(m_ARM_Motion_test->arm,segment,m_robot_index);
                     if(ret==Planner_EmptyBlock){//空的执行块
                         dobotMotion.m_RenewST_ready = true;//重新获取新的执行块
                     }
                     else if(ret == Planner_ErrorBlack){//空的执行块
                         Send_BottomMessage_change(QString("Line "+QString::number(segment.line)+":\tCalculatr ARM Error!"));
                         dobotMotion.m_RenewST_ready = true;//重新获取新的执行块
                     }

                     m_sys_reset = false;
                 }
             }
             else{
                 qDebug() << "m_GcodeSegment_Q==NULL!";
             }

        }
     }
}

void My_MotorApp_Callback::Send_BottomMessage_change(QString message)
{
   emit Control_BottomMessage_change(message);
}

///
/// \brief My_MotorApp_Callback::GcodeSendThread_run
///
void My_MotorApp_Callback::GcodeSendThread_run(){
    while(isRun){
        GcodeSendThread_Func(m_CNCMotion);
        QThread::msleep(5);
    }
//    Planner_BufferLine(m_ARM_Motion_test.arm,0);
//    m_sys_reset = false;
}

