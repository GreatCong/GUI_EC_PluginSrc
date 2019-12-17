#include "form_plot.h"
#include "ui_form_plot.h"

#include <QTime>
#include <QPushButton>

Form_plot::Form_plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_plot)
{
    ui->setupUi(this);

    //设置晶体管控件QLCDNumber能显示的位数
    ui->lcdNumber->setDigitCount(10);
    //设置显示的模式为十进制
    ui->lcdNumber->setMode(QLCDNumber::Dec);
    //设置显示方式
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);

    m_timePlot = new QTimer();

    this->Init_slots();

}

Form_plot::~Form_plot()
{
    m_timePlot->stop();

    delete ui;
}

void Form_plot::Init_slots()
{
    connect(ui->pushButton_run,SIGNAL(clicked()),this,SLOT(pushButton_run_clicked()));
    connect(m_timePlot,SIGNAL(timeout()),this,SLOT(user_timeout_handle()));
}


void Form_plot::user_timeout_handle(){
    //获取系统当前时间
    QTime time = QTime::currentTime();
    //设置晶体管控件QLCDNumber上显示的内容
   ui->lcdNumber->display(time.toString("hh:mm:ss"));
}

void Form_plot::pushButton_run_clicked()
{
    m_timePlot->start(1000);
}
