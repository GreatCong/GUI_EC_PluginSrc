#include "Form_Pack.h"
#include "ui_Form_Pack.h"

#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QFileInfo>

#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>

///
/// \brief 构造函数
/// \param parent
///
Form_Pack::Form_Pack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Pack)
{
    ui->setupUi(this);
    this->Init_cores();
}

///
/// \brief 析构函数
///
Form_Pack::~Form_Pack()
{
    this->Destroy_cores();
    delete ui;
}

///
/// \brief 获取ComboBoxPtr
/// \param choose
/// \return
///
QComboBox *Form_Pack::get_ComboBoxPtr(ComboBox_choose choose)
{
    switch(choose){
    case comboBox_qt_installVer_c:
        return ui->comboBox_QT_installVer;
        break;
    case comboBox_qt_compileVer_c:
        return ui->comboBox_QT_compileVer;
        break;
    default:
        QMessageBox::warning(this,"Form_Pack","ComboBoxPtr is invalid!");
        return nullptr;
        break;
    }
}

///
/// \brief 获取PushButtonPtr
/// \param choose
/// \return
///
QPushButton *Form_Pack::get_PushButtonPtr(PushButton_choose choose)
{
    switch(choose){
    case pushButton_create_p:
        return ui->pushButton_create;
        break;
    case pushButton_about_p:
        return ui->pushButton_about;
        break;
    case pushButton_moveRelease_p:
        return ui->pushButton_moveRelease;
        break;
    default:
        QMessageBox::warning(this,"Form_Pack","PushButtonPtr is invalid!");
        return nullptr;
        break;
    }
}

///
/// \brief 获取LineEditPtr
/// \param choose
/// \return
///
QLineEdit *Form_Pack::get_LineEditPtr(LineEdit_choose choose)
{
    switch(choose){
    case lineEdit_path_l:
        return ui->lineEdit_path;
        break;
    default:
        QMessageBox::warning(this,"Form_Pack","LineEditPtr is invalid!");
        return nullptr;
        break;
    }
}

///
/// \brief 初始化应用
///
void Form_Pack::Init_cores()
{
    this->setAcceptDrops(true);

    this->get_LineEditPtr(lineEdit_path_l)->setPlaceholderText(QString::fromLocal8Bit("请拖拽要生成的文件"));
    this->get_PushButtonPtr(pushButton_create_p)->setEnabled(false);
}

///
/// \brief 销毁应用
///
void Form_Pack::Destroy_cores()
{

}

///
/// \brief 递归查找文件
/// \param startDir
/// \param filters
/// \return
///
QStringList Form_Pack::findFiles(const QString &startDir, QStringList filters)
{
    QStringList names;
    QDir dir(startDir);

    //搜索当前目录符合条件的文件
    foreach (QString file, dir.entryList(filters, QDir::Files))
        names += startDir + '/' + file;

    //搜索当前目录的子目录符合条件的文件
    foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
        names += findFiles(startDir + '/' + subdir, filters);

    return names;
}

///
/// \brief 清空文件夹
/// \param temp_path
/// \return
///
bool Form_Pack::clearTempFiles(const QString& temp_path)
{
    QDir dir(temp_path);
    if(dir.isEmpty())
    {
        qDebug() << "empty dir";
        return false;
    }

    QStringList filter; //过滤器
    filter.append("*");
    QDirIterator it(temp_path, filter, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()) { //若容器中还有成员，继续执行删除操作
        dir.remove(it.next());
    }

    return true;
}


///
/// \brief 窗口拖拽事件(Enter)
/// \param event
///
void Form_Pack::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

///
/// \brief 窗口拖拽事件(Drop)
/// \param event
///
void Form_Pack::dropEvent(QDropEvent *event)//拖拽文件到窗口中，释放鼠标之后，会执行dropEvent
{
    QString name = event->mimeData()->urls().first().toLocalFile();    //  结果   "E:/a.png"
//    qDebug()<< name;
    QFileInfo fileInfo_tmp(name);
    QString suffix_tmp = fileInfo_tmp.suffix();
    //qDebug() << suffix_tmp;
    if((suffix_tmp.compare("exe") == 0)|| (suffix_tmp.compare("EXE") == 0) ||
       (suffix_tmp.compare("dll") == 0) || (suffix_tmp.compare("a") == 0)){
        this->get_LineEditPtr(lineEdit_path_l)->setText(fileInfo_tmp.fileName());

        emit sig_dragEvent(name);
    }
    else{
        QMessageBox::warning(this,"Warning","The file is invalid!");
    }

}

/************************ Slot begin *****************************/
///
/// \brief QT安装版本的Edit槽
/// \param arg1
///
void Form_Pack::on_comboBox_QT_installVer_currentTextChanged(const QString &arg1)
{
    emit sig_comboBox_changed(comboBox_qt_installVer_c,arg1);
}

///
/// \brief QT编译版本的Edit槽
/// \param arg1
///
void Form_Pack::on_comboBox_QT_compileVer_currentTextChanged(const QString &arg1)
{
    emit sig_comboBox_changed(comboBox_qt_compileVer_c,arg1);
}

///
/// \brief 生成依赖的button槽
///
void Form_Pack::on_pushButton_create_clicked()
{
    emit sig_pushButton_changed(pushButton_create_p);
}

///
/// \brief 关于部分的button槽
///
void Form_Pack::on_pushButton_about_clicked()
{
   QMessageBox::information(this,"Info","Please enjoy it!");
}

///
/// \brief 移动所有的插件到制定路径
///
void Form_Pack::on_pushButton_moveRelease_clicked()
{
    bool ret = false;

    QString dir_choose = "";
    QDir dir_tmp;
    QString dir_removeName = "bin_tmp";
    dir_choose = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("请选择插件路径..."), "./");//如果没有选择路径就会为空
    if(!dir_choose.isEmpty()){
        dir_tmp.cd(dir_choose);
        if(!dir_tmp.exists(dir_removeName)){
            dir_tmp.mkdir(dir_removeName);
        }
        else{
            ret = clearTempFiles(dir_tmp.absolutePath() + "/"+dir_removeName);
            if(ret == false){
                QMessageBox::warning(this,"Warning","delete libs error!");
            }
        }

        QStringList dll_files = findFiles(dir_choose, QStringList() << "*.dll" << "*.DLL");

        ret = true;
        foreach (QString path, dll_files) {
            QFileInfo fileInfo_tmp(path);
            ret &=  QFile::copy(path,dir_choose+"/"+dir_removeName+"/"+fileInfo_tmp.fileName());
        }

        if(ret == false){
            QMessageBox::warning(this,"Warning","copy libs error!");
        }
        else{
            QMessageBox::information(this,"Info","copy libs OK!");
        }
    }

}

/************************ Slot end *****************************/
