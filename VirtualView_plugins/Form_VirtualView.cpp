#include "Form_VirtualView.h"
#include "ui_Form_VirtualView.h"

#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QGraphicsItem>

#include <QLineEdit>
#include <QMessageBox>

Form_VirtualView::Form_VirtualView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_VirtualView)
{
    ui->setupUi(this);

    this->Init_cores();
}

Form_VirtualView::~Form_VirtualView()
{
    this->Destroy_cores();

    delete ui;
}

bool Form_VirtualView::Init_cores()
{
    m_pScene = new MyGraphicsScene(this);
    //m_pScene->setSceneRect(0, 0, 400, 300);
    m_pView = new MyGraphicsView(this);
    m_pView->setScene(m_pScene);

    //m_pView->resize(800, 600);
    this->layout()->addWidget(m_pView);
    m_pView->show();

    return true;
}

bool Form_VirtualView::Destroy_cores()
{
    return true;
}

MyGraphicsView *Form_VirtualView::get_GraphicsViewPtr()
{
    return m_pView;
}

MyGraphicsScene *Form_VirtualView::get_GraphicsScenePtr()
{
    return m_pScene;
}

void Form_VirtualView::on_pushButton_clearView_clicked()
{
    if(m_pScene->get_status_AppRun() == false){
//        foreach (QGraphicsItem *item, m_pScene->items()) {
//            if (item->type() == QGraphicsItem::UserType+1) {
//                // 从 Scene 上移除 item
//                if (item != NULL){
//                    m_pScene->removeItem(item);
//                    delete item;
//                }
//            }
//        }
        m_pScene->clear();
        this->get_GraphicsViewPtr()->set_appCount(0);
    }
    else{
        QMessageBox::warning(this,"Form_VirtualView","App is running!");
    }

}

void Form_VirtualView::on_pushButton_OpenProject_clicked()
{

}

void Form_VirtualView::on_pushButton_SaveProject_clicked()
{

}
