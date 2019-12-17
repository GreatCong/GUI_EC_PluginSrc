#include "DRE2008_OS_Callback.h"
#include <QDebug>

//int slave_num = 1;
//int16_t Normal_AD_Input[8] = { 0,0,0,0,0,0,0,0 };
char file[500]; //需要保存数据的文件


DRE2008_OS_Callback::DRE2008_OS_Callback(QObject *parent) : QObject(parent),Ethercat_Callback()
{
    m_isOS_Reset = false;//超采样初始化
    m_isOS_Run = false;
    pre_OverSampling_CycleCount = 0;

    output_ptr = nullptr;
    input_ptr = nullptr;

    m_measure_Q = new QQueue<int16_t>();
    m_OS_Channel = OS_CH1; //超采样通道设置
    m_SamplingRate = 20; //采样率设置(最大40Khz)
    m_AD_Channel = AD_CH1;

    m_ErrState = Error_None;

    memset(m_Normal_AD_Input,0,sizeof(m_Normal_AD_Input));
}

void DRE2008_OS_Callback::Master_AppLoop_callback()
{
    static int reset_num = 0;

    if(m_isOS_Run){
        if(!m_isOS_Reset){//这里貌似是需要按顺序执行，稍后再改
            //超采样设置
            if(reset_num < 3){//先Reset，再进行其他操作
                reset_num++;
//                DRE2008_OS_Reset(slave_num); //DRE2008-OS复位(禁止超采样)
//                pre_OverSampling_CycleCount = 0;
                DRE2008_OS_Reset(output_ptr); //DRE2008-OS复位(禁止超采样)
                pre_OverSampling_CycleCount = 0;
            }
            else{
                DRE2008_OS_SamplingRateSet(output_ptr, m_SamplingRate); //设置采样率

                DRE2008_OS_Enable(output_ptr, m_OS_Channel); //设置超采样通道并使能超采样

                m_isOS_Reset = true;
                reset_num = 0;
            }

        }
        else{
            DRE2008_OS_NormalInputRead(input_ptr, m_Normal_AD_Input); //普通模式数据采集
//            qDebug() << Normal_AD_Input[0];
            m_ErrState = DRE2008_OS_OverSamplingInputLog(input_ptr, m_OS_Channel, 0, file, m_AD_Channel); //超采样数据log

//            if(m_ErrState != Error_None){
//               qDebug() << m_ErrState;
//            }

        }

    }
    else{

            pre_OverSampling_CycleCount = 0;
            DRE2008_OS_SamplingRateSet(output_ptr, 50);
            DRE2008_OS_Disable(output_ptr);
    //        printf("required %d data log complete!\n", num_of_samples);
    //        TestKillTimer(1);
    }


}

void DRE2008_OS_Callback::Master_AppStart_callback()
{
//    qDebug() << "Index1:" << this->Master_getSlaveChooseIndex();

    output_ptr = (uint8_t *)(m_Master_addressBase + this->Master_getAddressList().at(this->Master_getSlaveChooseIndex()).outputs_offset);//这里需要根据扫描到的地址替换
    input_ptr = (uint8_t *)(m_Master_addressBase + this->Master_getAddressList().at(this->Master_getSlaveChooseIndex()).inputs_offset);

    m_isOS_Reset = false;//超采样初始化
    pre_OverSampling_CycleCount = 0;//超采样获取中的参数
    m_isOS_Run = false;

//    initQueue(&my_ecQueue.my_ecQueue_ch1);
    m_measure_Q->clear();

    emit Master_RunStateChanged(true);
}

void DRE2008_OS_Callback::Master_AppStop_callback()
{
    m_isOS_Reset = false;//超采样初始化
    m_isOS_Run = false;
//    clearQueue(&my_ecQueue.my_ecQueue_ch1);
    m_measure_Q->clear();

    emit Master_RunStateChanged(false);
}

void DRE2008_OS_Callback::Master_AppScan_callback()
{
    emit Sig_MasterScanChanged();
}

void DRE2008_OS_Callback::Master_ReleaseAddress()
{
    m_Master_addressBase = NULL;
    output_ptr = NULL;
    input_ptr = NULL;
}

int DRE2008_OS_Callback::Master_setAdressBase(char *address)
{
    m_Master_addressBase = address;
    return 0;
}

void DRE2008_OS_Callback::Set_OverSampling_run(bool isRun)
{
    m_isOS_Run = isRun;
}

void DRE2008_OS_Callback::Set_OverSampling_Reset(bool isReset)
{
    m_isOS_Reset = isReset;
}

const QString DRE2008_OS_Callback::ErrorState_ToString()
{
    QString str;

    switch(m_ErrState){
      case Error_None:
        str = "Error_None";
        break;
      case Error_NoFile:
        str = "Error_NoFile";
        break;
      case Error_File_WriteOver:
        str = "Error_File_WriteOver";
        break;
      case Error_TimeOut:
        str = "Error_TimeOut";
        break;
      case Error_Channel_Invalid:
        str = "Error_Channel_Invalid";
        break;
      default:
        str = "ErrorCode:"+QString::number(m_ErrState);
        break;
    }

    return str;
}

/* DRE2008 */
void DRE2008_OS_Callback::DRE2008_OS_Reset(uint8_t *data_OutPtr)
{
//    int i = 0;
    DRE2008_OS_SamplingRateSet(data_OutPtr, 50);
    DRE2008_OS_Disable(data_OutPtr);

//    while(i < 50)
//    {
//        ec_send_processdata();
//        ec_receive_processdata(EC_TIMEOUTRET);
//        osal_usleep(2000);
//        i++;
//    }

//    printf("RESET OVER......\n");
//    qDebug() << "RESET OVER......";
}

void DRE2008_OS_Callback::DRE2008_OS_SamplingRateSet(uint8_t *data_Outptr, int SamplingRate)
{
    uint8_t *data_out;
//    data_out = ec_slave[slave_num].outputs;
    data_out = data_Outptr;

    *data_out++ = SamplingRate & 0xFF;
    *data_out++ = SamplingRate >> 8;
}

void DRE2008_OS_Callback::DRE2008_OS_Enable(uint8_t *data_Outptr, int channel)
{
    uint8_t *data_out;
//    data_out = ec_slave[slave_num].outputs;
    data_out = data_Outptr;

    *data_out++;
    *data_out++;
    *data_out++ = channel & 0xFF;
    *data_out++ = channel >> 8;
}

void DRE2008_OS_Callback::DRE2008_OS_Disable(uint8_t *data_Outptr)
{
    uint8_t *data_out;
    int channel = OS_NONE;
//    data_out = ec_slave[slave_num].outputs;
    data_out = data_Outptr;

    *data_out++;
    *data_out++;
    *data_out++ = channel & 0xFF;
    *data_out++ = channel >> 8;
}

void DRE2008_OS_Callback::DRE2008_OS_NormalInputRead(uint8_t *data_Inptr, int16_t *NormalInput)
{
    uint8_t *data_in;
    int16_t temp1, temp2;
//    data_in = ec_slave[slave_num].inputs;
    data_in = data_Inptr;

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[0] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[1] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[2] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[3] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[4] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[5] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[6] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    NormalInput[7] = temp1 + (temp2 << 8);
}

void DRE2008_OS_Callback::DRE2008_OS_OverSamplingInputRead(uint8_t *data_Inptr, int32_t *CycleCount, int16_t *OverSamplingInput)
{
    uint8_t *data_in;
    int16_t temp1, temp2, temp3, temp4;
//    data_in = ec_slave[slave_num].inputs;
    data_in = data_Inptr;

    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;
    *data_in++;

    temp1 = *data_in++;
    temp2 = *data_in++;
    temp3 = *data_in++;
    temp4 = *data_in++;
    *CycleCount = temp1 + (temp2 << 8) + (temp3 << 16) + (temp4 << 24);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[0] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[1] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[2] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[3] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[4] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[5] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[6] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[7] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[8] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[9] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[10] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[11] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[12] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[13] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[14] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[15] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[16] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[17] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[18] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[19] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[20] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[21] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[22] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[23] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[24] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[25] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[26] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[27] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[28] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[29] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[30] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[31] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[32] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[33] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[34] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[35] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[36] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[37] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[38] = temp1 + (temp2 << 8);

    temp1 = *data_in++;
    temp2 = *data_in++;
    OverSamplingInput[39] = temp1 + (temp2 << 8);
}

int DRE2008_OS_Callback::DRE2008_OS_OverSamplingInputLog(uint8_t *data_Inptr, int channel, int num_of_samples, char *file, int dis_channel)
{
    //    FILE *fpWrite;
        (void*)(&num_of_samples);
        (void*)file;

        int32_t OverSampling_CycleCount = 0;
        int16_t OverSampling_AD_Input[40] = { 0 };
    //    static int32_t pre_OverSampling_CycleCount = 0;
        int circlecount_error = 3;//当前周期读取到的circlrcount与上一周期的差值，理论为1，将该值设置大于1可提高容错率但会造成部分数据丢失
        int ret = Error_None;
    //    int channelDis_temp = 0;
    //    channelDis_temp = dis_channel -1;//C语言数组从0开始，而参数是从1开始
        int channelDis_temp = dis_channel;//dis_channel本身从0开始

        DRE2008_OS_OverSamplingInputRead(data_Inptr, &OverSampling_CycleCount, OverSampling_AD_Input);//读取40个数据

        switch (channel)
        {
            case OS_CH1://CH1超采样
            {
                if((OverSampling_CycleCount > pre_OverSampling_CycleCount) && ((OverSampling_CycleCount - pre_OverSampling_CycleCount) < circlecount_error + 1))
                {
    //                printf("CycleCount: %d   PreCycleCount: %d\n", OverSampling_CycleCount, pre_OverSampling_CycleCount);
                    pre_OverSampling_CycleCount = OverSampling_CycleCount;
    //                fpWrite = fopen(file, "a+");

    //                if (fpWrite == NULL)
    //                {
    //                    return Error_NoFile;
    //                }
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 1, OverSampling_AD_Input[0]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 2, OverSampling_AD_Input[1]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 3, OverSampling_AD_Input[2]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 4, OverSampling_AD_Input[3]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 5, OverSampling_AD_Input[4]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 6, OverSampling_AD_Input[5]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 7, OverSampling_AD_Input[6]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 8, OverSampling_AD_Input[7]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 9, OverSampling_AD_Input[8]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 10, OverSampling_AD_Input[9]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 11, OverSampling_AD_Input[10]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 12, OverSampling_AD_Input[11]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 13, OverSampling_AD_Input[12]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 14, OverSampling_AD_Input[13]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 15, OverSampling_AD_Input[14]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 16, OverSampling_AD_Input[15]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 17, OverSampling_AD_Input[16]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 18, OverSampling_AD_Input[17]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 19, OverSampling_AD_Input[18]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 20, OverSampling_AD_Input[19]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 21, OverSampling_AD_Input[20]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 22, OverSampling_AD_Input[21]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 23, OverSampling_AD_Input[22]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 24, OverSampling_AD_Input[23]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 25, OverSampling_AD_Input[24]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 26, OverSampling_AD_Input[25]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 27, OverSampling_AD_Input[26]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 28, OverSampling_AD_Input[27]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 29, OverSampling_AD_Input[28]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 30, OverSampling_AD_Input[29]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 31, OverSampling_AD_Input[30]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 32, OverSampling_AD_Input[31]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 33, OverSampling_AD_Input[32]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 34, OverSampling_AD_Input[33]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 35, OverSampling_AD_Input[34]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 36, OverSampling_AD_Input[35]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 37, OverSampling_AD_Input[36]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 38, OverSampling_AD_Input[37]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 39, OverSampling_AD_Input[38]);
    //                fprintf(fpWrite, "%d  %d\n", (OverSampling_CycleCount - 1) * 40 + 40, OverSampling_AD_Input[39]);
    //                fclose(fpWrite);

                    if(channelDis_temp < 1){
                        for(int index=0,loop_num = 0;loop_num < 40;index++,loop_num++){
                            m_measure_Q->enqueue(OverSampling_AD_Input[index]);
//                            enQueue(&my_ecQueue.my_ecQueue_ch1, OverSampling_AD_Input[index]);
//                            qDebug() << OverSampling_AD_Input[index];
        //                    enQueue(&my_ecQueue.my_ecQueue_ch2, OverSampling_AD_Input[index]);
                        }
                    }
                    else {
                        ret = Error_Channel_Invalid;//通道不匹配
                    }



    //                if (((OverSampling_CycleCount - 1) * 40 + 40) >= num_of_samples)
    //                {
    //                    pre_OverSampling_CycleCount = 0;
    //                    DRE2008_OS_SamplingRateSet(slave_num, 50);
    //                    DRE2008_OS_Disable(slave_num);
    //                    printf("required %d data log complete!\n", num_of_samples);
    //                    ret = Error_File_WriteOver;//表示满了
    //                }
                }
                else{
                    ret = Error_TimeOut;
                }
                break;
            }
            case OS_CH1_CH2://CH1-CH2超采样
            {
                if ((OverSampling_CycleCount > pre_OverSampling_CycleCount) && ((OverSampling_CycleCount - pre_OverSampling_CycleCount) < circlecount_error + 1))
                {
    //                printf("CycleCount: %d   PreCycleCount: %d\n", OverSampling_CycleCount, pre_OverSampling_CycleCount);
                    pre_OverSampling_CycleCount = OverSampling_CycleCount;
    //                fpWrite = fopen(file, "a+");
    //                if (fpWrite == NULL)
    //                {
    //                    return Error_NoFile;
    //                }
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 1, OverSampling_AD_Input[0], OverSampling_AD_Input[20]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 2, OverSampling_AD_Input[1], OverSampling_AD_Input[21]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 3, OverSampling_AD_Input[2], OverSampling_AD_Input[22]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 4, OverSampling_AD_Input[3], OverSampling_AD_Input[23]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 5, OverSampling_AD_Input[4], OverSampling_AD_Input[24]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 6, OverSampling_AD_Input[5], OverSampling_AD_Input[25]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 7, OverSampling_AD_Input[6], OverSampling_AD_Input[26]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 8, OverSampling_AD_Input[7], OverSampling_AD_Input[27]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 9, OverSampling_AD_Input[8], OverSampling_AD_Input[28]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 10, OverSampling_AD_Input[9], OverSampling_AD_Input[29]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 11, OverSampling_AD_Input[10], OverSampling_AD_Input[30]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 12, OverSampling_AD_Input[11], OverSampling_AD_Input[31]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 13, OverSampling_AD_Input[12], OverSampling_AD_Input[32]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 14, OverSampling_AD_Input[13], OverSampling_AD_Input[33]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 15, OverSampling_AD_Input[14], OverSampling_AD_Input[34]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 16, OverSampling_AD_Input[15], OverSampling_AD_Input[35]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 17, OverSampling_AD_Input[16], OverSampling_AD_Input[36]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 18, OverSampling_AD_Input[17], OverSampling_AD_Input[37]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 19, OverSampling_AD_Input[18], OverSampling_AD_Input[38]);
    //                fprintf(fpWrite, "%d  %d  %d\n", (OverSampling_CycleCount - 1) * 20 + 20, OverSampling_AD_Input[19], OverSampling_AD_Input[39]);
    //                fclose(fpWrite);

    //                if (((OverSampling_CycleCount - 1) * 20 + 20) >= num_of_samples)
    //                {
    //                    pre_OverSampling_CycleCount = 0;
    //                    DRE2008_OS_SamplingRateSet(slave_num, 50);
    //                    DRE2008_OS_Disable(slave_num);
    //                    printf("required %d data log complete!\n", num_of_samples);
    //                    ret = Error_File_WriteOver;
    //                }

                    if(channelDis_temp < 2){
                        for(int index = channelDis_temp*20,loop_num = 0;loop_num < 20;index++,loop_num++){
//                            enQueue(&my_ecQueue.my_ecQueue_ch1, OverSampling_AD_Input[index]);
                            m_measure_Q->enqueue(OverSampling_AD_Input[index]);
        //                    enQueue(&my_ecQueue.my_ecQueue_ch2, OverSampling_AD_Input[index]);
                        }
                    }
                    else {
                        ret = Error_Channel_Invalid;
                    }
                }
                else{
                    ret = Error_TimeOut;
                }
                break;
            }
            case OS_CH1_CH4://CH1-CH4超采样
            {
                if ((OverSampling_CycleCount > pre_OverSampling_CycleCount) && ((OverSampling_CycleCount - pre_OverSampling_CycleCount) < circlecount_error + 1))
                {
    //                printf("CycleCount: %d   PreCycleCount: %d\n", OverSampling_CycleCount, pre_OverSampling_CycleCount);
                    pre_OverSampling_CycleCount = OverSampling_CycleCount;
    //                fpWrite = fopen(file, "a+");
    //                if (fpWrite == NULL)
    //                {
    //                    return -1;
    //                }
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 1, OverSampling_AD_Input[0], OverSampling_AD_Input[10], \
    //                    OverSampling_AD_Input[20], OverSampling_AD_Input[30]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 2, OverSampling_AD_Input[1], OverSampling_AD_Input[11], \
    //                    OverSampling_AD_Input[21], OverSampling_AD_Input[31]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 3, OverSampling_AD_Input[2], OverSampling_AD_Input[12], \
    //                    OverSampling_AD_Input[22], OverSampling_AD_Input[32]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 4, OverSampling_AD_Input[3], OverSampling_AD_Input[13], \
    //                    OverSampling_AD_Input[23], OverSampling_AD_Input[33]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 5, OverSampling_AD_Input[4], OverSampling_AD_Input[14], \
    //                    OverSampling_AD_Input[24], OverSampling_AD_Input[34]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 6, OverSampling_AD_Input[5], OverSampling_AD_Input[15], \
    //                    OverSampling_AD_Input[25], OverSampling_AD_Input[35]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 7, OverSampling_AD_Input[6], OverSampling_AD_Input[16], \
    //                    OverSampling_AD_Input[26], OverSampling_AD_Input[36]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 8, OverSampling_AD_Input[7], OverSampling_AD_Input[17], \
    //                    OverSampling_AD_Input[27], OverSampling_AD_Input[37]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 9, OverSampling_AD_Input[8], OverSampling_AD_Input[18], \
    //                    OverSampling_AD_Input[28], OverSampling_AD_Input[38]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 10 + 10, OverSampling_AD_Input[9], OverSampling_AD_Input[19], \
    //                    OverSampling_AD_Input[29], OverSampling_AD_Input[39]);
    //                fclose(fpWrite);

                    if(channelDis_temp < 4){
                        for(int index = channelDis_temp*10,loop_num = 0;loop_num < 10;index++,loop_num++){
//                            enQueue(&my_ecQueue.my_ecQueue_ch1, OverSampling_AD_Input[index]);
                            m_measure_Q->enqueue(OverSampling_AD_Input[index]);
        //                    enQueue(&my_ecQueue.my_ecQueue_ch2, OverSampling_AD_Input[index]);
                        }
                    }
                    else{
                        ret = Error_Channel_Invalid;
                    }

    //                if (((OverSampling_CycleCount - 1) * 10 + 10) >= num_of_samples)
    //                {
    //                    pre_OverSampling_CycleCount = 0;
    //                    DRE2008_OS_SamplingRateSet(slave_num, 50);
    //                    DRE2008_OS_Disable(slave_num);
    //                    printf("required %d data log complete!\n", num_of_samples);

    //                    ret = Error_File_WriteOver;
    //                }

                }
                else{
                    ret = Error_TimeOut;
                }
                break;
            }
            case OS_CH1_CH8://CH1-CH8超采样
            {
                if ((OverSampling_CycleCount > pre_OverSampling_CycleCount) && ((OverSampling_CycleCount - pre_OverSampling_CycleCount) < circlecount_error + 1))
                {
    //                printf("CycleCount: %d   PreCycleCount: %d\n", OverSampling_CycleCount, pre_OverSampling_CycleCount);
                    pre_OverSampling_CycleCount = OverSampling_CycleCount;
    //                fpWrite = fopen(file, "a+");
    //                if (fpWrite == NULL)
    //                {
    //                    return Error_NoFile;
    //                }
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 5 + 1, OverSampling_AD_Input[0], OverSampling_AD_Input[5], \
    //                    OverSampling_AD_Input[10], OverSampling_AD_Input[15], \
    //                    OverSampling_AD_Input[20], OverSampling_AD_Input[25], \
    //                    OverSampling_AD_Input[30], OverSampling_AD_Input[35]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 5 + 2, OverSampling_AD_Input[1], OverSampling_AD_Input[6], \
    //                    OverSampling_AD_Input[11], OverSampling_AD_Input[16], \
    //                    OverSampling_AD_Input[21], OverSampling_AD_Input[26], \
    //                    OverSampling_AD_Input[31], OverSampling_AD_Input[36]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 5 + 3, OverSampling_AD_Input[2], OverSampling_AD_Input[7], \
    //                    OverSampling_AD_Input[12], OverSampling_AD_Input[17], \
    //                    OverSampling_AD_Input[22], OverSampling_AD_Input[27], \
    //                    OverSampling_AD_Input[32], OverSampling_AD_Input[37]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 5 + 4, OverSampling_AD_Input[3], OverSampling_AD_Input[8], \
    //                    OverSampling_AD_Input[13], OverSampling_AD_Input[18], \
    //                    OverSampling_AD_Input[23], OverSampling_AD_Input[28], \
    //                    OverSampling_AD_Input[33], OverSampling_AD_Input[38]);
    //                fprintf(fpWrite, "%d  %d  %d  %d  %d  %d  %d  %d  %d\n", (OverSampling_CycleCount - 1) * 5 + 5, OverSampling_AD_Input[4], OverSampling_AD_Input[9], \
    //                    OverSampling_AD_Input[14], OverSampling_AD_Input[19], \
    //                    OverSampling_AD_Input[24], OverSampling_AD_Input[29], \
    //                    OverSampling_AD_Input[34], OverSampling_AD_Input[39]);
    //                fclose(fpWrite);

                    if(channelDis_temp < 8){
                        for(int index = channelDis_temp*5,loop_num = 0;loop_num < 5;index++,loop_num++){
//                            enQueue(&my_ecQueue.my_ecQueue_ch1, OverSampling_AD_Input[index]);
                            m_measure_Q->enqueue(OverSampling_AD_Input[index]);
        //                    enQueue(&my_ecQueue.my_ecQueue_ch2, OverSampling_AD_Input[index]);
                        }
                    }
                    else{
                        ret = Error_Channel_Invalid;
                    }
    //                if (((OverSampling_CycleCount - 1) * 5 + 5) >= num_of_samples)
    //                {
    //                    pre_OverSampling_CycleCount = 0;
    //                    DRE2008_OS_SamplingRateSet(slave_num, 50);
    //                    DRE2008_OS_Disable(slave_num);
    //                    printf("The required %d data log complete!\n", num_of_samples);

    //                    ret = Error_File_WriteOver;
    //                }


                }
                else{
                    ret = Error_TimeOut;
                }
                break;
            }
            default:
                break;
        }

        return ret;
}


