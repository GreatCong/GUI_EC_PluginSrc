#ifndef FORM_PACK_H
#define FORM_PACK_H

#include <QWidget>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class Form_Pack;
}

class Form_Pack : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Pack(QWidget *parent = 0);
    ~Form_Pack();
public :
    typedef enum{
        comboBox_qt_installVer_c,
        comboBox_qt_compileVer_c
    }ComboBox_choose;

    typedef enum{
        lineEdit_path_l
    }LineEdit_choose;

    typedef enum{
        pushButton_create_p,
        pushButton_about_p,
        pushButton_moveRelease_p
    }PushButton_choose;

    QComboBox *get_ComboBoxPtr(ComboBox_choose choose);
    QLineEdit *get_LineEditPtr(LineEdit_choose choose);
    QPushButton *get_PushButtonPtr(PushButton_choose choose);
    void Init_cores();
    void Destroy_cores();
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private slots:
    void on_comboBox_QT_installVer_currentTextChanged(const QString &arg1);

    void on_comboBox_QT_compileVer_currentTextChanged(const QString &arg1);

    void on_pushButton_create_clicked();

    void on_pushButton_about_clicked();

    void on_pushButton_moveRelease_clicked();

private:
    Ui::Form_Pack *ui;

    QStringList findFiles(const QString &startDir, QStringList filters);
    bool clearTempFiles(const QString &temp_path);
signals:
    void sig_comboBox_changed(int choose,const QString str);
    void sig_pushButton_changed(int choose);
    void sig_dragEvent(const QString str);
};

#endif // FORM_PACK_H
