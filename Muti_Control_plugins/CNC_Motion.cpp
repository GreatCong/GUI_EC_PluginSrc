#include "CNC_Motion.h"

#include "GcodeParser.h"

///
/// \brief 构造函数初始化
///
CNC_Motion::CNC_Motion()
{
   m_PositionInit = new ARM_Struct(AXIS_N);
   m_Stepper_block_Q = new QQueue<Stepper_block*>();
   m_Stepper_control = new Stepper_control(AXIS_N);

   m_RenewST_init = false;//自动运行的激励标志位
   m_RenewST_ready = false;//自动运行的激励标志位
   m_McodeFlag = false;

   memset(&m_sys_position,0,sizeof(m_sys_position));
   memset(m_limit_state,false,sizeof(m_limit_state));
   release_IO_ptr();
}

///
/// \brief CNC反解(为了兼容需要反解的机器)
/// \param cartesian_theta
/// \param cartesian
/// \return
///
uint8_t CNC_Motion::calculate_arm(float *cartesian_theta, const float *cartesian)
{
#if 0
    uint8_t res_state = 0;

    cartesian_theta[AXIS_X] = cartesian[AXIS_X];
    cartesian_theta[AXIS_Y] = cartesian[AXIS_Y];
    cartesian_theta[AXIS_Z] = cartesian[AXIS_Z];
    #if ARM_PEINT_DEBUG
    Printf_MSG("[Arm_Theta=(%f,%f,%f)]\r\n",cartesian_theta[AXIS_X],cartesian_theta[AXIS_Y],cartesian_theta[AXIS_Z]);
    #endif

    return res_state;
#endif
    uint8_t res_state = 0;
    res_state = (*(m_algorithm_func.calculate_arm_dll))(cartesian_theta,cartesian,m_DH_arrayParam.DH_param_ptr,
                                                        m_DH_arrayParam.dof_num,m_DH_arrayParam.param_num);
    #if ARM_PEINT_DEBUG
    //Printf_MSG("[%d,input=(%f,%f,%f)]\r\n",this->get_slave_num(),cartesian[AXIS_X],cartesian[AXIS_Y],cartesian[AXIS_Z]);
    Printf_MSG("[%d,Arm_Theta=(%f,%f,%f)]\r\n",this->get_slave_num(),cartesian_theta[AXIS_X],cartesian_theta[AXIS_Y],cartesian_theta[AXIS_Z]);
    #endif
    return res_state;
}

///
/// \brief CNC正解(为了兼容需要正解的机器)
/// \param cartesian
/// \param cartesian_theta
/// \return
///
uint8_t CNC_Motion::calculate_forward(float *cartesian, const float *cartesian_theta)
{
#if 0
    uint8_t res_state = 0;

    cartesian[AXIS_X] = cartesian_theta[AXIS_X];
    cartesian[AXIS_Y] = cartesian_theta[AXIS_Y];
    cartesian[AXIS_Z] = cartesian_theta[AXIS_Z];

   #if ARM_PEINT_DEBUG
   Printf_MSG("[Arm_Pos=(%f,%f,%f)]\r\n",cartesian[AXIS_X],cartesian[AXIS_Y],cartesian[AXIS_Z]);
   #endif

   return res_state;
#endif

   uint8_t res_state = 0;
   res_state = (*(m_algorithm_func.calculate_forward_dll))(cartesian,cartesian_theta,m_DH_arrayParam.DH_param_ptr,
                                                           m_DH_arrayParam.dof_num,m_DH_arrayParam.param_num);
   #if ARM_PEINT_DEBUG
   Printf_MSG("[%d,Arm_Pos=(%f,%f,%f)]\r\n",this->get_slave_num(),cartesian[AXIS_X],cartesian[AXIS_Y],cartesian[AXIS_Z]);
   #endif
   return res_state;
}

///
/// \brief 获取单位转换关系
/// \param n
/// \return
///
float CNC_Motion::get_StepsPerUnit(int n)
{
    float res = 88.88f;
    switch(n){
    case AXIS_X:
        res = m_machine_parameter.step_per_unit[AXIS_X];
        break;
    case AXIS_Y:
        res = m_machine_parameter.step_per_unit[AXIS_Y];
        break;
    case AXIS_Z:
        res = m_machine_parameter.step_per_unit[AXIS_Z];
        break;
    default:
        break;

    }

    return res;
}

///
/// \brief 获取轴数
/// \return
///
int CNC_Motion::get_Axis_count()
{
    return AXIS_N;
}

///
/// \brief 复位
///
void CNC_Motion::Arm_motion_reset(bool isCalibrate)
{
    Q_UNUSED(isCalibrate);

    ARM_Struct arm_position_init(AXIS_N);//笛卡尔坐标系为(0,0,0)的角度

    arm_position_init.arm[AXIS_X] = m_machine_parameter.manual_home_pos[AXIS_X];
    arm_position_init.arm[AXIS_Y] = m_machine_parameter.manual_home_pos[AXIS_Y];
    arm_position_init.arm[AXIS_Z] = m_machine_parameter.manual_home_pos[AXIS_Z];

    calculate_forward(m_PositionInit->arm,arm_position_init.arm);
    #if ARM_PEINT_DEBUG
    //打印初始位置坐标，便于调试
    Printf_MSG("[%d,Arm_PosInit=(%f,%f,%f)]\r\n",this->get_slave_num(),m_PositionInit->arm[AXIS_X],m_PositionInit->arm[AXIS_Y],m_PositionInit->arm[AXIS_Z]);
    #endif

    for(int idx = 0; idx < AXIS_N; idx++) { //只用了3轴
         m_sys_position[idx] = std::lround(arm_position_init.arm[idx]*get_StepsPerUnit(idx));
         //qDebug() << "m_sys_position:" << QString::num(m_sys_position[idx]);
    }
}

///
/// \brief 获取初始正解坐标
/// \return
///
ARM_Struct CNC_Motion::get_PositionInit()
{
    return *m_PositionInit;
}

///
/// \brief 添加到运动块中
/// \param target
/// \param userData
/// \param send_signal
/// \return
///
int CNC_Motion::planner_BufferLine(const float * target, const Gcode_segment &userData, ARM_SendState *send_signal){
    //Q_UNUSED(userData);//后面再添加
    int mCode = userData.Mcode;
    double speed_spindle = userData.speed_spindle;
    double speed_step = userData.speed_step;

    float unit_vec[AXIS_N], delta_mm;
    int32_t target_steps[AXIS_N], position_steps[AXIS_N];

    Stepper_block* block_new = new Stepper_block(AXIS_N);
    block_new->step_event_count = 0;

    memcpy(position_steps, m_sys_position, sizeof(m_sys_position));// sys_position (vector in steps.int32)

    if(mCode > Gcode_segment::RobotChange){//如果是切换机器人的G代码
        //分别计算
        for(int idx = 0;idx < AXIS_N;idx++){

            block_new->Axis_steps[idx] = 0;
            block_new->step_event_count = std::max(block_new->step_event_count,block_new->Axis_steps[idx]);
        }
        block_new->Mcode = mCode;//赋值Mcode
        block_new->speed_spindle = speed_spindle;
        block_new->speed_step = speed_step;

        m_Stepper_block_Q->enqueue(block_new);
//             qDebug() << userData;
        return Planner_OK;
    }

    //正解校验
//    float print_position[3];
//    for(int i=0;i<3;i++){
//        print_position[i] = position_steps[i]/m_Step_perMM[i];
//    }
//    calculate_forward(print_position);

    //正解校验并显示
    float print_position[AXIS_N];
    for(int i=0;i<AXIS_N;i++){
        print_position[i] = position_steps[i]/get_StepsPerUnit(i);
    }
    ARM_Struct arm_position_target(AXIS_N);
    calculate_forward(arm_position_target.arm,print_position);
    for(int i=0;i<AXIS_N;i++){
        send_signal->position_send->arm[i] =arm_position_target.arm[i];
    }
     send_signal->sendPositionOK = true;
    //QVector3D position_send = QVector3D(arm_position_target.arm[0],arm_position_target.arm[1],arm_position_target.arm[2]);
    //emit Gcode_PositionChange(position_send);//发送正解

    //如果是注释的Gcode，则为空,包含其他的忽略的Mcode
    if(mCode <= Gcode_segment::COMMENT_CODE){
        delete block_new;//避免内存泄漏
        return Planner_EmptyBlock;//暂时将其定为EMPTY_BLOCK,即不运动
    }

    //将target转化为角度坐标(反解)
    ARM_Struct arm_motion_target(AXIS_N);
    arm_motion_target.transfer_state = calculate_arm(arm_motion_target.arm,target);
    if(arm_motion_target.transfer_state > 0){ //不符合余弦定理,有潜在的隐患
      qDebug() << "Calculate ARM error,Line:"<<userData.line<<userData.data_xyz;

      delete block_new;//避免内存泄漏
      return Planner_ErrorBlack;//暂时将其定为EMPTY_BLOCK,即不运动
    }

//    QVector3D theta_send = QVector3D(arm_motion_target.arm[0],arm_motion_target.arm[1],arm_motion_target.arm[2]);
//    emit Gcode_ThetaChange(theta_send);//发送反解
    for(int i=0;i<AXIS_N;i++){
        send_signal->theta_send->arm[i] =arm_motion_target.arm[i];
    }
     send_signal->sendThetaOK = true;

    //分别计算
    for(int idx = 0;idx < AXIS_N;idx++){

        target_steps[idx] = std::lround(arm_motion_target.arm[idx]*get_StepsPerUnit(idx));
        //target_steps[idx] = lround(target[idx]*settings.steps_per_mm[idx]);

        block_new->Axis_steps[idx] = std::labs(target_steps[idx]-position_steps[idx]);
        block_new->step_event_count = std::max(block_new->step_event_count,block_new->Axis_steps[idx]);

        delta_mm = (target_steps[idx] - position_steps[idx])/get_StepsPerUnit(idx);

        unit_vec[idx] = delta_mm; // Store unit vector numerator

        // Set direction bits. Bit enabled always means direction is negative.
        if(delta_mm < 0.0) {
            block_new->direction_bits |= (1<< idx);//方向取反
        }

//        qDebug() << "Delta:"<<delta_mm;
    }//for

    block_new->Mcode = mCode;//赋值Mcode
    block_new->speed_spindle = speed_spindle;
    block_new->speed_step = speed_step;

//    qDebug() << block_new->Axis_steps[0] << block_new->Axis_steps[1] << block_new->Axis_steps[2];
//    qDebug() << "Dir:"<<block_new->direction_bits;

    ///偏移量为0，则为空的执行块
    if(block_new->step_event_count == 0) {
        if(mCode > Gcode_segment::No_Mcode){//有Mcode的代码不取消
             m_Stepper_block_Q->enqueue(block_new);
//             qDebug() << userData;
             return Planner_OK;
        }

        delete block_new;//避免内存泄漏
        return Planner_EmptyBlock;
    }



    //添加到block队列
    m_Stepper_block_Q->enqueue(block_new);

    return Planner_OK;
}

///
/// \brief CNC_Motion::loopRun
///
void CNC_Motion::loopRun()
{
    if( m_Stepper_control->step_count== 0){//需要获取新的block
        if(m_Stepper_block_Q->empty()){
            return ;
        }
        else{
            m_Stepper_control->exec_block = m_Stepper_block_Q->dequeue();
        //            m_Stepper_control.step_count = m_Stepper_control.exec_block->step_event_count;//一共运行所有轴的最大步长
        //            m_Stepper_control.counter[AXIS_X] = m_Stepper_control.counter[AXIS_Y] = m_Stepper_control.counter[AXIS_Z] = (m_Stepper_control.exec_block->step_event_count >> 1);

            m_Stepper_control->Axis_steps[AXIS_X] = m_Stepper_control->exec_block->Axis_steps[AXIS_X];
            m_Stepper_control->Axis_steps[AXIS_Y] = m_Stepper_control->exec_block->Axis_steps[AXIS_Y];
            m_Stepper_control->Axis_steps[AXIS_Z] = m_Stepper_control->exec_block->Axis_steps[AXIS_Z];
            //设置变速
            if(m_Stepper_control->exec_block->step_event_count < 200){//步数太小的时候，速度设置小一些
                //NOTE:最小频率设置为180，再小会变大???
                *(input_ptr+step_fre) = this->MIN_Speed_Step;//设置频率为1000Hz
            }
            else{
//                 *(input_ptr+step_fre) = 1000;//设置频率为1000Hz
                if(m_Stepper_control->exec_block->speed_step > this->MAX_Speed_Step){
                    m_Stepper_control->exec_block->speed_step = this->MAX_Speed_Step;
                }
                *(input_ptr+step_fre) = m_Stepper_control->exec_block->speed_step;//设置频率为1000Hz
            }


            m_Stepper_control->dir_outbits = m_Stepper_control->exec_block->direction_bits;//传递dir bits
            *(input_ptr+step_setting) = 1;

            STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行
            STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_AutoRun_stop);//从站中stop判断在前
            *(input_ptr+motor_dir) = m_Stepper_control->dir_outbits;
            *(input_MotorStep_ptr+motor1_step) = m_Stepper_control->Axis_steps[AXIS_X];
            *(input_MotorStep_ptr+motor2_step) = m_Stepper_control->Axis_steps[AXIS_Y];
            *(input_MotorStep_ptr+motor3_step) = m_Stepper_control->Axis_steps[AXIS_Z];

            *(input_ptr+motor_dir) |= (m_Stepper_control->dir_outbits)<<3 ;//测试4-6轴
            *(input_MotorStep_ptr+motor4_step) = m_Stepper_control->Axis_steps[AXIS_X];
            *(input_MotorStep_ptr+motor5_step) = m_Stepper_control->Axis_steps[AXIS_Y];
            *(input_MotorStep_ptr+motor6_step) = m_Stepper_control->Axis_steps[AXIS_Z];

            int32_t steps_loopNum[CNC_Motion::AXIS_N];
            int32_t steps_remain[CNC_Motion::AXIS_N];
            int32_t steps_limit = 3000;
            steps_loopNum[AXIS_X] = m_Stepper_control->Axis_steps[AXIS_X]/steps_limit;
            steps_remain[AXIS_X] = m_Stepper_control->Axis_steps[AXIS_X]%steps_limit;
            steps_loopNum[AXIS_Y] = m_Stepper_control->Axis_steps[AXIS_Y]/steps_limit;
            steps_remain[AXIS_Y] = m_Stepper_control->Axis_steps[AXIS_Y]%steps_limit;
            steps_loopNum[AXIS_Z] = m_Stepper_control->Axis_steps[AXIS_Z]/steps_limit;
            steps_remain[AXIS_Z] = m_Stepper_control->Axis_steps[AXIS_Z]%steps_limit;
        //            qDebug() <<"steps_loopNum"<<steps_loopNum[AXIS_X] << steps_remain[AXIS_X];
            //根据方向记录位置信息
            while(steps_loopNum[AXIS_X]-->0){
                if(m_Stepper_control->dir_outbits & (1<<AXIS_X)) {
        //                m_sys_position[AXIS_X]-=m_Stepper_control.Axis_steps[AXIS_X];//直接相加减会出现有符号数和无符号数之间的运算
                    m_sys_position[AXIS_X] -= steps_limit;
                }
                else {
        //                    m_sys_position[AXIS_X]+=m_Stepper_control.Axis_steps[AXIS_X];
                    m_sys_position[AXIS_X] += steps_limit;
                }
            }
            if(m_Stepper_control->dir_outbits & (1<<AXIS_X)) {//剩余的数字进行计算
        //                m_sys_position[AXIS_X]-=m_Stepper_control.Axis_steps[AXIS_X];//直接相加减会出现有符号数和无符号数之间的运算
                m_sys_position[AXIS_X] -= steps_remain[AXIS_X];
            }
            else {
        //                    m_sys_position[AXIS_X]+=m_Stepper_control.Axis_steps[AXIS_X];
                m_sys_position[AXIS_X] += steps_remain[AXIS_X];
            }

            while(steps_loopNum[AXIS_Y]-->0){
                if(m_Stepper_control->dir_outbits & (1<<AXIS_Y)) {
        //                m_sys_position[AXIS_Y]-=m_Stepper_control.Axis_steps[AXIS_Y];//直接相加减会出现有符号数和无符号数之间的运算
                    m_sys_position[AXIS_Y] -= steps_limit;
                }
                else {
        //                    m_sys_position[AXIS_Y]+=m_Stepper_control.Axis_steps[AXIS_Y];
                    m_sys_position[AXIS_Y] += steps_limit;
                }
            }
            if(m_Stepper_control->dir_outbits & (1<<AXIS_Y)) {//剩余的数字进行计算
        //                m_sys_position[AXIS_Y]-=m_Stepper_control.Axis_steps[AXIS_Y];//直接相加减会出现有符号数和无符号数之间的运算
                m_sys_position[AXIS_Y] -= steps_remain[AXIS_Y];
            }
            else {
        //                    m_sys_position[AXIS_Y]+=m_Stepper_control.Axis_steps[AXIS_Y];
                m_sys_position[AXIS_Y] += steps_remain[AXIS_Y];
            }

            while(steps_loopNum[AXIS_Z]-->0){
                if(m_Stepper_control->dir_outbits & (1<<AXIS_Z)) {
        //                m_sys_position[AXIS_Z]-=m_Stepper_control.Axis_steps[AXIS_Z];//直接相加减会出现有符号数和无符号数之间的运算
                    m_sys_position[AXIS_Z] -= steps_limit;
                }
                else {
        //                    m_sys_position[AXIS_Z]+=m_Stepper_control.Axis_steps[AXIS_Z];
                    m_sys_position[AXIS_Z] += steps_limit;
                }
            }
            if(m_Stepper_control->dir_outbits & (1<<AXIS_Z)) {//剩余的数字进行计算
        //                m_sys_position[AXIS_Z]-=m_Stepper_control.Axis_steps[AXIS_Z];//直接相加减会出现有符号数和无符号数之间的运算
                m_sys_position[AXIS_Z] -= steps_remain[AXIS_Z];
            }
            else {
        //                    m_sys_position[AXIS_Z]+=m_Stepper_control.Axis_steps[AXIS_Z];
                m_sys_position[AXIS_Z] += steps_remain[AXIS_Z];
            }

            m_RenewST_init = true;

            if(m_Stepper_control->exec_block->Mcode !=Gcode_segment::No_Mcode){
//                qDebug() << "Mcode_debug:"<<m_Stepper_control->exec_block->Mcode;
                switch(m_Stepper_control->exec_block->Mcode){
                case Gcode_segment::MotorTool_On:
//                    *(input_MotorStep_ptr+AXIS_TOOL1) = 2000;
                    if(m_Stepper_control->exec_block->speed_spindle >this->MAX_Speed_Spindle){//最大转速是2000
                        m_Stepper_control->exec_block->speed_spindle = this->MAX_Speed_Spindle;
                    }
                    *(input_MotorStep_ptr+AXIS_TOOL1) = m_Stepper_control->exec_block->speed_spindle;
        //                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行
                    break;
                case Gcode_segment::MotorTool_Off:
                    *(input_MotorStep_ptr+AXIS_TOOL1) = this->MIN_Speed_Spindle;
        //                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行
                    break;
                case Gcode_segment::MotorTool1_On://控制继电器
                    STEP_BIT_SetTrue(*(input_ptr+io_output),1<<0);//暂时设置第一位控制
        //                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行
                    break;
                case Gcode_segment::MotorTool1_Off://控制继电器
                    STEP_BIT_SetFalse(*(input_ptr+io_output),1<<0);//暂时设置第一位控制
        //                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行
                    break;
                case Gcode_segment::LIMIT_Wait_X:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitX);//设置等待限位X
                    m_limit_state[AXIS_1] = true;
                    m_RenewST_init = false;//取消自动运行
                    //qDebug() << "LIMIT_Wait_X:"+ QString::number(*(input_ptr+step_setting),2);
                    break;
                case Gcode_segment::LIMIT_Wait_Y:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitY);//设置等待限位Y
                    m_limit_state[AXIS_2] = true;
                    m_RenewST_init = false;//取消自动运行
                    break;
                case Gcode_segment::LIMIT_Wait_Z:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitZ);//设置等待限位Z
                    m_limit_state[AXIS_3] = true;
                    m_RenewST_init = false;//取消自动运行
                    break;
                case Gcode_segment::LIMIT_Wait_A:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitA);//设置等待限位A
                    m_limit_state[AXIS_4] = true;
                    m_RenewST_init = false;//取消自动运行
                    break;
                case Gcode_segment::LIMIT_Wait_B:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitB);//设置等待限位B
                    m_limit_state[AXIS_5] = true;
                    m_RenewST_init = false;//取消自动运行
                    break;
                case Gcode_segment::LIMIT_Wait_C:
                    STEP_BIT_SetTrue(*(input_ptr+step_setting),1<<step_wait_limitC);//设置等待限位C
                    m_limit_state[AXIS_6] = true;
                    m_RenewST_init = false;//取消自动运行
                    break;
                default:
                    if(m_Stepper_control->exec_block->Mcode < Gcode_segment::RobotChange ||
                            m_Stepper_control->exec_block->Mcode >= Gcode_segment::RobotChange_end){
                        qDebug() << "Mcode:"<<m_Stepper_control->exec_block->Mcode;
                    }
                    else{
//                        qDebug() << "change_Mcode:"<<m_Stepper_control->exec_block->Mcode;
                    }

                    break;
                }


               m_McodeFlag = true;
        //                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_stop);//从站中stop判断在前
                STEP_BIT_SetFalse(*(input_ptr+step_setting),1<<step_AutoRun_start);//设置自动运行

        //                qDebug() << "Mcode:"<<m_Stepper_control.exec_block->Mcode;
        //                qDebug() << *(input_MotorStep_ptr+AXIS_4);
        //                qDebug() << *(input_ptr+step_setting);
            }


            delete m_Stepper_control->exec_block;
            m_Stepper_control->exec_block = NULL;//释放内存
        }
    }
    else{
        Motor_Reset();
        *(input_ptr+step_setting) = 1 | ((*(input_ptr+step_setting))&0xff00);//只是使能，去掉自动运行的标志
    }

}

///
/// \brief 机器人状态的Reset
///
void CNC_Motion::Motor_Reset()
{
    //设置方向
    *(input_ptr+motor_dir) = 0;//dir

     //设置Step
    *(input_MotorStep_ptr+motor1_step) = 0;//step1
    *(input_MotorStep_ptr+motor2_step) = 0;//step2
    *(input_MotorStep_ptr+motor3_step) = 0;//step3

    *(input_MotorStep_ptr+motor4_step) = 0;//step1
    *(input_MotorStep_ptr+motor5_step) = 0;//step2
    *(input_MotorStep_ptr+motor6_step) = 0;//step3
}

///
/// \brief 电机的Reset
///
void CNC_Motion::State_Reset()
{
    m_RenewST_init = false;//自动运行的激励标志位
    m_RenewST_ready = false;//自动运行的激励标志位
    m_McodeFlag = false;
}

///
/// \brief 更新机器人的DH参数
/// \param robot_DH
///
void CNC_Motion::renew_robotDH_ptr(Robot_parameter_s &robot_DH)
{
    if(m_DH_arrayParam.DH_param_ptr){
        delete[] m_DH_arrayParam.DH_param_ptr;
        m_DH_arrayParam.DH_param_ptr = nullptr;
        m_DH_arrayParam.dof_num = 0;
        m_DH_arrayParam.param_num = 0;
    }

    m_DH_arrayParam.dof_num = robot_DH.value_DOF;
    m_DH_arrayParam.param_num = 3;//一行3个参数
    m_DH_arrayParam.DH_param_ptr = new float*[m_DH_arrayParam.dof_num];
    for(int dof_index=0;dof_index<robot_DH.value_DOF;dof_index++){
        m_DH_arrayParam.DH_param_ptr[dof_index] = new float[m_DH_arrayParam.param_num];
        m_DH_arrayParam.DH_param_ptr[dof_index][0] = robot_DH.array_DH_parameter[dof_index].value_d;
        m_DH_arrayParam.DH_param_ptr[dof_index][1] = robot_DH.array_DH_parameter[dof_index].value_a;
        m_DH_arrayParam.DH_param_ptr[dof_index][2] = robot_DH.array_DH_parameter[dof_index].value_alpha;
    }

    //测试用
//    for(int i=0;i<m_DH_arrayParam.dof_num;i++){
//        for(int j=0;j<m_DH_arrayParam.param_num;j++){
//            printf("%f,",m_DH_arrayParam.DH_param_ptr[i][j]);
//        }
//        printf("\n");
//    }
}


