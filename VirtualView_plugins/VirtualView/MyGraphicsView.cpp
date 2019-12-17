#include "MyGraphicsView.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QMessageBox>
#include <QLabel>

#include "MyGraphicsScene.h"

MyGraphicsView::MyGraphicsView(QWidget *parent):
    QGraphicsView(parent)
{
   m_master_Message = new EtherCAT_Message(this);
//   m_itemProxyWidget = nullptr;

   m_slaveCount = 0;
   m_Master_addressBase = nullptr;
   m_appCount = 0;
}

MyGraphicsView::~MyGraphicsView()
{
    qDebug() << "~MyGraphicsView";
}

/********** EtherCAT App begin *****************/

void MyGraphicsView::Master_AppLoop_callback()
{
//    qDebug() << "MyGraphicsView::Master_AppLoop_callback";

    foreach (QGraphicsItem *item, this->scene()->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
           MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget *>(item);
           if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_AppLoop_callback();
           }
        }
//        else{
//            qDebug() << item->type();
//        }
    }
}

void MyGraphicsView::Master_AppStart_callback()
{
//    qDebug() << "MyGraphicsView::Master_AppStart_callback";

    MyGraphicsScene *my_scene = dynamic_cast<MyGraphicsScene *>(this->scene());
    my_scene->set_status_AppRun(true);//设置不响应鼠标事件了

    foreach (QGraphicsItem *item, this->scene()->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
           MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget *>(item);
           if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setAdressBase(this->m_Master_addressBase);
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setSlaveCount(this->m_slaveCount);
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setAddressList(this->m_Master_addressList);

               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_AppStart_callback();
           }
        }
//        else{
//            qDebug() << item->type();
//        }
    }
}

void MyGraphicsView::Master_AppStop_callback()
{
//    qDebug() << "MyGraphicsView::Master_AppStop_callback";

    MyGraphicsScene *my_scene = dynamic_cast<MyGraphicsScene *>(this->scene());
    my_scene->set_status_AppRun(false);//设置响应鼠标事件了

    foreach (QGraphicsItem *item, this->scene()->items()) {
        if (item->type() == QGraphicsItem::UserType+1) {
           MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget *>(item);
           if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_AppStop_callback();

               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_ReleaseAddress();
           }
        }
//        else{
//            qDebug() << item->type();
//        }
    }

}

void MyGraphicsView::Master_AppScan_callback()
{
//    qDebug() << "MyGraphicsView::Master_AppScan_callback";

    foreach (QGraphicsItem *item, this->scene()->items()) {

        //NOTE:发现只是拖动一个控件，会产生3个item
        if (item->type() == QGraphicsItem::UserType+1) {
           MyGraphicsProxyWidget *item_proxy = dynamic_cast<MyGraphicsProxyWidget *>(item);
           if(item_proxy->get_userAppPtr()->get_CallbackPtr() != nullptr){
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setAdressBase(this->m_Master_addressBase);
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setSlaveCount(this->m_slaveCount);
               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_setAddressList(this->m_Master_addressList);

               item_proxy->Init_form_SlaveChoose_data();

               item_proxy->get_userAppPtr()->get_CallbackPtr()->Master_AppScan_callback();
           }
        }
//        else{
//            qDebug() << item->type();
//        }

    }

}

/********** EtherCAT App end *****************/

void MyGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/plain")){
        event->acceptProposedAction();
    }

    QGraphicsView::dragEnterEvent(event);
}

void MyGraphicsView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
    {

        QString text = event->mimeData()->text();

//        QPushButton * pb =  new QPushButton(text);

//        Form_example * pb =  new Form_example();
//        pb->setLabel(text);
        MyGraphicsProxyWidget *m_itemProxyWidget = new MyGraphicsProxyWidget();
//        item->setWidget(pb);
        bool ret = this->Plugins_Load(text,m_itemProxyWidget);
        if(ret){
            m_itemProxyWidget->widget()->resize(60,60);
            m_itemProxyWidget->Load_UserStyle(":/style/Resource/style/qdarkstyle/style.qss");
            m_itemProxyWidget->setPos(mapToScene(event->pos()));//注意加上这句话
            m_itemProxyWidget->SetGraphicsMode(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            this->scene()->addItem(m_itemProxyWidget);

            m_itemProxyWidget->get_formSlaveChoosePtr()->resize(250,100);
            m_itemProxyWidget->get_formSlaveChoosePtr()->setWindowIcon(m_itemProxyWidget->get_userAppPtr()->get_AppIcon());
            m_itemProxyWidget->get_formSlaveChoosePtr()->setWindowTitle(m_itemProxyWidget->get_userAppPtr()->get_AppName());

            event->acceptProposedAction();

            this->m_appCount++;//记录App的个数 这样从1开始
            if(m_itemProxyWidget->get_userAppPtr()->get_MessageObj()){
                m_itemProxyWidget->get_userAppPtr()->get_MessageObj()->set_appID(this->m_appCount);
            }
        }
        else{
            delete m_itemProxyWidget;
            event->ignore();
        }

    }

    QGraphicsView::dropEvent(event);
}

bool MyGraphicsView::Plugins_Load(const QString &fileName,MyGraphicsProxyWidget *item_widget)
{
    QDir pluginsDir(this->get_appPath());
#if 0
    if(plugin_userApps){//如果不为空
        plugin_userApps->Destroy_Cores();
        if(plugin_userApps->get_MessageObj()){//有消息响应
//            disconnect(plugin_userApps->get_MessageObj(),SIGNAL(MasterStop_Signal()),this,SLOT(Control_MasterStop_Signal()));
//            disconnect(plugin_userApps->get_MessageObj(),SIGNAL(StatusMessage_change(QString,int)),this,SLOT(Control_StatusMessage_change(QString,int)));
//            disconnect(plugin_userApps->get_MessageObj(),SIGNAL(BottomMessage_change(QString)),this,SLOT(Control_BottomMessage_change(QString)));
        }

        //NOTE:会delete UI
        Dock_Obj_Utils::clearLayout(mtabWeidgetItem_UserApps->layout());//清除layout

        //NOTE:这里unload会调用plugin的析构
        m_Plugin_Loader->unload();//卸载动态库
        delete m_Plugin_Loader;

        //释放new的对象
        plugin_userApps->set_UIWidgetPtr(nullptr);//clearLayout会delete UI，需要手动ptr置空
        delete plugin_userApps;

        plugin_userApps = nullptr;
    }
#endif
    QPluginLoader *m_Plugin_Loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
    // 返回插件的根组件对象
    QObject *pPlugin = m_Plugin_Loader->instance();
    if (pPlugin != Q_NULLPTR) {
        QJsonObject json = m_Plugin_Loader->metaData().value("MetaData").toObject();
        QString plugin_type =  json.value("type").toVariant().toString();
        if(plugin_type.compare("Plugin")){//不是插件
//            qDebug() <<"OK";
            QMessageBox::information(this,tr("Information"),tr("Plugin type is not \"Plugin\" !"));

            //NOTE:这里unload会调用plugin的析构
            m_Plugin_Loader->unload();//卸载动态库
            delete m_Plugin_Loader;

            return false;
        }

       // 访问感兴趣的接口
       EtherCAT_UserApp *plugin_userApps_origin = qobject_cast<EtherCAT_UserApp *>(pPlugin);
       if (plugin_userApps_origin != Q_NULLPTR) {
           item_widget->set_userAppPtr(plugin_userApps_origin->get_NewAppPtr());//new一个新的对象,可以delete
           item_widget->get_userAppPtr()->Init_Object();//初始化相关的对象(其他功能实现的前提)

           item_widget->get_userAppPtr()->set_appPath(this->get_appPath());//传递app的路径
           //NOTE:为了让错误消息能够显示，先连接消息
           if(item_widget->get_userAppPtr()->get_MessageObj()){//有消息响应就连接信号槽
               connect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(MasterStop_Signal()),this,SLOT(Control_MasterStop_Signal()));
               connect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(StatusMessage_change(QString,int)),this,SLOT(Control_StatusMessage_change(QString,int)));
               connect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(BottomMessage_change(QString)),this,SLOT(Control_BottomMessage_change(QString)));
           }

           if(item_widget->get_userAppPtr()->get_CallbackPtr() != nullptr){
               item_widget->get_userAppPtr()->get_CallbackPtr()->Master_setSlaveChoose_AppAllow(false);//App内部的从站编号选择失能
           }

           bool m_isScan_allowed = item_widget->get_userAppPtr()->Init_Cores();
           if(m_isScan_allowed == false){
               item_widget->get_userAppPtr()->Destroy_Cores();
               if(item_widget->get_userAppPtr()->get_MessageObj()){//有消息响应
                   disconnect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(MasterStop_Signal()),this,SLOT(Control_MasterStop_Signal()));
                   disconnect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(StatusMessage_change(QString,int)),this,SLOT(Control_StatusMessage_change(QString,int)));
                   disconnect(item_widget->get_userAppPtr()->get_MessageObj(),SIGNAL(BottomMessage_change(QString)),this,SLOT(Control_BottomMessage_change(QString)));
               }
               //NOTE:这里unload会调用plugin的析构
               m_Plugin_Loader->unload();//卸载动态库
               delete m_Plugin_Loader;

               delete item_widget->get_userAppPtr();
               item_widget->set_userAppPtr(nullptr);

               QMessageBox::information(this,tr("Information"),tr("Plugin Init error!"));
               return false;
           }
           if(item_widget->get_userAppPtr()->get_UIWidgetPtr() == nullptr){//如果没有UI,就定义一个默认的UI
               QLabel *plugin_default_UI = new QLabel(fileName+" UI");
//               mtabWeidgetItem_UserApps->layout()->addWidget(plugin_default_UI);
               item_widget->setWidget(plugin_default_UI);
           }
           else{
//               qDebug() << mtabWeidgetItem_UserApps->width() << mtabWeidgetItem_UserApps->height();
//                mtabWeidgetItem_UserApps->layout()->addWidget(plugin_userApps->get_UIWidgetPtr());
               item_widget->setWidget(item_widget->get_userAppPtr()->get_UIWidgetPtr());
           }

//           if(plugin_userApps->get_MessageObj()){//有消息响应就连接信号槽
//               connect(plugin_userApps->get_MessageObj(),SIGNAL(MasterStop_Signal()),this,SLOT(Control_MasterStop_Signal()));
//               connect(plugin_userApps->get_MessageObj(),SIGNAL(StatusMessage_change(QString,int)),this,SLOT(Control_StatusMessage_change(QString,int)));
//               connect(plugin_userApps->get_MessageObj(),SIGNAL(BottomMessage_change(QString)),this,SLOT(Control_BottomMessage_change(QString)));
//           }

       }

       else {
           //qWarning() << "qobject_cast falied";
           this->get_MessageObj()->Set_BottomMessage(tr("qobject_cast falied"));
//           Master_exit();
           //NOTE:这里unload会调用plugin的析构
           m_Plugin_Loader->unload();//卸载动态库
           delete m_Plugin_Loader;

           return false;
       }
    }

    //NOTE:这里unload会调用plugin的析构
    m_Plugin_Loader->unload();//卸载动态库
    delete m_Plugin_Loader;

    return true;
}




/************* master Slot begin **************************/

void MyGraphicsView::Control_StatusMessage_change(QString message,int interval){
   this->get_MessageObj()->Set_StatusMessage(message,interval);
}

void MyGraphicsView::Control_BottomMessage_change(QString message){
    this->get_MessageObj()->Set_BottomMessage(message);
}

void MyGraphicsView::Control_MasterStop_Signal(){
    this->get_MessageObj()->Set_MasterStop();
}

/************  master Slots end ***************/
