#include "Form_Robot_Parameter.h"
#include "ui_Form_Robot_Parameter.h"

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QStringList>
#include <QLabel>

Form_Robot_Parameter::Form_Robot_Parameter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Robot_Parameter)
{
//    this->get_get_scrollAreaContent_ptr()->setLayout(vBoxlayout);
    ui->setupUi(this);

    QVBoxLayout *vBoxlayout = new QVBoxLayout();
    this->get_get_scrollAreaContent_ptr()->setLayout(vBoxlayout);
//    this->setWindowTitle("Robot Parameter Table");

    QPixmap pixmap(":/Controls/Resource/Control/App_icon.png");
    QIcon robot_icon(pixmap);
    this->setWindowIcon(robot_icon);

}

Form_Robot_Parameter::~Form_Robot_Parameter()
{
    delete ui;
}

void Form_Robot_Parameter::set_robot_ids(const QVector<int> array_robot_id,const QVector<int>  array_robot_index)
{
    m_array_robot_id = array_robot_id;
    m_array_robot_index = array_robot_index;
}

void Form_Robot_Parameter::update_robot_table(int slave_count)
{
    if(m_array_robot_info.size() == 0){
        qDebug() << "m_array_robot_info is invalid!";
        return;
    }
    else if(m_array_robot_id.size() == 0){
        qDebug() << "m_array_robot_id is invalid!";
        return;
    }
    else{
        clear_layout_all(this->get_get_scrollAreaContent_ptr()->layout());//先清除layout

        QLabel *slave_label = new QLabel("Slave_num = "+QString::number(slave_count)+",Robot_num = " + QString::number(m_array_robot_id.size()));
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        slave_label->setPalette(pa);
        QFont ft;
        ft.setPointSize(16);
        slave_label->setFont(ft);
        this->get_get_scrollAreaContent_ptr()->layout()->addWidget(slave_label);

        for(int robot_index =0;robot_index<m_array_robot_id.size();robot_index++){
            QTableWidget *tableWidget = new QTableWidget;
            tableWidget->setMinimumHeight(120);//设置最小显示高度，超过高度就显示滚动条
            QStringList header;
            tableWidget->setColumnCount(3);
            if(m_array_robot_id[robot_index] == 0){//如果ID=0,则说明没有初始化过
                tableWidget->setRowCount(1);//只是显示一行
                header<<"a(n)"<<"d(n)" << "alpha(n)";
                tableWidget->setHorizontalHeaderLabels(header);
                tableWidget->setItem(0,0,new QTableWidgetItem("NULL"));
                tableWidget->setItem(0,1,new QTableWidgetItem("NULL"));
                tableWidget->setItem(0,2,new QTableWidgetItem("NULL"));
            }
            else{
                tableWidget->setRowCount(m_array_robot_info[m_array_robot_id[robot_index]-1].value_DOF);
                header<<"d(n)" <<"a(n)"<< "alpha(n)";
                tableWidget->setHorizontalHeaderLabels(header);
                int index_tmp =0;
                float data_tmp =0;
                for(int dof_index = 0;dof_index <m_array_robot_info[m_array_robot_id[robot_index]-1].value_DOF;dof_index++){
                    index_tmp = 0;
                    data_tmp = m_array_robot_info[m_array_robot_id[robot_index]-1].array_DH_parameter[dof_index].value_d;
                    tableWidget->setItem(dof_index,index_tmp++,new QTableWidgetItem(QString("%1").arg(data_tmp)));
                    data_tmp = m_array_robot_info[m_array_robot_id[robot_index]-1].array_DH_parameter[dof_index].value_a;
                    tableWidget->setItem(dof_index,index_tmp++,new QTableWidgetItem(QString("%1").arg(data_tmp)));
                    data_tmp = m_array_robot_info[m_array_robot_id[robot_index]-1].array_DH_parameter[dof_index].value_alpha;
                    tableWidget->setItem(dof_index,index_tmp++,new QTableWidgetItem(QString("%1").arg(data_tmp)));
                }
            }


            QString robot_str;
            robot_str = "slave"+QString::number(m_array_robot_index[robot_index]+1)+": ";
            if(m_array_robot_id[robot_index] > 0){
                robot_str += m_array_robot_info[m_array_robot_id[robot_index]-1].value_name;
                robot_str +="(id = "+QString::number(m_array_robot_id[robot_index])+
                            ",DOF = "+QString::number(m_array_robot_info[m_array_robot_id[robot_index]-1].value_DOF)+
                            ",model_type = "+QString::number(m_array_robot_info[m_array_robot_id[robot_index]-1].value_type)+")";
            }
            else{
                robot_str += "id = 0";
            }

            QPushButton *button = new QPushButton(robot_str);
            this->get_get_scrollAreaContent_ptr()->layout()->addWidget(button);
            this->get_get_scrollAreaContent_ptr()->layout()->addWidget(tableWidget);
        }
    }

}

void Form_Robot_Parameter::set_window_title(const QString title)
{
    this->setWindowTitle(title);
}

QScrollArea *Form_Robot_Parameter::get_scrollArea_ptr()
{
    return ui->scrollArea;
}

QWidget *Form_Robot_Parameter::get_get_scrollAreaContent_ptr()
{
    return ui->scrollAreaWidgetContents;
}

void Form_Robot_Parameter::clear_layout_all(QLayout *layout)
{
    QLayoutItem *child = nullptr;
    while ((child = layout->takeAt(0)) != 0) {
        delete child;
    }

}

void Form_Robot_Parameter::set_robot_parameters(const QVector<Robot_parameter_s> array_robot_info)
{
    m_array_robot_info = array_robot_info;
}
