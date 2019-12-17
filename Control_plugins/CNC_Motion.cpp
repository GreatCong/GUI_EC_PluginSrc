#include "CNC_Motion.h"

#include "GcodeParser.h"

#define DEFAULT_X_STEPS_PER_UNIT 			800.0f//89.29//86.48f
#define DEFAULT_Y_STEPS_PER_UNIT 			800.0f//88.90//89.29f
#define DEFAULT_Z_STEPS_PER_UNIT 			165.34f//800.0f//158.73f 传送带的传动比165.34f

//原点的设置是三个步进电机的中心位置的交点
//#define BIG_ARM_LENGTH 135				//	@大臂长度	135mm
//#define SMALL_ARM_LENGTH 170			//	@小臂长度	170mm
//#define HEAD_OFFSET   50				//	@头偏移		50mm
//#define CENCER_OFFSET 47				//	@中心偏移	47mm

//#define ARM_HIGH_OFFSET 110 // @ 原点到水平的高度110mm

//#define ARM_OFFSET_X 100 //mm   		//	@X偏移	100mm
//#define ARM_OFFSET_Y -56 //mm			//	@Y偏移	-56mm
//#define ARM_OFFSET_Z -56 //mm			//	@Z偏移	-56mm
//#define ARM_RAD2DEG 57.2957795			//	@转角	57.2957795度

#define BIG_ARM_LENGTH 135				//	@大臂长度	155mm
#define SMALL_ARM_LENGTH 170			//	@小臂长度	170mm
#define HEAD_OFFSET   50.2				//	@头偏移	48.8mm
#define CENCER_OFFSET 62				//	@中心偏移	62mm

//#define MANUAL_X_HOME_POS 90.0f
//#define MANUAL_Y_HOME_POS 0.0f
//#define MANUAL_Z_HOME_POS 0.0f
#define MANUAL_X_HOME_POS 0.0f
#define MANUAL_Y_HOME_POS 0.0f
#define MANUAL_Z_HOME_POS 0.0f

//#define ARM_HIGH_OFFSET 110 // @ 原点到水平的高度110mm

//#define ARM_OFFSET_X 100 //mm   		//	@X偏移	100mm
//#define ARM_OFFSET_Y -56 //mm			//	@Y偏移	-56mm
//#define ARM_OFFSET_Z -56 //mm			//	@Z偏移	-56mm
#define ARM_RAD2DEG 57.2957795			//	@转角	57.2957795度

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
    uint8_t res_state = 0;

    cartesian_theta[AXIS_X] = cartesian[AXIS_X];
    cartesian_theta[AXIS_Y] = cartesian[AXIS_Y];
    cartesian_theta[AXIS_Z] = cartesian[AXIS_Z];
    #if ARM_PEINT_DEBUG
    Printf_MSG("[Arm_Theta=(%f,%f,%f)]\r\n",cartesian_theta[AXIS_X],cartesian_theta[AXIS_Y],cartesian_theta[AXIS_Z]);
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
    uint8_t res_state = 0;

    cartesian[AXIS_X] = cartesian_theta[AXIS_X];
    cartesian[AXIS_Y] = cartesian_theta[AXIS_Y];
    cartesian[AXIS_Z] = cartesian_theta[AXIS_Z];

   #if ARM_PEINT_DEBUG
   Printf_MSG("[Arm_Pos=(%f,%f,%f)]\r\n",cartesian[AXIS_X],cartesian[AXIS_Y],cartesian[AXIS_Z]);
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
        res = DEFAULT_X_STEPS_PER_UNIT;
        break;
    case AXIS_Y:
        res = DEFAULT_Y_STEPS_PER_UNIT;
        break;
    case AXIS_Z:
        res = DEFAULT_Z_STEPS_PER_UNIT;
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

    arm_position_init.arm[AXIS_X] = MANUAL_X_HOME_POS;
    arm_position_init.arm[AXIS_Y] = MANUAL_Y_HOME_POS;
    arm_position_init.arm[AXIS_Z] = MANUAL_Z_HOME_POS;

    calculate_forward(m_PositionInit->arm,arm_position_init.arm);
    #if ARM_PEINT_DEBUG
    //打印初始位置坐标，便于调试
    Printf_MSG("[Arm_PosInit=(%f,%f,%f)]\r\n",m_PositionInit->arm[AXIS_X],m_PositionInit->arm[AXIS_Y],m_PositionInit->arm[AXIS_Z]);
    #endif

    for(int idx = 0; idx < AXIS_N; idx++) { //只用了3轴
         m_sys_position[idx] = std::lround(arm_position_init.arm[idx]*get_StepsPerUnit(idx));
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

    if(mCode == Gcode_segment::COMMENT_CODE || mCode == Gcode_segment::EndParse_CODE){//如果是注释的Gcode，则为空
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
                default:
                    qDebug() << "Mcode:"<<m_Stepper_control->exec_block->Mcode;
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
        *(input_ptr+step_setting) = 1;//只是使能，去掉自动运行的标志
    }

}

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


