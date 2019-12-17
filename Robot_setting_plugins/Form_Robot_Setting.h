#ifndef FORM_ROBOT_SETTING_H
#define FORM_ROBOT_SETTING_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVector>
#include <QTableWidgetItem>

namespace Ui {
class Form_Robot_Setting;
}

class Form_Robot_Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Robot_Setting(QWidget *parent = 0);
    ~Form_Robot_Setting();
private:
    bool Init_cores();
    bool Destroy_cores();
    void table_setData(QTableWidget *tableWidget, int row, int column, QTableWidgetItem *item, QString &str);

    #define Count_serialParam  9
    QTableWidgetItem m_array_serialItem[Count_serialParam];
public:
    typedef enum{
        comboBox_Serial_name_c
    }comboBox_choose;

    typedef enum{
        pushButton_Serial_open_p,
        pushButton_Serial_search_p,
        pushButton_get_MSG_p,
        pushButton_set_MSG_p
    }pushButton_choose;

    typedef enum{
       lineEdit_version_l
    }lineEdit_choose;

    typedef enum{
       tableWidget_robotMessage,
       tableWidget_serialData
    }tableWidget_choose;

    typedef enum{
        serialItem_version,
        serialItem_robot_id,
        serialItem_limitEnable_mask,
        serialItem_limitRunDir_mask,
        serialItem_limitMode,
        serialItem_limitGoHomeSpeed,
        serialItem_limitAngle_X,
        serialItem_limitAngle_Y,
        serialItem_limitAngle_Z
    }serialItem_choose;

    QComboBox *get_ComboBoxPtr(comboBox_choose choose);
    QPushButton *get_PushButtonPtr(pushButton_choose choose);
    QLineEdit *get_LineEditPtr(lineEdit_choose choose);
    QTableWidget *get_TableWidgePtr(tableWidget_choose choose);
    QTableWidgetItem * get_array_serialItemPtr(void);
private slots:

private:
    Ui::Form_Robot_Setting *ui;
};

#endif // FORM_ROBOT_SETTING_H
