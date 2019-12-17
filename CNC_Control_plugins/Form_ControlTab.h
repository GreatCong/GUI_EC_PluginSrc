#ifndef FORM_CONTROLTAB_H
#define FORM_CONTROLTAB_H

#include <QWidget>

#include <Qtablewidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>

namespace Ui {
class Form_ControlTab;
}

class Form_ControlTab : public QWidget
{
    Q_OBJECT

public:
    explicit Form_ControlTab(QWidget *parent = 0);
    ~Form_ControlTab();

private:
    Ui::Form_ControlTab *ui;
private:
    void Init_Cores();
    void Destroy_Cores();

    void keyPressEvent(QKeyEvent *event);
public:
    typedef enum{
        Gcode_openFile_b,
        Gcode_reloadFile_b,
        Gcode_sendFile_b,
        Gcode_reset_b,
        Gcode_commandSend_b,

        Jog_AxisX_P_b,
        Jog_AxisX_N_b,
        Jog_AxisY_P_b,
        Jog_AxisY_N_b,
        Jog_AxisZ_P_b,
        Jog_AxisZ_N_b,
        Jog_Home_b,
        Jog_Halt_b

    }gcode_button_choose;

    typedef enum{
        Gcode_filePath_e,
        Jog_step_e,
        Gcode_command_e,
        Jog_Speed_e,
        Jog_PosX_e,
        Jog_PosY_e,
        Jog_PosZ_e
    }gcode_lineEdit_choose;

    typedef enum{
       Groups_parameter_g,
       Groups_jog_g,
       Groups_Master_g
    }group_choose;

    typedef enum{
       check_isThetaDis_c,
       check_isModeCalibrate_c,
       check_isLoadFileSafe_c
    }checkBox_choose;

    typedef enum{
       comboBox_SlaveIndex_com
    }comboBox_choose;


    typedef enum{
        Axis_X,
        Axis_Y,
        Axis_Z
    }Axis_num;

    bool m_isSlaveChoose_init;

    QTableWidget *get_TableGcode();
    QPushButton *get_ButtonGcode(gcode_button_choose choose);
    QLineEdit *get_LineEditGcode(gcode_lineEdit_choose choose);
    QGroupBox *get_GroupPtr(group_choose choose);
    QCheckBox *get_CheckBoxPtr(checkBox_choose choose);
    QComboBox *get_ComboBoxPtr(comboBox_choose choose);
    void set_LCDnumber_Display(Axis_num num, double data);
signals:
    void Jog_ButtonDown(int button);
    void Key_EventSignal(QKeyEvent *event);

private slots:
    void on_toolButton_jogY_positive_clicked();
    void on_toolButton_jogX_positive_clicked();
    void on_toolButton_jogX_negtive_clicked();
    void on_toolButton_jogY_negtive_clicked();
    void on_toolButton_jogZ_positive_clicked();
    void on_toolButton_jogZnegtive_clicked();
    void on_toolButton_jogHome_clicked();
    void on_toolButton_jogHalt_clicked();
};

#endif // FORM_CONTROLTAB_H
