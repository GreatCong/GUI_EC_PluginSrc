#ifndef FORM_SLAVECHOOSE_H
#define FORM_SLAVECHOOSE_H

#include <QWidget>

#include <QLineEdit>
#include <QComboBox>

namespace Ui {
class Form_SlaveChoose;
}

class Form_SlaveChoose : public QWidget
{
    Q_OBJECT

public:
    explicit Form_SlaveChoose(QWidget *parent = 0);
    ~Form_SlaveChoose();

    bool Init_cores();
    bool Destroy_cores();

    QComboBox *get_ComboBox_SlaveIndexPtr();
    QLineEdit *get_LineEdit_SlaveNamePtr();
private slots:

private:
    Ui::Form_SlaveChoose *ui;
};

#endif // FORM_SLAVECHOOSE_H
