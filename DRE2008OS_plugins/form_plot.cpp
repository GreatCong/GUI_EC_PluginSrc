#include "form_plot.h"
#include "ui_form_plot.h"

#include <QDebug>

Form_plot::Form_plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_plot)
{
    ui->setupUi(this);

    Init_Cores();
    m_isSlaveChoose_init = false;
}

Form_plot::~Form_plot()
{
    Destroy_Cores();
    delete ui;
}

void Form_plot::Init_Cores(){

    Init_Plots(get_customPlot(plot_standard_e));
    Init_Plots(get_customPlot(plot_fft_e));
    get_customPlot(plot_fft_e)->xAxis->setRange(-10, 1024);

    get_DialPtr(Measure_SampleRate_d)->setMinimum(1);//最小1Khz
    get_DialPtr(Measure_SampleRate_d)->setMaximum(40);//最大支持40khz
    get_DialPtr(Measure_SampleRate_d)->setValue(20);//初始值设置20

    get_DialPtr(Measure_DisplayNum_d)->setMinimum(10);
    get_DialPtr(Measure_DisplayNum_d)->setMaximum(1024*10);
    get_DialPtr(Measure_SampleRate_d)->setValue(1024);//初始值设置1024

    get_comboBoxptr(Measure_OSchannel_c)->setCurrentIndex(1);//初始设置最大OS_CH1

    get_pushButtonPtr(Form_plot::Measure_start_b)->setEnabled(false);
    get_pushButtonPtr(Form_plot::Measure_stop_b)->setEnabled(false);
}

void Form_plot::Destroy_Cores(){

}

void Form_plot::Init_Plots(QCustomPlot * plot_ptr){
    //    // CustomPlot的基础功能设置
    //    m_CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
    //    // 基础功能共有以下几种，大体意思是：
    //    // 1、轴可拖动     2、通过鼠标滚轮改变轴显示范围 3、用户可以选择多个对象,设定的修饰符（不是特别明白）
    //    // 4、图形是可选的  5、轴是可选的 6、图例是可选的。。。
    //    /*enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref QCPAxisRect::setRangeDrag, \ref QCPAxisRect::setRangeDragAxes)
    //                         ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref QCPAxisRect::setRangeZoom, \ref QCPAxisRect::setRangeZoomAxes)
    //                         ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref QCustomPlot::setMultiSelectModifier while clicking
    //                         ,iSelectPlottables = 0x008 ///< <tt>0x008</tt> Plottables are selectable (e.g. graphs, curves, bars,... see QCPAbstractPlottable)
    //                         ,iSelectAxes       = 0x010 ///< <tt>0x010</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectableParts)
    //                         ,iSelectLegend     = 0x020 ///< <tt>0x020</tt> Legends are selectable (or their child items, see QCPLegend::setSelectableParts)
    //                         ,iSelectItems      = 0x040 ///< <tt>0x040</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
    //                         ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, the plot title,...)
    //                     };*/
    //    // 设置矩形边框
    //    m_CustomPlot->axisRect()->setupFullAxesBox();
    //    // 清空CustomPlot中的图形
    //    m_CustomPlot->clearGraphs();
    //    // 在CustomPlot中添加图形
    //    m_CustomPlot->addGraph();
    //    // 设置图形中的数据m_x和m_y是两个QVector容器
    //    m_CustomPlot->graph(0)->setData(m_x, m_y);
    //    // 这个是设置图形显示为合适范围（感觉设置的只是Y轴）
    //    m_CustomPlot->graph(0)->rescaleValueAxis(true);
    //    // 设置X轴的显示范围（这里是4条轴，x是下面那条，x2是上面那条，Y是先左后右）
    //    m_CustomPlot->xAxis->setRange(m_x.at(0) - 1, m_x.at(m_x.size() - 1) + 1 );
    //    // 刷新m_CustomPlot中数据
    //    m_CustomPlot->replot();

        plot_ptr->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
        //设定背景为黑色
        //customPlot->setBackground(QBrush(Qt::black));
        plot_ptr->setBackground(QBrush(QColor::fromRgb(100,156,199)));
         //设定右上角图形标注可见
//        plot_ptr->legend->setVisible(true);//有于图像变小了，所以不需要设置图标可见
        //设定右上角图形标注的字体
        plot_ptr->legend->setFont(QFont("Helvetica", 9));
        //向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
        plot_ptr->addGraph();
        //设置坐标轴标签名称
    //    customPlot->xAxis->setLabel("x");
    //    customPlot->yAxis->setLabel("y");
        //嵌入式下设置不可见，占屏幕空间
    //    customPlot->xAxis->setVisible(false);//没有用
    //    customPlot->yAxis->setVisible(false);
        //设置右上角图形标注名称
        plot_ptr->graph(0)->setName(QString::fromLocal8Bit("CH1"));//QString::fromLocal8Bit为了避免中文乱码

//        get_customPlot()->addGraph();
//        get_customPlot()->graph(1)->setName(QString::fromLocal8Bit("CH2"));
//        get_customPlot()->graph(1)->setPen(QPen(Qt::red));
//        get_customPlot()->addGraph();
//        get_customPlot()->graph(2)->setPen(QPen(Qt::green));
//        get_customPlot()->graph(2)->setName(QString::fromLocal8Bit("CH3"));
//        get_customPlot()->addGraph();
//        get_customPlot()->graph(3)->setPen(QPen(Qt::yellow));
//        get_customPlot()->graph(3)->setName(QString::fromLocal8Bit("CH4"));

        plot_ptr->axisRect()->setupFullAxesBox();

        //定义两个可变数组存放绘图的坐标数据
    //    QVector<double> x(101),y(101);//分别存放x和y坐标的数据,101为数据长度
    //    //添加数据，我们这里演示y=x^3,为了正负对称，我们x从-10到+10
    //    for(int i=0;i<101;i++)
    //    {
    //        x[i] = i/5 - 10;
    //        y[i] = x[i] * x[i] * x[i];
    //    }
        //添加数据
    //    customPlot->graph(0)->setData(x,y);

        //设置坐标轴显示范围,否则我们只能看到默认的范围
    //    customPlot->xAxis->setRange(-11,11);
    //    customPlot->yAxis->setRange(-1100,1100);

        //重绘，这里可以不用，官方例子有，执行setData函数后自动重绘
        //我认为应该用于动态显示或者是改变坐标轴范围之后的动态显示，我们以后探索
        //ui->qCustomPlot->replot();
}

QCustomPlot *Form_plot::get_customPlot(PlotType_enum plotType){
    switch(plotType){
      case plot_standard_e:
        return ui->widget_plotStandard;
        break;
    case plot_fft_e:
        return ui->widget_plotFFT;
        break;
    default:
        return ui->widget_plotStandard;
        break;
    }
}

QPushButton *Form_plot::get_pushButtonPtr(Measure_button_choose choose)
{
    switch(choose){
    case Measure_start_b:
        return ui->pushButton_PlotStart;
        break;
    case Measure_stop_b:
        return ui->pushButton_PlotStop;
        break;
    case Measure_displayReset_b:
        return ui->pushButton_DisPlayReset;
        break;
    default:
        QMessageBox::critical(this,tr("Form_plot Button"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QComboBox *Form_plot::get_comboBoxptr(Measure_ComboBox_choose choose)
{
    switch(choose){
    case Measure_ADchannel_c:
        return ui->comboBox_AD_CH;
        break;
    case Measure_OSchannel_c:
        return ui->comboBox_SampleOS_CH;
        break;
    case Measure_SlaveIndex_c:
        return ui->comboBox_SlaveIndex;
        break;
    default:
        QMessageBox::critical(this,tr("Form_plot Combobox"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QDial *Form_plot::get_DialPtr(Measure_Dial_choose choose)
{
    switch(choose){
    case Measure_SampleRate_d:
        return ui->dial_SampleRate;
        break;
    case Measure_DisplayNum_d:
        return ui->dial_Display_Num;
        break;
    default:
        QMessageBox::critical(this,tr("Form_plot Dial"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QLineEdit *Form_plot::get_LineEditPtr(Measure_LineEdit_choose choose)
{
    switch(choose){
    case Measure_SampleRate_e:
        return ui->lineEdit_Sample_Rate;
        break;
    case Measure_DisplayNum_e:
        return ui->lineEdit_DisplayNum;
        break;
    case Measure_ErrState_e:
        return ui->lineEdit_ErrState;
        break;
    default:
        QMessageBox::critical(this,tr("Form_plot LineEdit"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

QScrollArea *Form_plot::get_rightWidgetPtr()
{
    return ui->scrollArea_right;
}

void Form_plot::on_lineEdit_Sample_Rate_editingFinished()
{
    QString rate_str = get_LineEditPtr(Measure_SampleRate_e)->text();
    int rate = rate_str.toInt();
    if(rate >=1 && rate <= 40){
        get_DialPtr(Measure_SampleRate_d)->setValue(rate);
    }
    else{
        get_LineEditPtr(Measure_SampleRate_e)->setText("20");//设置成默认的20kHz
    }

//    qDebug() << "arg1";
}

void Form_plot::on_lineEdit_DisplayNum_editingFinished()
{
    QString dis_str = get_LineEditPtr(Measure_DisplayNum_e)->text();
    int dis_num = dis_str.toInt();
    if(dis_num >=10 && dis_num <= 1024*10){
        get_DialPtr(Measure_DisplayNum_d)->setValue(dis_num);
    }
    else{
        get_LineEditPtr(Measure_DisplayNum_e)->setText("1024");
    }
//    qDebug() << "arg2";
}
