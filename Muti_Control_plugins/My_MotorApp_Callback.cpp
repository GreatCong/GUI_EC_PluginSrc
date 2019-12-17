#include "My_MotorApp_Callback.h"
#include <QDebug>

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

//    Arm_motion_reset();
    isRun = true;

//    m_slave_index = 0;
    this->Master_setSlaveChooseIndex(0);//默认是0
    m_robot_index = 0;//机器人的编号
    m_robot_LastIndex = 0;
    m_isMode_calibrate = false;

    this->Master_setAppScan_allowRead(true);//设置成允许从站扫描之后获取过程数据
    this->Master_setAppScan_ReadTimeout(2000);//设置timeout大些

    m_SlaveChooseError = SlaveChoose_err_none;
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
    bool tmp_flag = false;

    if(m_CNCMotion_array.size() > 0){
        for(int robot_index = 0;robot_index < m_CNCMotion_array.size();robot_index++){
            m_CNCMotion_array[robot_index]->m_Stepper_control->step_count = *(m_CNCMotion_array[robot_index]->get_Output_MotorCountPtr());
            if(m_sys_reset){//如果用户进行了复位
               m_CNCMotion_array[robot_index]->m_Stepper_control->step_count=0;
               STEP_BIT_SetTrue(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),1<<step_AutoRun_stop);//从站中stop判断在前
               Motor_Reset();
               *(m_CNCMotion_array[robot_index]->get_Iutput_MotorStepPtr()+AXIS_TOOL1) = 0;
            }
        }

        for(int robot_index = 0;robot_index < m_CNCMotion_array.size();robot_index++){
            //BUG:如果步数比较小,有时候无法自动完成激励
            tmp_flag = false;

            if(m_CNCMotion_array[robot_index]->m_McodeFlag){
                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_1]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_1)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xff00);//高8位置0
                        memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                        //NOTE:这里直接设成0，需要测试是否有问题
                        m_CNCMotion_array[robot_index]->m_sys_position[AXIS_1] = 0;
                    }
                }
                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_2]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_2)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xff00);//高8位置0
                         memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                         //NOTE:这里直接设成0，需要测试是否有问题
                         m_CNCMotion_array[robot_index]->m_sys_position[AXIS_2] = 0;
                    }
                }

                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_3]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_3)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xf0);//高8位置0
                         memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                         //NOTE:这里直接设成0，需要测试是否有问题
                         m_CNCMotion_array[robot_index]->m_sys_position[AXIS_3] = 0;
                    }
                }

                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_4]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_4)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xf0);//高8位置0
                         memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                         //NOTE:这里直接设成0，需要测试是否有问题
//                         m_CNCMotion_array[robot_index]->m_sys_position[AXIS_4] = 0;
                    }
                }

                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_5]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_5)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xf0);//高8位置0
                         memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                         //NOTE:这里直接设成0，需要测试是否有问题
//                         m_CNCMotion_array[robot_index]->m_sys_position[AXIS_5] = 0;
                    }
                }

                if(m_CNCMotion_array[robot_index]->m_limit_state[AXIS_6]){
                    if(STEP_BIT_IsTrue(*(m_CNCMotion_array[robot_index]->get_OutputPtr()+Output_limit_state),1<<AXIS_6)){
                        tmp_flag = true;
                        STEP_BIT_SetFalse(*(m_CNCMotion_array[robot_index]->get_IutputPtr()+step_setting),0xf0);//高8位置0
                         memset(m_CNCMotion_array[robot_index]->m_limit_state,false,sizeof(m_CNCMotion_array[robot_index]->m_limit_state));
                         //NOTE:这里直接设成0，需要测试是否有问题
//                         m_CNCMotion_array[robot_index]->m_sys_position[AXIS_6] = 0;
                    }
                }

                if(tmp_flag){
                    m_CNCMotion_array[robot_index]->m_RenewST_init = true;//给一个激励
                }

            }// is flag


            if(m_CNCMotion_array[robot_index]->m_RenewST_init){//给一个循环运动的激励
                if((*(m_CNCMotion_array[robot_index]->get_OutputPtr()+motor_MSG)==2) || m_CNCMotion_array[robot_index]->m_McodeFlag){
                        m_CNCMotion_array[robot_index]->m_RenewST_ready = true;
                        m_CNCMotion_array[robot_index]->m_RenewST_init = false;
                        m_CNCMotion_array[robot_index]->m_McodeFlag = false;
                }
            }
        }

        for(int robot_index = 0;robot_index < m_CNCMotion_array.size();robot_index++){
                m_CNCMotion_array[robot_index]->loopRun();//根据新的block进行循环运动
        }

        //qDebug() << QString::number(*(m_CNCMotion_array[0]->get_IutputPtr()+step_setting),2);

    }//if

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

    const uint32_t* output_MotorCount_ptr = NULL;
    const int16_t* output_ptr = NULL;
    uint16_t* input_ptr = NULL;
    uint32_t* input_MotorStep_ptr = NULL;

    //NOTE:设置第一个从站是可以改变的（这里不校验，因为会重载Master_setSlaveChooseIndex函数）
    int index_robot0 = 0;
    if(this->Master_getSlaveChooseIndex() < this->Master_getSlaveCount()){
        if(m_Master_addressList[this->Master_getSlaveChooseIndex()].slave_name.compare(m_slaveName_limit)){
            index_robot0 = m_array_slave_id[0];
            this->Send_BottomMessage_change("Warning:Slave_index is invalid,it is changed to Slave_"+QString::number(index_robot0+1));
        }
        else{
            index_robot0 = this->Master_getSlaveChooseIndex();
        }
    }

    //更改根据从站编号获取偏移地址
    output_ptr = (int16_t*)(m_Master_addressBase+m_Master_addressList[index_robot0].inputs_offset + 0x04);
    input_ptr = (uint16_t*)(m_Master_addressBase+m_Master_addressList[index_robot0].outputs_offset);
    input_MotorStep_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[index_robot0].outputs_offset+0x08);
    output_MotorCount_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[index_robot0].inputs_offset);

    //设置dobot的IO地址
    m_CNCMotion_array[0]->set_OutputPtr(output_ptr);
    m_CNCMotion_array[0]->set_IutputPtr(input_ptr);
    m_CNCMotion_array[0]->set_Iutput_MotorStepPtr(input_MotorStep_ptr);
    m_CNCMotion_array[0]->set_Output_MotorCountPtr(output_MotorCount_ptr);


    int robot_id = -1;
    int robot_type = -1;
    int robot_id_index = -1;
    for(robot_id_index = 0;robot_id_index < m_array_slave_id.size();robot_id_index++){//查找适合的id_index
        if(m_array_slave_id[robot_id_index] == index_robot0){
            break;
        }
    }

    if(m_array_robot_id[robot_id_index] == 0){//如果是硬件没有初始化，或者没有获取到id信息
        robot_id = m_array_robot_id[robot_id_index];
    }
    else{
        robot_id = m_array_robot_id[robot_id_index]-1;
    }

    robot_type = m_algorithm_loader.get_robotDH_parameters()[robot_id].value_type;//从excel中获取的数据
    m_CNCMotion_array[0]->set_function_ptr(m_algorithm_loader.get_algorithm_dll_array()[robot_type]);//从json中读取的第一个是雕刻机的对象
    m_CNCMotion_array[0]->set_machine_param(m_algorithm_loader.get_Algorithm_obj().machine_param[robot_type]);
    m_CNCMotion_array[0]->set_slave_num(m_array_slave_id[robot_id_index]);
    m_CNCMotion_array[0]->renew_robotDH_ptr(m_algorithm_loader.get_robotDH_parameters()[robot_id]);//传递DH参数

    //可视化页面只是发第一个从站的信息
    *m_PositionInit = m_CNCMotion_array[0]->get_PositionInit();
    emit Send_Robot_PosInit(QVector3D(m_PositionInit->arm[CNC_Motion::AXIS_X],m_PositionInit->arm[CNC_Motion::AXIS_Y],m_PositionInit->arm[CNC_Motion::AXIS_Z]));

    qDebug() << "AppStart_callback:slave0," << m_CNCMotion_array[0]->get_model_name();

    //对除了第一个从站的处理
    if(this->Master_getSlaveCount() > 1){
        if(m_CNCMotion_array.size() > 1){
            for(int robot_index = 1;robot_index < m_CNCMotion_array.size();robot_index++){
                output_ptr = (int16_t*)(m_Master_addressBase+m_Master_addressList[m_array_slave_id[robot_index]].inputs_offset + 0x04);
                input_ptr = (uint16_t*)(m_Master_addressBase+m_Master_addressList[m_array_slave_id[robot_index]].outputs_offset);
                input_MotorStep_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[m_array_slave_id[robot_index]].outputs_offset+0x08);
                output_MotorCount_ptr = (uint32_t*)(m_Master_addressBase+m_Master_addressList[m_array_slave_id[robot_index]].inputs_offset);

                m_CNCMotion_array[robot_index]->set_OutputPtr(output_ptr);
                m_CNCMotion_array[robot_index]->set_IutputPtr(input_ptr);
                m_CNCMotion_array[robot_index]->set_Iutput_MotorStepPtr(input_MotorStep_ptr);
                m_CNCMotion_array[robot_index]->set_Output_MotorCountPtr(output_MotorCount_ptr);

                //qDebug() << "AppStart_callback:slave"+QString::number(robot_index) <<","<<m_CNCMotion_array[robot_index]->get_model_name();
            }
        }//if
    }//if

    //设定一个初始值,否则Ethercat会将循环的数值传递到从站
   Motor_Reset();
   if(m_CNCMotion_array.size() > 0){
       for(int robot_index = 1;robot_index < m_CNCMotion_array.size();robot_index++){
           *(m_CNCMotion_array[robot_index]->get_Iutput_MotorStepPtr()+AXIS_TOOL1) = 0;
       }
   }
//   xx=0;
   isRun = true;
//   if(m_GcodeSegment_Q->empty()){
//       qDebug() << "My_MotorApp_Callback Q empty!";
//   }
//   else{
//       qDebug() << "My_MotorApp_Callback Q No empty!";
//   }

   emit Master_QuitSignal(false);//线程开始

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
  if(m_CNCMotion_array.size() > 0){
      for(int robot_index = 0;robot_index < m_CNCMotion_array.size();robot_index++){
         m_CNCMotion_array[robot_index]->m_Stepper_control->step_count = 0;
      }
  }

  m_robot_index = 0;
  m_robot_LastIndex = 0;

//  memset(&m_sys_position,0,sizeof(m_sys_position));
  Arm_motion_reset();//不能全部置0

  emit Master_QuitSignal(true);//线程停止
}

///
/// \brief My_MotorApp_Callback::Master_AppScan_callback
///
void My_MotorApp_Callback::Master_AppScan_callback()
{
    emit Master_ScanSignal();//发送scan消息

    if(this->Master_getAppScan_allowRead()){
        const int16_t* output_ptr = NULL;
        int robot_id = -1;
//        QVector<int> array_robot_id;
        m_array_robot_id.clear();
        m_array_slave_id.clear();

        int robot_model_count = 0;
        for(int slave_index = 0;slave_index < this->Master_getSlaveCount();slave_index++){
            if(m_Master_addressList[slave_index].slave_name.compare(m_slaveName_limit)){//如果不是这个从站型号
                continue;
            }
            //更改根据从站编号获取偏移地址
            output_ptr = (int16_t*)(m_Master_addressBase+m_Master_addressList[slave_index].inputs_offset + 0x04);
            robot_id = *(output_ptr+Output_machine_param);
//            qDebug() << "robot_id:"+ QString::number(robot_id);
            m_array_robot_id.push_back(robot_id);
            m_array_slave_id.push_back(slave_index);
            robot_model_count++;
        }
//          m_array_robot_id[0] = 2;// 测试用

//        m_CNCMotion_array.resize(this->Master_getSlaveCount());//重新设置模型的大小
        if(robot_model_count == 0){//说明没有对应的机器人从站
            m_SlaveChooseError = SlaveChoose_err_noRobot;
            emit Master_QuitSignal(true);
            Send_BottomMessage_change("No robot found here!");
            return;
        }

        m_CNCMotion_array.resize(robot_model_count);//重新设置模型的大小

        //qDebug() << array_robot_id;
        int robot_type = -1;
        bool isSendWaring = false;
        for(int model_index=0;model_index < m_CNCMotion_array.size();model_index ++){
            m_CNCMotion_array[model_index] = new CNC_Motion;
            //NOTE:机器人的id号是从1开始的,如果是0的话，说明硬件没有初始化,因此下面获取DH参数时候要减去1
            if(m_array_robot_id[model_index] == 0){//如果是硬件没有初始化，或者没有获取到id信息
                isSendWaring = true;
                robot_id = m_array_robot_id[model_index];
            }
            else{
                robot_id = m_array_robot_id[model_index]-1;
            }
            robot_type = m_algorithm_loader.get_robotDH_parameters()[robot_id].value_type;//从excel中获取的数据
            m_CNCMotion_array[model_index]->set_function_ptr(m_algorithm_loader.get_algorithm_dll_array()[robot_type]);//从json中读取的第一个是雕刻机的对象
            m_CNCMotion_array[model_index]->set_machine_param(m_algorithm_loader.get_Algorithm_obj().machine_param[robot_type]);
            m_CNCMotion_array[model_index]->set_slave_num(m_array_slave_id[model_index]);

            m_CNCMotion_array[model_index]->renew_robotDH_ptr(m_algorithm_loader.get_robotDH_parameters()[robot_id]);//传递DH参数

            qDebug() << "robot " << QString::number(model_index)
                     << "slave " << QString::number(m_array_slave_id[model_index]+1)
                     << " name:" << m_CNCMotion_array[model_index]->get_model_name()
                     << " id: " << QString::number(m_array_robot_id[model_index])
                     << " model_type: " << QString::number(robot_type) ;
        }

        if(isSendWaring){
            emit Send_Robot_WariningBox("some of robots'id are invalid , all of these are set to CNC model");
        }

        //NOTE:如果放在前面,会导致m_CNCMotion_array被占用,robot_id一直为0，出错
        emit Send_Robot_ID(m_array_robot_id,m_array_slave_id);

        this->Arm_motion_reset();
    }

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
    if(m_CNCMotion_array.size() > 0){
        for(int i=0;i<m_CNCMotion_array.size();i++){
            m_CNCMotion_array[i]->release_IO_ptr();
        }
    }
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

void My_MotorApp_Callback::Master_setSlaveChooseIndex(int num)
{
    m_slaveChoose_index = num;

    if(this->Master_getSlaveCount() > 0){
        if(this->Master_getAddressList().at(m_slaveChoose_index).slave_name.compare(m_slaveName_limit)){
            m_SlaveChooseError = SlaveChoose_err_InvalidIndex;
            emit Master_QuitSignal(true);
            Send_BottomMessage_change("Set Slave error!");
        }
    }

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

    if(m_CNCMotion_array.size() > 0){
        for(int robot_index =0;robot_index < m_CNCMotion_array.size();robot_index++){
             m_CNCMotion_array[robot_index]->Motor_Reset();
        }
    }
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
         if(robot_index == 0){
             robot_index = 1;
         }
        res = m_CNCMotion_array[robot_index-1]->planner_BufferLine(target,userData,sendState);

        if(sendState->sendPositionOK){
            QVector3D position_send = QVector3D(sendState->position_send->arm[0],sendState->position_send->arm[1],sendState->position_send->arm[2]);
            emit Gcode_PositionChange(position_send);//发送正解
        }

        if(sendState->sendThetaOK){
          QVector3D theta_send = QVector3D(sendState->theta_send->arm[0],sendState->theta_send->arm[1],sendState->theta_send->arm[2]);
          emit Gcode_ThetaChange(theta_send);//发送反解
        }

        delete sendState;//避免内存泄漏

        break;
//    case 2:
//        sendState= new ARM_SendState(CNC_Motion::AXIS_N);
//        res = m_CNCMotion1.planner_BufferLine(target,userData,sendState);
//        break;
//    case 3:
//        break;
    default:
        if((robot_index-1) < m_CNCMotion_array.size()){
            //其他的可以不考虑发送正反解信息了
            sendState= new ARM_SendState(CNC_Motion::AXIS_N);
            res = m_CNCMotion_array[robot_index-1]->planner_BufferLine(target,userData,sendState);
            delete sendState;//避免内存泄漏
        }
        else{
            Send_BottomMessage_change("robot_index = "+QString::number(robot_index) + " is invalid!");
        }

        break;
    }

    return res;
}

///
/// \brief My_MotorApp_Callback::Arm_motion_reset
///
void My_MotorApp_Callback::Arm_motion_reset(){
    if(m_CNCMotion_array.size() > 0){
        for(int robot_index = 0;robot_index<m_CNCMotion_array.size();robot_index++){
          m_CNCMotion_array[robot_index]->Arm_motion_reset(m_isMode_calibrate);//m_isMode_calibrate默认是false
          m_CNCMotion_array[robot_index]->State_Reset();
        }

        //可视化页面只是发第一个从站的信息
        *m_PositionInit = m_CNCMotion_array[0]->get_PositionInit();
        emit Send_Robot_PosInit(QVector3D(m_PositionInit->arm[CNC_Motion::AXIS_X],m_PositionInit->arm[CNC_Motion::AXIS_Y],m_PositionInit->arm[CNC_Motion::AXIS_Z]));
    }

}

///
/// \brief My_MotorApp_Callback::Control_QueueClear
///
void My_MotorApp_Callback::Control_QueueClear()
{
    if(m_CNCMotion_array.size() > 0){
        for(int robot_index = 0;robot_index<m_CNCMotion_array.size();robot_index++){
            m_CNCMotion_array[robot_index]->m_Stepper_block_Q->clear();
        }
    }
}

///
/// \brief My_MotorApp_Callback::set_RenewST_Ready
/// \param isReady
///
void My_MotorApp_Callback::set_RenewST_Ready(bool isReady){
    //可视化页面只是发第一个从站的信息
    m_CNCMotion_array[0]->m_RenewST_ready = isReady;
}

///
/// \brief 检查IO地址是否释放
/// \return
///
bool My_MotorApp_Callback::is_InputPtr_Release()
{
    if(m_CNCMotion_array.size() == 0){//如果没有初始化，就直接返回释放了
        return true;
    }

    //可视化页面只是发第一个从站的信息
    if(m_CNCMotion_array[0]->get_IutputPtr() == NULL){
        return true;
    }
    else{
        return false;
    }
}

void My_MotorApp_Callback::set_Mode_Calibrate(bool mode){
    m_isMode_calibrate = mode;
}

bool My_MotorApp_Callback::Init_Cores()
{
    bool ret = false;
    bool isOK = false;

   this->motion_init();

   int ret_json = m_algorithm_loader.json_load(m_path+"/../Machine_libs/Model_lib.json");

   switch (ret_json) {
   case Algorithm_loader::Json_err_fileInvalid:
       qDebug() << "could not open project json";
       break;
   case Algorithm_loader::Json_err_parseInvalid:
       qDebug() << "parse json error!";
       break;
   default:
       //qDebug() << "num = " << QString::number(ret);
       break;
   }

   isOK = true;
   if(ret_json == Algorithm_loader::Json_err_none){
       //检查dll是否存在
       foreach (QString fileName, m_algorithm_loader.get_Algorithm_obj().list_machine) {
#ifdef Q_OS_WIN
           ret = m_algorithm_loader.isFileExist(m_path+"/../Machine_libs/"+fileName+".dll");
#else //unix
           ret = m_algorithm_loader.isFileExist(m_path+"/../Machine_libs/lib"+fileName+".so");
#endif
           if(ret == false){
               Send_BottomMessage_change("model: " + fileName + " is invalid!");//此时消息响应还没有初始化，无法显示
//               qDebug() << "model: " << fileName << " is invalid!";
               isOK = false;
           }
       }

       if(isOK == false){
           return false;
       }

       bool ret = m_algorithm_loader.libArray_load(m_path+"/../Machine_libs/");
       if(!ret){
//           qDebug() << "load lib error!";
           Send_BottomMessage_change("load lib error!");
           return false;
       }
       else{
           //m_algorithm_loader.test();
          //Arm_motion_reset();
       }
   }
   else{
       Send_BottomMessage_change("Parse Json error!");
       return false;
   }

//   qDebug() << m_algorithm_loader.libArray_unload();
//   qDebug() << m_algorithm_loader.get_Algorithm_obj().describe;
//   qDebug() << m_algorithm_loader.get_Algorithm_obj().version;

//   foreach (QString str, m_algorithm_loader.get_Algorithm_obj().list_machine) {
//       qDebug() << str;
//   }

   return true;
}

bool My_MotorApp_Callback::Destroy_Cores()
{
   m_algorithm_loader.libArray_unload();

   this->motion_destroy();

   return true;
}

void My_MotorApp_Callback::set_robotDH_parameters(const QVector<Robot_parameter_s> array_robot_info)
{
    m_algorithm_loader.set_robotDH_parameters(array_robot_info);
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
                         //qDebug() << data << QString::number(dobotMotion.get_slave_num());
//                         qDebug() << "Mcode:"<<Mcode;

                     emit Gcode_lineChange(segment.line);//发射Gcode行号改变的信号

                     if(Mcode >Gcode_segment::RobotChange){
                         m_robot_LastIndex = m_robot_index;
                         m_robot_index = Mcode-Gcode_segment::RobotChange;
                     }
                     else if(Mcode == Gcode_segment::EndParse_CODE){//如果是结束行的话，就恢复机器人的默认编号
                        m_robot_LastIndex = 0;
                        m_robot_index = 0;
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

void My_MotorApp_Callback::motion_init()
{
    if(m_CNCMotion_array.size() > 0){
        for(int i=0;i<m_CNCMotion_array.size();i++){
            m_CNCMotion_array[i]->init_cores();
        }
    }

}

void My_MotorApp_Callback::motion_destroy()
{
    if(m_CNCMotion_array.size() > 0){
        for(int i=0;i<m_CNCMotion_array.size();i++){
            m_CNCMotion_array[i]->destroy_cores();
        }
    }
}

///
/// \brief My_MotorApp_Callback::GcodeSendThread_run
///
void My_MotorApp_Callback::GcodeSendThread_run(){
    while(isRun){
        if(m_CNCMotion_array.size() > 0){
            for(int robot_index = 0;robot_index < m_CNCMotion_array.size();robot_index++){
                GcodeSendThread_Func(*(m_CNCMotion_array[robot_index]));
            }
        }

        QThread::msleep(5);
    }
//    Planner_BufferLine(m_ARM_Motion_test.arm,0);
//    m_sys_reset = false;
}

