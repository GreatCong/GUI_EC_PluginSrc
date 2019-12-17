#include "MyGraphicsScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QMimeData>

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>

#include "MyGraphicsProxyWidget.h"

MyGraphicsScene::MyGraphicsScene(QObject *parent):QGraphicsScene(parent)
{
   m_isAppRun = false;
   m_widgetObjectController = new ObjectController();
}

MyGraphicsScene::~MyGraphicsScene()
{

}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Custom scene clicked.";
    QGraphicsScene::mousePressEvent(event);
    //if(this->get_status_AppRun() == false){ //只有app没有run，才能响应鼠标事件
        if (!event->isAccepted()) {
            if (event->button() == Qt::LeftButton) {
                // 在 Scene 上添加一个自定义 item
//                QPointF point = event->scenePos();
//                custom_item *item = new custom_item();
//                item->setRect(point.x()-25, point.y()-25, 60, 60);

//                Form_example * pb =  new Form_example();
//                XGraphicsProxyWidget *item = new XGraphicsProxyWidget;
//                item->setWidget(pb);
//                pb->resize(60,60);
//                item->setPos(point.x(), point.y());//注意加上这句话

//                addItem(item);
                foreach (QGraphicsItem *item, this->items()) {
                    if (item->type() == QGraphicsItem::UserType+1) {
                        MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget*>(item);
                        if(item_proxy->get_userAppPtr()){
                            if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
                                item_proxy->get_formSlaveChoosePtr()->close();
                            }
                        }

                    }
                }
                this->get_WidgetObjectController()->setObject(nullptr);//点击空白的时候,不显示属性框

            } else if (event->button() == Qt::RightButton) {
                // 检测光标下是否有 item
                QGraphicsItem *itemToRemove = NULL;
                foreach (QGraphicsItem *item, items(event->scenePos())) {
                    if (item->type() == QGraphicsItem::UserType+1) {
                        itemToRemove = item;
                        break;
                    }
                }

                if(event->modifiers() == Qt::ControlModifier){ //ctrl + 右键 删除操作
                    // 从 Scene 上移除 item
                    if (itemToRemove != NULL){
                        removeItem(itemToRemove);
                        delete itemToRemove;
                    }
                }
                else{
                    MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget*>(itemToRemove);
                    if(item_proxy->get_userAppPtr()){
                        if(this->get_status_AppRun() == false){
                            if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
                                item_proxy->get_formSlaveChoosePtr()->show();
                            }
    //                        else{
    //                            qDebug() << "MyGraphicsScene: No callback here!";
    //                        }
                        }

                        if(item_proxy->get_userAppPtr()->get_MessageObj() != nullptr){
                            if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
                                item_proxy->get_userAppPtr()->get_MessageObj()->set_slaveIndex(item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_getSlaveChooseIndex());
                                item_proxy->get_userAppPtr()->get_MessageObj()->set_VirtualslaveIndex(item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_getVirtualSlaveChooseIndex());
                            }

                            item_proxy->get_userAppPtr()->get_MessageObj()->setObjectName(item_proxy->get_userAppPtr()->get_AppName()+"_MSG");
                            this->get_WidgetObjectController()->setObject(item_proxy->get_userAppPtr()->get_MessageObj());
                        }
                    }


                }

            }//right button
        }//is Accept
    //}//status_AppRun
//    else{
//        qDebug() << "MyGraphicsScene status_AppRun = true";
//    }


}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "Custom scene moved.";
        QGraphicsScene::mouseMoveEvent(event);
}

void MyGraphicsScene::keyPressEvent(QKeyEvent *event)
{
//    if (event->key() == Qt::Key_Backspace) {
//    // 移除所有选中的 items
//    qDebug() << "selected items " << selectedItems().size();
//    while (!selectedItems().isEmpty()) {
//        removeItem(selectedItems().front());
//    }
//    } else {
//        QGraphicsScene::keyPressEvent(event);
//    }

    QGraphicsScene::keyPressEvent(event);

}

void MyGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/plain")){
        event->acceptProposedAction();
    }
}

void MyGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dropEvent(event);
}

void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}
