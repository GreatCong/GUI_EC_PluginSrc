#include "form_plot.h"
#include "ui_form_plot.h"

#include <QTime>

Form_plot::Form_plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_plot)
{
    ui->setupUi(this);

    Init_Plots();

    m_timePlot = new QTimer();
    connect(m_timePlot,SIGNAL(timeout()),this,SLOT(user_timeout_handle()));
}

Form_plot::~Form_plot()
{
    m_timePlot->stop();

    delete ui;
}

void Form_plot::Init_Plots(){
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

        get_customPlot()->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
        //设定背景为黑色
        //customPlot->setBackground(QBrush(Qt::black));
        get_customPlot()->setBackground(QBrush(QColor::fromRgb(100,156,199)));
         //设定右上角图形标注可见
//        get_customPlot()->legend->setVisible(true);
        //设定右上角图形标注的字体
        get_customPlot()->legend->setFont(QFont("Helvetica", 9));
        //向绘图区域QCustomPlot(从widget提升来的)添加一条曲线
        get_customPlot()->addGraph();
        //设置坐标轴标签名称
    //    customPlot->xAxis->setLabel("x");
    //    customPlot->yAxis->setLabel("y");
        //嵌入式下设置不可见，占屏幕空间
    //    customPlot->xAxis->setVisible(false);//没有用
    //    customPlot->yAxis->setVisible(false);
        //设置右上角图形标注名称
        get_customPlot()->graph(0)->setName(QString::fromLocal8Bit("CH1"));//QString::fromLocal8Bit为了避免中文乱码

//        get_customPlot()->addGraph();
//        get_customPlot()->graph(1)->setName(QString::fromLocal8Bit("CH2"));
//        get_customPlot()->graph(1)->setPen(QPen(Qt::red));
//        get_customPlot()->addGraph();
//        get_customPlot()->graph(2)->setPen(QPen(Qt::green));
//        get_customPlot()->graph(2)->setName(QString::fromLocal8Bit("CH3"));
//        get_customPlot()->addGraph();
//        get_customPlot()->graph(3)->setPen(QPen(Qt::yellow));
//        get_customPlot()->graph(3)->setName(QString::fromLocal8Bit("CH4"));

        get_customPlot()->axisRect()->setupFullAxesBox();

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

QCustomPlot *Form_plot::get_customPlot(){
    return ui->widget_customPlot;
}



void Form_plot::on_pushButton_PlotStart_clicked()
{

    m_timePlot->start(100);
}

void Form_plot::on_pushButton_PlotStop_clicked()
{
   m_timePlot->stop();
}

void Form_plot::user_timeout_handle(){
    static int xx;

    QVector<double> m_x(101),m_y(101);
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    for(int i=0;i<101;i++){
        m_x[i] = (i+xx)/5.0;
//        m_y[i] = sin(m_x[i]*3.14)*(qrand()%10);
        m_y[i] = sin(m_x[i]*3.14);
        m_x[i] = i;
    }
    xx+= 2;
    if(xx>3000){
        xx = 0;
    }

    get_customPlot()->graph(0)->setData(m_x, m_y);
    get_customPlot()->xAxis->setRange(-10, 120);
//    get_customPlot()->graph(0)->rescaleValueAxis(true);
//    get_customPlot()->graph(0)->rescaleValueAxis();
    get_customPlot()->yAxis->setRange(-2, 2);
    get_customPlot()->replot();
}
