#include "Form_Robot_Setting.h"
#include "ui_Form_Robot_Setting.h"

#include <QMessageBox>

///
/// \brief 构造函数
/// \param parent
///
Form_Robot_Setting::Form_Robot_Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Robot_Setting)
{
    ui->setupUi(this);

    this->Init_cores();
}

///
/// \brief 析构函数
///
Form_Robot_Setting::~Form_Robot_Setting()
{
    this->Destroy_cores();

    delete ui;
}

///
/// \brief 初始化应用
/// \return
///
bool Form_Robot_Setting::Init_cores()
{
    this->get_TableWidgePtr(tableWidget_robotMessage)->setColumnCount(2);
    this->get_TableWidgePtr(tableWidget_robotMessage)->setHorizontalHeaderLabels(QStringList()<<tr("ID")<<tr("Name"));
    this->get_TableWidgePtr(tableWidget_robotMessage)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置等宽
    this->get_TableWidgePtr(tableWidget_robotMessage)->horizontalHeader()->setStretchLastSection(true);
    this->get_TableWidgePtr(tableWidget_robotMessage)->verticalHeader()->setVisible(false); //设置垂直头不可见

    this->get_TableWidgePtr(tableWidget_robotMessage)->setMinimumHeight(120);//设置最小显示高度，超过高度就显示滚动条

    //tableWidget_serialData
    this->get_TableWidgePtr(tableWidget_serialData)->setColumnCount(2);
    this->get_TableWidgePtr(tableWidget_serialData)->setHorizontalHeaderLabels(QStringList()<<tr("Name")<<tr("Data"));
    this->get_TableWidgePtr(tableWidget_serialData)->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//设置等宽
    this->get_TableWidgePtr(tableWidget_serialData)->horizontalHeader()->setStretchLastSection(true);
    this->get_TableWidgePtr(tableWidget_serialData)->verticalHeader()->setVisible(false); //设置垂直头不可见

    this->get_TableWidgePtr(tableWidget_serialData)->setMinimumHeight(120);//设置最小显示高度，超过高度就显示滚动条

    //int param_count = 5;//5个参数
    int index =0;
    QTableWidgetItem *item_name_tmp = nullptr;
    this->get_TableWidgePtr(tableWidget_serialData)->setRowCount(Count_serialParam);
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("版本号"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("机器人ID"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("限位使能掩码"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("归位方向掩码"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("限位模式"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("归位速度"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("限位角度X"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("限位角度Y"));
    this->table_setData(this->get_TableWidgePtr(tableWidget_serialData),index++,0,item_name_tmp,QString::fromLocal8Bit("限位角度Z"));

    for(index=0;index<Count_serialParam;index++){
        m_array_serialItem[index].setText("");
        m_array_serialItem[index].setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        this->get_TableWidgePtr(tableWidget_serialData)->setItem(index,1,&m_array_serialItem[index]);
    }

    return true;
}

///
/// \brief 销毁应用
/// \return
///
bool Form_Robot_Setting::Destroy_cores()
{
    return true;
}

///
/// \brief 向table中添加数据
/// \param tableWidget
/// \param row
/// \param column
/// \param item
/// \param str
///
void Form_Robot_Setting::table_setData(QTableWidget *tableWidget, int row, int column, QTableWidgetItem *item,QString &str)
{
    item = new QTableWidgetItem(str);
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    tableWidget->setItem(row,column,item);
}

///
/// \brief 获得ComboBoxPtr
/// \param choose
/// \return
///
QComboBox *Form_Robot_Setting::get_ComboBoxPtr(comboBox_choose choose)
{
    switch(choose){
    case comboBox_Serial_name_c:
        return ui->comboBox_serialName;
        break;
    default:
        QMessageBox::critical(this,tr("Form_Robot_Setting ComboBox"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

///
/// \brief 获得PushButtonPtr
/// \param choose
/// \return
///
QPushButton *Form_Robot_Setting::get_PushButtonPtr(pushButton_choose choose)
{
    switch(choose){
    case pushButton_Serial_open_p:
        return ui->pushButton_serialOpen;
        break;
    case pushButton_Serial_search_p:
        return ui->pushButton_serialSearch;
        break;
    case pushButton_get_MSG_p:
        return ui->pushButton_getMSG;
        break;
    case pushButton_set_MSG_p:
        return ui->pushButton_setMSG;
        break;
    default:
        QMessageBox::critical(this,tr("Form_Robot_Setting PushButton"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

///
/// \brief 获得LineEditPtr
/// \param choose
/// \return
///
QLineEdit *Form_Robot_Setting::get_LineEditPtr(lineEdit_choose choose)
{
    switch(choose){
    case 0:
    default:
        QMessageBox::critical(this,tr("Form_Robot_Setting LineEdit"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

///
/// \brief 获得TableWidgePtr
/// \param choose
/// \return
///
QTableWidget *Form_Robot_Setting::get_TableWidgePtr(tableWidget_choose choose)
{
    switch(choose){
    case tableWidget_robotMessage:
        return ui->tableWidget_robotMessage;
        break;
    case tableWidget_serialData:
        return ui->tableWidget_serialData;
        break;
    default:
        QMessageBox::critical(this,tr("Form_Robot_Setting TableWidge"),tr("Return NULL!"));
        return NULL;
        break;
    }
}

///
/// \brief 获得serialItemPtr(table中的对象值)
/// \return
///
QTableWidgetItem *Form_Robot_Setting::get_array_serialItemPtr()
{
    return m_array_serialItem;
}
