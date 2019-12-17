#ifndef FORM_ROBOT_PARAMETER_H
#define FORM_ROBOT_PARAMETER_H

#include <QWidget>
#include <QScrollArea>
#include "Excel_Object.h"

namespace Ui {
class Form_Robot_Parameter;
}

class Form_Robot_Parameter : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Robot_Parameter(QWidget *parent = 0);
    ~Form_Robot_Parameter();
public:
    void set_robot_parameters(const QVector<Robot_parameter_s>  array_robot_info);
    void set_robot_ids(const QVector<int>  array_robot_id,const QVector<int>  array_robot_index);
    void update_robot_table(int slave_count);
    void set_window_title(const QString title);

    QScrollArea* get_scrollArea_ptr();
    QWidget *get_get_scrollAreaContent_ptr();
private:
    QVector<Robot_parameter_s> m_array_robot_info;
    QVector<int> m_array_robot_id;
    QVector<int> m_array_robot_index;

    void clear_layout_all(QLayout *layout);
private:
    Ui::Form_Robot_Parameter *ui;
};

#endif // FORM_ROBOT_PARAMETER_H
