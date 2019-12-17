#include "DRE2008OS_plugins.h"
#include <QGridLayout>
#include <QPushButton>

#include "My_FFT.h"

//#include "my_queue.h"

DRE2008OS_plugin::DRE2008OS_plugin(QObject *parent) :
    QObject(parent),EtherCAT_UserApp()
{
    //NOTE:如果要设置图标，在这个位置设置
    this->set_AppName("DRE2008 example");
    QPixmap pixmap(":/Controls/Resource/Control/Measure_icon.png");
    this->set_AppIcon(QIcon(pixmap));
}

DRE2008OS_plugin::~DRE2008OS_plugin(){
    //NOTE:用get_UIWidgetPtr函数，不能用user_form_controlTab这样的
    if(this->get_UIWidgetPtr()){
       delete this->get_UIWidgetPtr();
    }

    if(this->get_CallbackPtr()){
        delete this->get_CallbackPtr();
    }

    if(this->get_MessageObj()){
        delete this->get_MessageObj();
    }
}

bool DRE2008OS_plugin::Init_Object()
{
    m_userWidget = new Form_plot();
    m_userCallback = new DRE2008_OS_Callback();
    m_userMessage = new AppUser_Message();

    set_UIWidgetPtr(m_userWidget);
    set_CallbackPtr(m_userCallback);
    set_MessageObj(m_userMessage);

    return true;
}

EtherCAT_UserApp *DRE2008OS_plugin::get_NewAppPtr(QObject *parent)
{
    return new DRE2008OS_plugin(parent);
}

int DRE2008OS_plugin::Check_SampleParameter(int OS_ch,int AD_ch,int sampleRate)
{
    int ret = 0;

    switch(OS_ch){
    case DRE2008_OS_Callback::OS_CH1:
        if(AD_ch > DRE2008_OS_Callback::AD_CH1){
            QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("AD Channel is invalid!"));
            ret = -1;
        }
        break;
    case DRE2008_OS_Callback::OS_CH1_CH2:
        if(AD_ch > DRE2008_OS_Callback::AD_CH2){
            QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("AD Channel is invalid!"));
            ret = -1;
        }
        else{
            if(sampleRate > 20){
                QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("SampleRate is invalid!"));
                ret = -2;
            }
        }
        break;
    case DRE2008_OS_Callback::OS_CH1_CH4:
        if(AD_ch > DRE2008_OS_Callback::AD_CH4){
            QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("AD Channel is invalid!"));
            ret = -1;
        }
        else{
            if(sampleRate > 10){
                QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("SampleRate is invalid!"));
                ret = -2;
            }
        }
        break;
    case DRE2008_OS_Callback::OS_CH1_CH8:
//        if(m_userCallback->m_AD_Channel > DRE2008_OS_Callback::AD_CH8){
//            QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("AD Channel is invalid!"));
//        }
        if(sampleRate > 5){
            QMessageBox::warning(get_UIWidgetPtr(),tr("Warning"),tr("SampleRate is invalid!"));
            ret = -2;
        }
        break;
    case DRE2008_OS_Callback::OS_NONE:
        break;
    default:
        break;

    }

    return ret;
}

bool DRE2008OS_plugin::Init_messageSlots()
{
    connect(m_userMessage,SIGNAL(sig_isAppRun_Changed(bool)),this,SLOT(messageSig_isAppRun_Changed(bool)));
    connect(m_userMessage,SIGNAL(sig_isHideRight_Changed(bool)),this,SLOT(messageSig_isHideRight_Changed(bool)));
    connect(m_userMessage,SIGNAL(sig_displayNum_Changed(int)),this,SLOT(messageSig_displayNum_Changed(int)));

    return true;
}

bool DRE2008OS_plugin::Init_Cores()
{
    m_plotDisplay_Num = 1024;
    m_isMaster_Run = false;
    m_isDisFFT_reset = false;

    m_timePlot = new QTimer();
    connect(m_timePlot,SIGNAL(timeout()),this,SLOT(user_timeout_handle()));
    connect(m_userWidget->get_pushButtonPtr(Form_plot::Measure_start_b),SIGNAL(clicked(bool)),this,SLOT(plot_pushButton_PlotStart_clicked()));
    connect(m_userWidget->get_pushButtonPtr(Form_plot::Measure_stop_b),SIGNAL(clicked(bool)),this,SLOT(plot_pushButton_PlotStop_clicked()));
    connect(m_userWidget->get_pushButtonPtr(Form_plot::Measure_displayReset_b),SIGNAL(clicked(bool)),this,SLOT(plot_pushButton_DisplayReset_clicked()));
    connect(m_userWidget->get_comboBoxptr(Form_plot::Measure_ADchannel_c),SIGNAL(currentIndexChanged(int)),SLOT(plot_combobox_ADchannel_currentIndexChanged(int)));
    connect(m_userWidget->get_comboBoxptr(Form_plot::Measure_OSchannel_c),SIGNAL(currentIndexChanged(int)),SLOT(plot_combobox_OSchannel_currentIndexChanged(int)));
    connect(m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c),SIGNAL(currentIndexChanged(int)),SLOT(plot_combobox_SlaveIndex_currentIndexChanged(int)));
    connect(m_userWidget->get_DialPtr(Form_plot::Measure_SampleRate_d),SIGNAL(valueChanged(int)),SLOT(plot_dial_SampleRate_valueChanged(int)));
    connect(m_userWidget->get_DialPtr(Form_plot::Measure_DisplayNum_d),SIGNAL(valueChanged(int)),SLOT(plot_dial_DisplayNum_valueChanged(int)));

    connect(m_userCallback,SIGNAL(Master_RunStateChanged(bool)),this,SLOT(callback_Master_RunStateChanged(bool)));
    connect(m_userCallback,SIGNAL(Sig_MasterScanChanged()),this,SLOT(callback_Master_ScanChanged()));
//    qDebug() << "m_OS_Channel" << m_userCallback->m_OS_Channel;
//    qDebug() << "m_SamplingRate" << m_userCallback->m_SamplingRate;
//    qDebug() << "m_AD_Channel" << m_userCallback->m_AD_Channel;

    this->Init_messageSlots();

    if(m_userCallback->Master_getSlaveChoose_AppAllow() == false){
        m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->setEnabled(false);//失能选择
        m_userMessage->set_isHideRight(true);//拖拽模式下就隐藏页面
    }

    return true;
}

bool DRE2008OS_plugin::Destroy_Cores()
{
    m_timePlot->stop();
    return true;
}

/******************* SLOTs *********************************/
void DRE2008OS_plugin::plot_pushButton_PlotStart_clicked()
{

    m_userCallback->Set_OverSampling_run(true);
    m_timePlot->start(5);//刷新时间不能太快，会崩溃
    m_userWidget->get_pushButtonPtr(Form_plot::Measure_start_b)->setEnabled(false);
    m_userWidget->get_pushButtonPtr(Form_plot::Measure_stop_b)->setEnabled(true);

    m_userCallback->Set_OverSampling_Reset(false);//超采样初始化
    m_userCallback->m_measure_Q->clear();

}

void DRE2008OS_plugin::plot_pushButton_PlotStop_clicked()
{
    m_userCallback->Set_OverSampling_run(false);
    m_timePlot->stop();
    m_userWidget->get_pushButtonPtr(Form_plot::Measure_start_b)->setEnabled(true);
    m_userWidget->get_pushButtonPtr(Form_plot::Measure_stop_b)->setEnabled(false);

    m_userCallback->Set_OverSampling_Reset(false);//超采样初始化
    m_userCallback->m_measure_Q->clear();

}

void DRE2008OS_plugin::plot_pushButton_DisplayReset_clicked()
{
    m_isDisFFT_reset = true;
//    m_userWidget->get_customPlot(Form_plot::plot_fft_e)->replot();
    m_plotDisplay_Num = 1024;
    m_userWidget->get_LineEditPtr(Form_plot::Measure_DisplayNum_e)->setText(QString::number(m_plotDisplay_Num));
}

void DRE2008OS_plugin::user_timeout_handle(){
    int display_num = 1024*4;
    int fft_num = display_num/4;//其实就是1024个点
    static QVector<double> m_x(display_num),m_y(display_num);
    static QVector<double> fft_x(fft_num/2),fft_y(fft_num/2);
    static complex_s FFT_IN_BUFF[1024];

    m_userWidget->get_LineEditPtr(Form_plot::Measure_ErrState_e)->setText(m_userCallback->ErrorState_ToString());//显示Error

    int q_size = m_userCallback->m_measure_Q->size();
    if(q_size > display_num){
        for(int i=0;i<display_num;i++){
          m_x[i] = i;
          m_y[i] = m_userCallback->m_measure_Q->dequeue();
          if(i<fft_num){
              FFT_IN_BUFF[i].imag = 0;
              FFT_IN_BUFF[i].real = m_y[i];
          }
        }
    }
    else{
        return ;
    }

    fft(fft_num, FFT_IN_BUFF);//计算FFT

    for (int i = 0; i < fft_num/2; i++) {//对称的，一半即可
        //fft_x[i] = i/fft_num*m_userCallback->m_SamplingRate;//这样会直接转化为频率值
        fft_x[i] = i;
        fft_y[i] = std::sqrt(FFT_IN_BUFF[i].imag*FFT_IN_BUFF[i].imag + FFT_IN_BUFF[i].real*FFT_IN_BUFF[i].real)/(fft_num/2); //计算各次谐波幅值)
    }

    m_userWidget->get_customPlot(Form_plot::plot_standard_e)->graph(0)->setData(m_x, m_y);
    m_userWidget->get_customPlot(Form_plot::plot_standard_e)->xAxis->setRange(-10, m_plotDisplay_Num);
    m_userWidget->get_customPlot(Form_plot::plot_standard_e)->graph(0)->rescaleValueAxis(true);
//    m_userWidget->get_customPlot()->graph(0)->rescaleValueAxis();
//    m_userWidget->get_customPlot()->yAxis->setRange(-20, 700);
    m_userWidget->get_customPlot(Form_plot::plot_standard_e)->replot();

    m_userWidget->get_customPlot(Form_plot::plot_fft_e)->graph(0)->setData(fft_x, fft_y);
    if(m_isDisFFT_reset){
       m_userWidget->get_customPlot(Form_plot::plot_fft_e)->xAxis->setRange(-10, 1024);//fft默认显示全部1024个点
        m_userWidget->get_customPlot(Form_plot::plot_fft_e)->graph(0)->rescaleValueAxis();
       m_isDisFFT_reset = false;
    }
    else{
        //    m_userWidget->get_customPlot(Form_plot::plot_fft_e)->xAxis->setRange(-10, m_plotDisplay_Num);//fft显示就不限制了
        m_userWidget->get_customPlot(Form_plot::plot_fft_e)->graph(0)->rescaleValueAxis(true);
    }

//    m_userWidget->get_customPlot()->graph(0)->rescaleValueAxis();
//    m_userWidget->get_customPlot()->yAxis->setRange(-20, 700);
    m_userWidget->get_customPlot(Form_plot::plot_fft_e)->replot();
}

void DRE2008OS_plugin::plot_dial_SampleRate_valueChanged(int value)
{ 

    if(Check_SampleParameter(m_userCallback->m_OS_Channel,m_userCallback->m_AD_Channel,value) < 0){//恢复上一次的值
        m_userWidget->get_DialPtr(Form_plot::Measure_SampleRate_d)->setValue(m_userCallback->m_SamplingRate);
    }
    else{
        m_userCallback->m_SamplingRate = value;
        m_userWidget->get_LineEditPtr(Form_plot::Measure_SampleRate_e)->setText(QString::number(m_userCallback->m_SamplingRate));
    }

}

void DRE2008OS_plugin::plot_dial_DisplayNum_valueChanged(int value)
{

    m_plotDisplay_Num = value;
    m_userWidget->get_LineEditPtr(Form_plot::Measure_DisplayNum_e)->setText(QString::number(m_plotDisplay_Num));

}

void DRE2008OS_plugin::plot_combobox_ADchannel_currentIndexChanged(int index)
{
   //参数从0开始,index也是从0开始
//    qDebug() << index;
    if(Check_SampleParameter(m_userCallback->m_OS_Channel,index,m_userCallback->m_SamplingRate) < 0){//恢复上一次的值
         m_userWidget->get_comboBoxptr(Form_plot::Measure_ADchannel_c)->setCurrentIndex(m_userCallback->m_AD_Channel);
    }
    else{
         m_userCallback->m_AD_Channel = index;
    }
}

void DRE2008OS_plugin::plot_combobox_OSchannel_currentIndexChanged(int index)
{
   //参数从0开始
    if(Check_SampleParameter(index,m_userCallback->m_AD_Channel,m_userCallback->m_SamplingRate) < 0){//恢复上一次的值
        m_userWidget->get_comboBoxptr(Form_plot::Measure_OSchannel_c)->setCurrentIndex(m_userCallback->m_OS_Channel);
    }
    else{
        m_userCallback->m_OS_Channel = index;
    }
}

void DRE2008OS_plugin::plot_combobox_SlaveIndex_currentIndexChanged(int index)
{
    if(m_userWidget->m_isSlaveChoose_init){
        return;
    }

    m_userCallback->Master_setSlaveChooseIndex(index);//设置优先选择的index
//    qDebug() << index;
}

void DRE2008OS_plugin::callback_Master_RunStateChanged(bool isRun)
{
//    m_is_EtherCAT_Run = isRun;
    if(isRun){
        m_userWidget->get_pushButtonPtr(Form_plot::Measure_start_b)->setEnabled(true);
        m_userWidget->get_pushButtonPtr(Form_plot::Measure_stop_b)->setEnabled(true);
        if(m_userCallback->Master_getSlaveChoose_AppAllow() == true){
            m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->setEnabled(false);//失能选择
        }
    }
    else{
        m_userWidget->get_pushButtonPtr(Form_plot::Measure_start_b)->setEnabled(false);
        m_userWidget->get_pushButtonPtr(Form_plot::Measure_stop_b)->setEnabled(false);
        if(m_userCallback->Master_getSlaveChoose_AppAllow() == true){
            m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->setEnabled(true);//使能选择
        }
    }
}

void DRE2008OS_plugin::callback_Master_ScanChanged()
{
    static int index_last;

    if(index_last == m_userCallback->Master_getSlaveCount()){
        return;
    }

    //NOTE:一定要加上这句话,否则index=-1
    m_userWidget->m_isSlaveChoose_init = true;
    m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->clear();

    if(m_userCallback->Master_getSlaveCount() > 0){
        for(int index = 0;index < m_userCallback->Master_getSlaveCount();index++){//重新加载太耗时了
            m_userWidget->m_isSlaveChoose_init = true;
            m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->addItem(QString::number(index));
        }

        int slaveChoose_tmp = m_userCallback->Master_getSlaveChooseIndex();
        if( slaveChoose_tmp> 0 && slaveChoose_tmp < m_userCallback->Master_getSlaveCount()){//如果已经做了选择
            m_userWidget->m_isSlaveChoose_init = true;
            m_userWidget->get_comboBoxptr(Form_plot::Measure_SlaveIndex_c)->setCurrentIndex(slaveChoose_tmp);
        }

        m_userWidget->m_isSlaveChoose_init = false;
    }

    index_last = m_userCallback->Master_getSlaveCount();

}

void DRE2008OS_plugin::messageSig_isAppRun_Changed(bool isRun)
{
    if(this->get_CallbackPtr()->Master_getSlaveCount() > 0){
        if(isRun){
            plot_pushButton_PlotStart_clicked();
        }
        else{
            plot_pushButton_PlotStop_clicked();
        }
    }
    else{
        QMessageBox::warning(this->get_UIWidgetPtr(),"Warning","No slave Found here!");
    }

}

void DRE2008OS_plugin::messageSig_isHideRight_Changed(bool isHide)
{
    if(isHide){
        this->m_userWidget->get_rightWidgetPtr()->hide();
    }
    else{
        this->m_userWidget->get_rightWidgetPtr()->show();
    }

}

void DRE2008OS_plugin::messageSig_displayNum_Changed(int num)
{
    plot_dial_DisplayNum_valueChanged(num);
}
/******************* SLOTs  end ***********************/


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DRE2008OS_plugins, DRE2008OS_plugin)
#endif // QT_VERSION < 0x050000
