﻿#ifndef ARM_MOTION_H
#define ARM_MOTION_H

#include <stdint.h>
#include <QDebug>
#include <cmath>

#include "algorithm_common.h"

#define ARM_PEINT_DEBUG 0
#define Printf_MSG qDebug

#define STEP_BIT(n) 					(1 << n) 				//位mask
#define STEP_BIT_SetTrue(x,mask) 	(x |= mask)				//该位设置为真
#define STEP_BIT_SetFalse(x,mask)	(x &= ~mask)			//该位设置为假
#define STEP_BIT_Toggle(x,mask) 		(x ^= mask)				//位切换
#define STEP_BIT_IsTrue(x,mask) 		((x & mask) != 0)		//该位是否真
#define STEP_BIT_IsFalse(x,mask) 	((x & mask) == 0)		//该位是否假

typedef struct ARM_Struct
{
    float *arm;
    uint8_t transfer_state;
    int n_axis;
    ARM_Struct(int n) {
        n_axis = n;
        transfer_state = 0;
        arm = new float[n_axis];
        for(int i=0;i<n_axis;i++){
            arm[i] = 0;
        }
    }

    ARM_Struct(){
        ARM_Struct(3);//默认的是三轴
    }
}ARM_Struct;

typedef struct Stepper_block
{
  uint32_t *Axis_steps;
  uint32_t step_event_count;//最大步长
  uint16_t direction_bits;
  int Mcode;
  double speed_step;
  double speed_spindle;
  int n_axis;
  Stepper_block(int n){
      n_axis = n;
      Axis_steps = new uint32_t[n_axis];
      for(int i=0;i<n_axis;i++){
          Axis_steps[i] = 0;
      }
      step_event_count = 0;
      direction_bits = 0;
      Mcode = 0;
      speed_step = 0;
      speed_spindle = 0;
  }

  Stepper_block(){
      Stepper_block(3);//默认的是三轴
  }
}Stepper_block;

typedef Stepper_block* Stepper_block_t;

typedef struct Stepper_control
{
  uint32_t *Axis_steps;
  uint32_t *counter;
  uint32_t step_count;
  uint16_t step_outbits;//支持16轴
  uint16_t dir_outbits;

  uint16_t exec_block_index;
  Stepper_block *exec_block;
  int n_axis;
  Stepper_control(int n){
      n_axis = n;
      counter = new uint32_t[n_axis];
      Axis_steps = new uint32_t[n_axis];
      for(int i=0;i<n_axis;i++){
          counter[i] = 0;
          Axis_steps[i] = 0;
      }
      step_count = 0;
      step_outbits = 0;
      dir_outbits = 0;
      exec_block_index = 0;
      exec_block = NULL;
  }

  Stepper_control(){
      Stepper_control(3);//默认的是三轴
  }
}Stepper_control;

class ARM_Motion
{
public:
    ARM_Motion();
public:
    float return_angle(float a, float b, float c);
    uint8_t check_angle(float a, float b, float c);

    virtual uint8_t calculate_arm(float *cartesian_theta,const float *cartesian){
        if(m_algorithm_func.is_load){
            return (*(m_algorithm_func.calculate_arm_dll))(cartesian_theta,cartesian,NULL,0,0);
        }

        return 0;
    }

    virtual uint8_t calculate_forward(float *cartesian,const float *cartesian_theta){
        if(m_algorithm_func.is_load){
            return (*(m_algorithm_func.calculate_forward_dll))(cartesian,cartesian_theta,NULL,0,0);
        }

        return 0;
    }

    virtual void init_cores(){
        m_algorithm_func.calculate_arm_dll = nullptr;
        m_algorithm_func.calculate_forward_dll = nullptr;
        m_algorithm_func.is_load = false;
        m_algorithm_func.model_name = "";
    }

    virtual void destroy_cores(){
        m_algorithm_func.calculate_arm_dll = nullptr;
        m_algorithm_func.calculate_forward_dll = nullptr;
        m_algorithm_func.is_load = false;
        m_algorithm_func.model_name = "";
    }

    virtual void set_function_ptr(const Algorithm_dll_t algorithm_func){
        m_algorithm_func.calculate_arm_dll = algorithm_func.calculate_arm_dll;
        m_algorithm_func.calculate_forward_dll =algorithm_func.calculate_forward_dll;
        m_algorithm_func.is_load = algorithm_func.is_load;
        m_algorithm_func.model_name = algorithm_func.model_name;
    }

    virtual void set_machine_param(const Machine_parameter_t machine_param){
        m_machine_parameter = machine_param;
    }

    virtual QString get_model_name(){ return m_algorithm_func.model_name;}
    virtual void set_slave_num(int num){ m_slave_num = num;}//从0开始
    virtual int get_slave_num(){return m_slave_num;}

protected:
        Algorithm_dll_t m_algorithm_func;
        Machine_parameter_t m_machine_parameter;
        int m_slave_num;
};

#endif // ARM_MOTION_H
