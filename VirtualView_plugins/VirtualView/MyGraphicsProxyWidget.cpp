#include "MyGraphicsProxyWidget.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>

#define MARGIN 20   //四个角的长度

MyGraphicsProxyWidget::MyGraphicsProxyWidget( QGraphicsItem * parent /* = 0 */, Qt::WindowFlags wFlags /* = 0 */ ) :
    QGraphicsProxyWidget(parent, wFlags)
{
    m_eWidgetFlag = this->flags();
    m_eGraphicsFlag = 0;
    m_userApp = nullptr;
    m_SlaveChoose_indexLast = -1;

    //标记鼠标左击时的位置
    m_Zoom_curPos = 0;

    m_isInitSlaveChoose_data = false;

    m_form_SlaveChoose = new Form_SlaveChoose();

    connect(m_form_SlaveChoose->get_ComboBox_SlaveIndexPtr(),SIGNAL(currentIndexChanged(int)),this,SLOT(SlaveChoose_slaveIndex_changed(int)));
//    // 可选择 可移动
//    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);//注意调用SetGraphicsMode函数设置模式
}

void MyGraphicsProxyWidget::SetGraphicsMode(GraphicsItemFlags flags)
{
    m_eGraphicsFlag = flags;
}

MyGraphicsProxyWidget::~MyGraphicsProxyWidget()
{
    if(this->get_userAppPtr()){
        qDebug() << "~MyGraphicsProxyWidget" << this->get_userAppPtr()->get_AppName();
    }
    else{
        qDebug() << "~MyGraphicsProxyWidget,name=NULL";
    }

    if(m_userApp){
        m_userApp->Destroy_Cores();
        m_userApp->set_UIWidgetPtr(nullptr);
        delete m_userApp;
        m_userApp = nullptr;
    }

    m_form_SlaveChoose->close();
    delete m_form_SlaveChoose;
}

void MyGraphicsProxyWidget::Init_form_SlaveChoose_data()
{
//    qDebug() << "index_last" << m_SlaveChoose_indexLast;

    if(this->get_userAppPtr()->get_CallbackPtr() != nullptr){
        if(m_SlaveChoose_indexLast == this->get_userAppPtr()->get_CallbackPtr()->Master_getSlaveCount()){
//            qDebug() << "index_last = count,return" << this->get_userAppPtr()->get_AppName();
             return;
        }

        m_isInitSlaveChoose_data = true;
        m_form_SlaveChoose->get_ComboBox_SlaveIndexPtr()->clear();
        int slave_count = this->get_userAppPtr()->get_CallbackPtr()->Master_getSlaveCount();
        if(slave_count >0){
            for(int index=0;index<slave_count;index++){
                m_isInitSlaveChoose_data = true;
                m_form_SlaveChoose->get_ComboBox_SlaveIndexPtr()->addItem(QString::number(index));
            }

            int slaveChoose_tmp = this->get_userAppPtr()->get_CallbackPtr()->Master_getSlaveChooseIndex();
            if( slaveChoose_tmp> 0 && slaveChoose_tmp < slave_count){//如果已经做了选择
                m_isInitSlaveChoose_data = true;
                m_form_SlaveChoose->get_ComboBox_SlaveIndexPtr()->setCurrentIndex(slaveChoose_tmp);
                m_form_SlaveChoose->get_LineEdit_SlaveNamePtr()->setText(this->get_userAppPtr()->get_CallbackPtr()->Master_getAddressList().at(slaveChoose_tmp).slave_name);
            }
            else{
                m_form_SlaveChoose->get_LineEdit_SlaveNamePtr()->setText(this->get_userAppPtr()->get_CallbackPtr()->Master_getAddressList().at(0).slave_name);
            }

            m_isInitSlaveChoose_data = false;

        }

        m_SlaveChoose_indexLast = this->get_userAppPtr()->get_CallbackPtr()->Master_getSlaveCount();
    }

}

void MyGraphicsProxyWidget::StartGraphicsMode()
{
    m_eWidgetFlag = flags(); // save

    setFlags(m_eGraphicsFlag);
}

void MyGraphicsProxyWidget::StopGraphicsMode()
{
    setFlags(m_eWidgetFlag);
}

bool MyGraphicsProxyWidget::IsGraphicsMode() const
{
    return flags() == m_eGraphicsFlag;
}

void MyGraphicsProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //只有按住ctrl才能响应QGraphicsItem,否则就响应QGraphicsProxyWidget
    if (event->type() == QEvent::GraphicsSceneMousePress
        && (event->modifiers() & Qt::ControlModifier))
    {
        StartGraphicsMode();

        //QGraphicsItem::mousePressEvent(event);
        if(event->button() == Qt::LeftButton){
            if (event->modifiers() & Qt::ShiftModifier) {
//                qDebug() << "Custom item left clicked with shift key.";
                // 选中 item
                setSelected(true);
            }
            else if (event->modifiers() & Qt::AltModifier) {
//                qDebug() << "Custom item left clicked with alt key.";
                m_bResizing = true;
                QPointF temp = event->scenePos();
//                qDebug()<<"Press globalpos: "<<temp<<"Press pos: "<<event->pos();
                m_Zoom_pLast = temp;
                m_Zoom_curPos = countFlag(event->pos(), countRow(event->pos()));
//                qDebug() << "curPos:" << m_Zoom_curPos;
            }
            else{
//                qDebug() << "Custom item left clicked.";
                QGraphicsItem::mousePressEvent(event);
//                QGraphicsProxyWidget::mousePressEvent(event);
                event->accept();
            }
        }
        else if (event->button() == Qt::RightButton) {
//            qDebug() << "Custom item right clicked.";
            event->ignore();
        }
    }
    else
    {
        QGraphicsProxyWidget::mousePressEvent(event);
    }

}

void MyGraphicsProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (IsGraphicsMode())
    {
        StopGraphicsMode();
        if (((event->modifiers() & Qt::AltModifier)) && m_bResizing){
            m_bResizing = false;
            this->setCursor(Qt::ArrowCursor);
        }
        else{
            QGraphicsItem::mouseReleaseEvent(event);
        }
        //QGraphicsItem::mouseReleaseEvent(event);
    }
    else
    {
        QGraphicsProxyWidget::mouseReleaseEvent(event);
    }



}

void MyGraphicsProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if (IsGraphicsMode()){
        //QGraphicsItem::mouseMoveEvent(event);
        if (((event->modifiers() & Qt::AltModifier)) && m_bResizing) {
            int poss = countFlag(event->pos(), countRow(event->pos()));
            this->setCursorType(poss);
            //是否左击
            if(m_bResizing)
            {
                QPointF ptemp = event->scenePos();
                ptemp = ptemp - m_Zoom_pLast;
                if(m_Zoom_curPos == Zoom_Pos_Center)    //移动窗口
                {
                    //ptemp = ptemp + e->pos();
//                    ptemp = ptemp + this->pos();
    //                this->widget()->move(ptemp);
//                    this->setPos(ptemp);
//                  qDebug() << "move 22";
//                    QGraphicsItem::mouseMoveEvent(event);
                }
                else
                {
                    QRectF wid = geometry();

                    //改变窗口的大小
                    switch(m_Zoom_curPos)
                    {
                    case Zoom_Pos_LeftTop://左上角
                        wid.setTopLeft(wid.topLeft() + ptemp);
                        break;
                    case Zoom_Pos_RightTop://右上角
                        wid.setTopRight(wid.topRight() + ptemp);
                        break;
                    case Zoom_Pos_LeftBottom://左下角
                        wid.setBottomLeft(wid.bottomLeft() + ptemp);
                        break;
                    case Zoom_Pos_RightBottom://右下角
                        wid.setBottomRight(wid.bottomRight() + ptemp);
                        break;
                    case Zoom_Pos_Top://中上角
                        wid.setTop(wid.top() + ptemp.y());
                        break;
                    case Zoom_Pos_Left://中左角
                        wid.setLeft(wid.left() + ptemp.x());
                        break;
                    case Zoom_Pos_Right://中右角
                        wid.setRight(wid.right() + ptemp.x());
                        break;
                    case Zoom_Pos_Bottom://中下角
                        wid.setBottom(wid.bottom() + ptemp.y());
                        break;
                    }
                    this->setGeometry(wid);
                }

                //更新位置
                m_Zoom_pLast = event->scenePos();
            }
//               event->ignore();
        }
        else if(!(event->modifiers() & Qt::AltModifier)) {
//            qDebug() << "Custom item moved.";
            QGraphicsItem::mouseMoveEvent(event);
    //                QGraphicsProxyWidget::mouseMoveEvent(event);
//            qDebug() << "moved" << pos();
        }
    }
    else{
        QGraphicsProxyWidget::mouseMoveEvent(event);
    }


}


///
/// \brief MyGraphicsProxyWidget::Load_UserStyle
/// \param name
/// \return
///
bool MyGraphicsProxyWidget::Load_UserStyle(const QString &name){
    QFile qss(name);
    if(!qss.open(QFile::ReadOnly)){
        return false;
    }

    this->widget()->setStyleSheet(qss.readAll());
    qss.close();

    return true;
}

//计算鼠标在哪一行那一列
int MyGraphicsProxyWidget::countFlag(QPointF p, int row)
{
    if(p.y() < MARGIN)
        return 10+row;
    else if(p.y() > this->boundingRect().height()-MARGIN)
        return 30+row;
    else
        return 20+row;
}


//根据鼠标所在位置改变鼠标指针形状
void MyGraphicsProxyWidget::setCursorType(int flag)
{
    Qt::CursorShape cursor;
    switch(flag)
    {
    case Zoom_Pos_LeftTop:
    case Zoom_Pos_RightBottom:
//        qDebug() << "leftTop 11 : rightBottom 33";
        cursor = Qt::SizeFDiagCursor;
        break;
    case Zoom_Pos_RightTop:
    case Zoom_Pos_LeftBottom:
//        qDebug() << "RightTop 13 : Left Bottom 31";
        cursor = Qt::SizeBDiagCursor;
        break;
    case Zoom_Pos_Left:
    case Zoom_Pos_Right:
//        qDebug() << "Left 21 : Right 23";
        cursor = Qt::SizeHorCursor;
        break;
    case Zoom_Pos_Top:
    case Zoom_Pos_Bottom:
//        qDebug() << "Top 12 : Bottom 32";
        cursor = Qt::SizeVerCursor;
        break;
    case Zoom_Pos_Center:
        cursor = Qt::SizeAllCursor;
        break;
    default:
        //恢复鼠标指针形状
        QApplication::restoreOverrideCursor();
        break;
    }
    this->setCursor(cursor);
}

//计算在哪一列
int MyGraphicsProxyWidget::countRow(QPointF p)
{
    return (p.x()<MARGIN) ? 1:( (p.x()>(this->boundingRect().width()-MARGIN) ? 3:2) );
}

/*********** Slots begin  **************************/

void MyGraphicsProxyWidget::SlaveChoose_slaveIndex_changed(int index)
{
    if(m_isInitSlaveChoose_data){ //如果是正在添加ittem,则不处理
       return;
    }

    if(this->get_userAppPtr()->get_CallbackPtr() != nullptr){
        this->get_userAppPtr()->get_CallbackPtr()->Master_setSlaveChooseIndex(index);
        QString slave_name = this->get_userAppPtr()->get_CallbackPtr()->Master_getAddressList().at(index).slave_name;
        m_form_SlaveChoose->get_LineEdit_SlaveNamePtr()->setText(slave_name);
    }
}

/*********** Slots begin  **************************/

