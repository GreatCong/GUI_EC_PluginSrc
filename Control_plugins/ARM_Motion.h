#ifndef ARM_MOTION_H
#define ARM_MOTION_H

#include <stdint.h>
#include <QDebug>
#include <cmath>

//#define ARM_PEINT_DEBUG 1
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

    virtual uint8_t calculate_arm(float *cartesian_theta,const float *cartesian) = 0;
    virtual uint8_t calculate_forward(float *cartesian,const float *cartesian_theta) = 0;

};

#endif // ARM_MOTION_H
