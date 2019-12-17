#ifndef DRE2008_OS_CALLBACK_H
#define DRE2008_OS_CALLBACK_H

#include <QObject>
#include "../../QT_VS_Dock/Plugin_common/EtherCAT_UserApp.h"

//#include "my_queue.h"
#include <QQueue>

class DRE2008_OS_Callback : public QObject,public Ethercat_Callback
{
    Q_OBJECT
public:
    explicit DRE2008_OS_Callback(QObject *parent = nullptr);
public:
    virtual void Master_AppLoop_callback();
    virtual void Master_AppStart_callback();
    virtual void Master_AppStop_callback();
    virtual void Master_AppScan_callback();

    virtual void Master_ReleaseAddress();
    virtual int Master_setAdressBase(char* address);

    typedef enum{
       OS_NONE,
       OS_CH1,
       OS_CH1_CH2,
       OS_CH1_CH4,
       OS_CH1_CH8
    }OS_CH_Choose;

    typedef enum{
        AD_CH1,
        AD_CH2,
        AD_CH3,
        AD_CH4,
        AD_CH5,
        AD_CH6,
        AD_CH7,
        AD_CH8
    }AD_CH_Choose;

    typedef enum{
        Error_None = 0,
        Error_NoFile = -1,
        Error_File_WriteOver = -2,
        Error_TimeOut = -3,
        Error_Channel_Invalid = -4,
    }SampleOS_Error;

//    my_ec_data_Q_s my_ecQueue;
     QQueue<int16_t> *m_measure_Q;
    void Set_OverSampling_run(bool isRun);
    void Set_OverSampling_Reset(bool isReset);
    const QString ErrorState_ToString();
    int m_OS_Channel; //超采样通道设置
    int m_SamplingRate; //采样率设置(最大40Khz)
    int m_AD_Channel;

private:
    bool m_isOS_Reset;//超采样初始化
    bool m_isOS_Run;
    int32_t pre_OverSampling_CycleCount;
    int m_ErrState;

    int16_t m_Normal_AD_Input[8];

    uint8_t *output_ptr;
    uint8_t *input_ptr;

    void DRE2008_OS_Reset(uint8_t *data_OutPtr);//复位DRE2008使其进入普通模式(无超采样)
    void DRE2008_OS_SamplingRateSet(uint8_t *data_Outptr, int SamplingRate);//设置采样率(1-50KHZ)
    void DRE2008_OS_Enable(uint8_t *data_Outptr, int channel);//设置超采样通道(0-4)并使能超采样
    void DRE2008_OS_Disable(uint8_t *data_Outptr);//禁止超采样
    void DRE2008_OS_NormalInputRead(uint8_t *data_Inptr, int16_t *NormalInput);//普通模式输入读取
    void DRE2008_OS_OverSamplingInputRead(uint8_t *data_Inptr, int32_t *CycleCount, int16_t *OverSamplingInput);//超采样模式输入读取(每次40个数据点)
    int DRE2008_OS_OverSamplingInputLog(uint8_t *data_Inptr, int channel, int num_of_samples, char* file, int dis_channel);//超采样数据记录写入txt,可设置通道、采集的点数和文件路径
    //void DRE2008_OS_CreatFile(int slave_num, int channel, int SamplingRate, int num_of_samples, char* file_folder, char* file);//根据配置信息创建文件用来保存数据
signals:
    void Master_RunStateChanged(bool isRun);
    void Sig_MasterScanChanged();
public slots:
};

#endif // DRE2008_OS_CALLBACK_H
