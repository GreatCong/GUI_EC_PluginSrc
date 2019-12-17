#ifndef FORM_PLOT_H
#define FORM_PLOT_H

#include <QWidget>

#include <QTimer>
#include <qcustomplot.h>
#include <QPushButton>
#include <QDial>
#include <QComboBox>
#include <QLineEdit>

namespace Ui {
class Form_plot;
}

class Form_plot : public QWidget
{
    Q_OBJECT

public:
    explicit Form_plot(QWidget *parent = 0);
    ~Form_plot();
public:
    typedef enum{
        Measure_start_b,
        Measure_stop_b,
        Measure_displayReset_b
    }Measure_button_choose;

    typedef enum{
        Measure_ADchannel_c,
        Measure_OSchannel_c,
        Measure_SlaveIndex_c
    }Measure_ComboBox_choose;

    typedef enum{
        Measure_SampleRate_d,
        Measure_DisplayNum_d
    }Measure_Dial_choose;

    typedef enum{
        Measure_SampleRate_e,
        Measure_DisplayNum_e,
        Measure_ErrState_e
    }Measure_LineEdit_choose;

    typedef enum{
        plot_standard_e,
        plot_fft_e
    }PlotType_enum;

    bool m_isSlaveChoose_init;

    QCustomPlot *get_customPlot(PlotType_enum plotType);
    QPushButton *get_pushButtonPtr(Measure_button_choose choose);
    QComboBox *get_comboBoxptr(Measure_ComboBox_choose choose);
    QDial *get_DialPtr(Measure_Dial_choose choose);
    QLineEdit *get_LineEditPtr(Measure_LineEdit_choose choose);
    QScrollArea *get_rightWidgetPtr();
private slots:
    void on_lineEdit_Sample_Rate_editingFinished();

    void on_lineEdit_DisplayNum_editingFinished();

private:
    Ui::Form_plot *ui;
private:

    void Init_Plots(QCustomPlot * plot_ptr);
    void Init_Cores();
    void Destroy_Cores();
};

#endif // FORM_PLOT_H
