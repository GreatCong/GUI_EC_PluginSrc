#ifndef FORM_PLOT_H
#define FORM_PLOT_H

#include <QWidget>

#include <QTimer>

namespace Ui {
class Form_plot;
}

class Form_plot : public QWidget
{
    Q_OBJECT

public:
    explicit Form_plot(QWidget *parent = 0);
    ~Form_plot();
    QTimer *getTimerPtr(){ return m_timePlot;}
    void Init_slots();

private slots:
    void user_timeout_handle();
    void pushButton_run_clicked();

private:
    Ui::Form_plot *ui;

    int time_count;
    QTimer *m_timePlot;
};

#endif // FORM_PLOT_H
