#ifndef ALGORITHM_COMMON_H
#define ALGORITHM_COMMON_H

#include <QString>
#include <QVector>

typedef uint8_t ( *calculate_dll)(float *cartesian_theta, const float *cartesian,
                                  float**DH_table,int count_dof,int count_param); //定义函数指针

typedef struct Algorithm_dll_t{
    calculate_dll calculate_arm_dll;
    calculate_dll calculate_forward_dll;
    bool is_load;
    QString model_name;

    Algorithm_dll_t(){
        calculate_arm_dll = nullptr;
        calculate_forward_dll = nullptr;

        is_load = false;
        model_name = "";
    }
}Algorithm_dll_t;

typedef struct{
    QVector<float> step_per_unit;
    QVector<float> manual_home_pos;
}Machine_parameter_t;

typedef struct Robot_DH_Parameter_s
{
    float value_d;
    float value_a;
    float value_alpha;

    Robot_DH_Parameter_s() {
        value_d = 0;
        value_a = 0;
        value_alpha = 0;
    }
}Robot_DH_Parameter_s;

typedef struct Robot_parameter_s{
    int value_id; //机器人ID
    QString value_name;//机器人的名字
    int value_DOF;//机器人的自由度
    int value_type;//标记下机器人类型（可能是传送带或机器人）

    typedef enum{
        TYPE_ROBOT,
        TYPE_CARRIER //传送带
    }Robot_type_enu;

    QVector<Robot_DH_Parameter_s> array_DH_parameter;//DH参数表

    Robot_parameter_s(){
        this->value_id = -1;
        this->value_name = "";
        this->value_DOF = 0;
        this->value_type = TYPE_ROBOT;//默认是机器人类型
    }

    void resize_DOF(){
        array_DH_parameter.resize(this->value_DOF);
    }

    QString toString(){
        QString str;
        str += QString::number(value_id)+" , ";
        str += value_name+" , ";
        str += QString::number(value_type)+" , ";
        str += QString::number(value_DOF)+" , ";
        for(int i=0;i<value_DOF && i<array_DH_parameter.size();i++){
            str += QString("%1").arg(array_DH_parameter[i].value_a)+" , ";
            str += QString("%1").arg(array_DH_parameter[i].value_d)+" , ";
            str += QString("%1").arg(array_DH_parameter[i].value_alpha)+" , ";
        }

        return str;
    }
}Robot_parameter_s;

#endif // ALGORITHM_COMMON_H
