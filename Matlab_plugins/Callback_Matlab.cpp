#include "Callback_Matlab.h"

#include <QDebug>

#define TEST_MOTOR 0

///
/// \brief 构造函数
/// \param parent
///
Callback_Matlab::Callback_Matlab(QObject *parent) :QObject(parent),Ethercat_Callback()
{
   m_MemShare_Win_sender.MemShare_set_memSize(1024*5);//设置共享内存的大小,暂且设置为5K

   m_slave_transferPtr = nullptr;
}

///
/// \brief 析构函数
///
Callback_Matlab::~Callback_Matlab()
{
   m_MemShare_Win_sender.MemShare_Destroy_cores();
}

///
/// \brief EtherCAT loop
///
void Callback_Matlab::Master_AppLoop_callback()
{
   if(!m_MemShare_Win_sender.MemShare_baseAddr_isRelease()){//模拟扇区，暂且第一个扇区存储变量设置
      m_slave_transferPtr->test_num++;
      m_slave_transferPtr->slave_count = (int16_t)this->Master_getSlaveCount();

//      //copy 系统参数
//      memcpy(m_MemShare_Win_sender.MemShare_get_baseAddr(),m_slave_transferPtr,sizeof(EtherCAT_MemParam));

      if(m_slave_transferPtr->renewState == 0){
          memset(m_Master_addressBase+m_copySize[Copy_output_begin],0,m_copySize[Copy_output_size]);
      }
      else{
          //copy output变量，matlab传入master
          memcpy(m_Master_addressBase+m_copySize[Copy_output_begin],
                 m_MemShare_Win_sender.MemShare_get_baseAddr()+m_MemParam_offset+m_copySize[Copy_output_begin],
                 m_copySize[Copy_output_size]);
          m_slave_transferPtr->renewState = 0;
      }
      //copy input变量，master传入matlab
      memcpy(m_MemShare_Win_sender.MemShare_get_baseAddr()+m_MemParam_offset+m_copySize[Copy_input_begin],
             m_Master_addressBase+m_copySize[Copy_input_begin],
             m_copySize[Copy_input_size]);

#if TEST_MOTOR
      *((uint16_t*)(m_Master_addressBase + 4)) = 1500;

      *((uint16_t*)(m_Master_addressBase + 2)) = 1;
      *((uint32_t*)(m_Master_addressBase + 8)) = 1;

      qDebug() << "setting=" << *((uint16_t*)(m_Master_addressBase + 2));
      qDebug() << "motor1=" << *((uint32_t*)(m_Master_addressBase + 8));
#endif

   }
}

///
/// \brief EtherCAT start
///
void Callback_Matlab::Master_AppStart_callback()
{
   m_MemShare_Win_sender.MemShare_Init_cores();
   m_slave_transferPtr = (EtherCAT_MemParam*)(m_MemShare_Win_sender.MemShare_get_baseAddr());//前1024个数据块用于保存系统数据
   //NOTE:Master中,AI输入变量在后，电机控制变量在前
   m_copySize[Copy_input_begin] = this->Master_getAddressList().at(0).inputs_offset;
   m_copySize[Copy_input_end] = this->Master_getAddressList().at(this->Master_getSlaveCount()-1).inputs_offset +
                                (this->Master_getAddressList().at(this->Master_getSlaveCount()-1).inputs_Bits/2)/4;

   m_copySize[Copy_output_begin] = this->Master_getAddressList().at(0).outputs_offset;
   m_copySize[Copy_output_end] = m_copySize[Copy_input_begin];

   m_copySize[Copy_input_size] = m_copySize[Copy_input_end] - m_copySize[Copy_input_begin];
   m_copySize[Copy_output_size] = m_copySize[Copy_output_end] - m_copySize[Copy_output_begin];

//   for(int i=0;i<6;i++){
//       qDebug() << m_copySize[i] << ",";
//   }

   m_slave_transferPtr->test_num = 0;
}

///
/// \brief EtherCAT Stop
///
void Callback_Matlab::Master_AppStop_callback()
{
    if(!m_MemShare_Win_sender.MemShare_baseAddr_isRelease()){
        m_slave_transferPtr->test_num = 0;
        m_slave_transferPtr->slave_count = 0;
        memcpy(m_MemShare_Win_sender.MemShare_get_baseAddr(),m_slave_transferPtr,sizeof(EtherCAT_MemParam));

        m_slave_transferPtr = nullptr;
    }

   m_MemShare_Win_sender.MemShare_Destroy_cores();
}

///
/// \brief EtherCAT Scan
///
void Callback_Matlab::Master_AppScan_callback()
{

}

///
/// \brief 初始化应用
/// \return
///
bool Callback_Matlab::Init_cores()
{
    if(m_MemShare_Win_sender.MemShare_Init_cores() == false){//测试下内存是否存在
        return false;
    }
    else{
        m_MemShare_Win_sender.MemShare_Destroy_cores();
    }

    return true;
}

///
/// \brief 销毁应用
/// \return
///
bool Callback_Matlab::Destroy_cores()
{
    return true;
}
