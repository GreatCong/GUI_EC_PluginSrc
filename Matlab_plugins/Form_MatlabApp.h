#ifndef FORM_MATLABAPP_H
#define FORM_MATLABAPP_H

#include <QWidget>

namespace Ui {
class Form_MatlabApp;
}

class Form_MatlabApp : public QWidget
{
    Q_OBJECT

public:
    explicit Form_MatlabApp(QWidget *parent = 0);
    ~Form_MatlabApp();

    bool Init_cores();
    bool Destroye_cores();

private:
    Ui::Form_MatlabApp *ui;
};

#endif // FORM_MATLABAPP_H
