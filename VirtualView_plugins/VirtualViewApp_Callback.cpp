#include "VirtualViewApp_Callback.h"

VirtualViewApp_Callback::VirtualViewApp_Callback(QObject *parent):QObject(parent),Ethercat_Callback()
{
    m_graphicsView = nullptr;
    m_isAppScan_allow_read = true;//设置扫描期间可以读取数据
    m_appScan_readTimeout = 6000;//设置读取的延迟
}

VirtualViewApp_Callback::~VirtualViewApp_Callback()
{
    m_graphicsView = nullptr;
}

void VirtualViewApp_Callback::Master_AppLoop_callback()
{
    m_graphicsView->Master_AppLoop_callback();
}

void VirtualViewApp_Callback::Master_AppStart_callback()
{
    m_graphicsView->Master_setAdressBase(this->m_Master_addressBase);
    m_graphicsView->Master_setSlaveCount(this->m_slaveCount);
    m_graphicsView->Master_setAddressList(this->m_Master_addressList);

    m_graphicsView->Master_AppStart_callback();
}

void VirtualViewApp_Callback::Master_AppStop_callback()
{
    m_graphicsView->Master_AppStop_callback();

    this->Master_ReleaseAddress();
}

void VirtualViewApp_Callback::Master_AppScan_callback()
{
    m_graphicsView->Master_setAdressBase(this->m_Master_addressBase);
    m_graphicsView->Master_setSlaveCount(this->m_slaveCount);
    m_graphicsView->Master_setAddressList(this->m_Master_addressList);

    m_graphicsView->Master_AppScan_callback();
}
