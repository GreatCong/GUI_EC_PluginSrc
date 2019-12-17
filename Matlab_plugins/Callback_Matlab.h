#ifndef CALLBACK_MATLAB_H
#define CALLBACK_MATLAB_H

#include "EtherCAT_UserApp.h"

#include "MemShare_Win.h"

typedef struct{
    int16_t test_num;
    int16_t slave_count;
    int16_t renewState;
}EtherCAT_MemParam;

class Callback_Matlab: public QObject,public Ethercat_Callback
{
    Q_OBJECT
public:
    Callback_Matlab(QObject *parent = nullptr);
    virtual ~Callback_Matlab();
public:
    virtual void Master_AppLoop_callback();
    virtual void Master_AppStart_callback();
    virtual void Master_AppStop_callback();
    virtual void Master_AppScan_callback();

    bool Init_cores();
    bool Destroy_cores();
private:
    typedef enum{
       Copy_input_size,
       Copy_output_size,
       Copy_input_begin,
       Copy_input_end,
       Copy_output_begin,
       Copy_output_end
    }Copy_dir_e;

   MemShare_Win m_MemShare_Win_sender;
   EtherCAT_MemParam *m_slave_transferPtr;//模拟扇区，暂且第一个扇区存储变量设置
   const int m_MemParam_offset = 1024;

   int m_copySize[6];
};

#endif // CALLBACK_MATLAB_H
