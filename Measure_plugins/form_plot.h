#ifndef FORM_PLOT_H
#define FORM_PLOT_H

#include <QWidget>

#include <QTimer>
#include <qcustomplot.h>

namespace Ui {
class Form_plot;
}

class Form_plot : public QWidget
{
    Q_OBJECT

public:
    explicit Form_plot(QWidget *parent = 0);
    ~Form_plot();

    QCustomPlot *get_customPlot();
private slots:

    void on_pushButton_PlotStart_clicked();

    void on_pushButton_PlotStop_clicked();

    void user_timeout_handle();
private:
    Ui::Form_plot *ui;
private:
    QTimer *m_timePlot;
    void Init_Plots();
};

#endif // FORM_PLOT_H
