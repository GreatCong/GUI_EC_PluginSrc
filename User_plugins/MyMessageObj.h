#ifndef MYMESSAGEOBJ_H
#define MYMESSAGEOBJ_H

#include "EtherCAT_UserApp.h"

class MyMessageObj : public EtherCAT_Message
{
    Q_OBJECT
    Q_PROPERTY(int  xx READ getxx WRITE setxx)
    Q_PROPERTY(bool check_num READ get_check_num WRITE set_check_num)
public:
    MyMessageObj(QObject *parent = nullptr);

    int getxx(){return x;}
    void setxx(int x){this->x = x; emit sig_x(x);}
    int get_check_num(){return check_num;}
    void set_check_num(bool check_num){this->check_num = check_num; emit sig_x(check_num);}
private:
    int x;
    bool check_num;
signals:
    void sig_x(int data);
};

#endif // MYMESSAGEOBJ_H
