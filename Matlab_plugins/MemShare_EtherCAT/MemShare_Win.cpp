#include "MemShare_Win.h"

#include <QDebug>

MemShare_Win::MemShare_Win():MemShare_EtherCAT()
{
   this->MemShare_set_key("Local\\Master_MappingObject");

   m_hMapFile = nullptr;
   m_hMapBaseAddr = nullptr;
}

MemShare_Win::~MemShare_Win()
{

}

char *MemShare_Win::MemShare_get_baseAddr()
{
   return m_hMapBaseAddr;
}

void MemShare_Win::MemShare_set_baseAddr(char *addr)
{
   m_hMapBaseAddr = addr;
}

void MemShare_Win::MemShare_release_baseAddr()
{
    if(m_hMapBaseAddr != nullptr){
        UnmapViewOfFile(m_hMapBaseAddr);
        CloseHandle(m_hMapFile);

        m_hMapBaseAddr = nullptr;
        m_hMapFile = nullptr;
    }
}

bool MemShare_Win::MemShare_baseAddr_isRelease()
{
    if(m_hMapBaseAddr == nullptr){
        return true;
    }
    else{
        return false;
    }
}

char *MemShare_Win::MemShare_get_key()
{
    return m_hMapFile_key;
}

void MemShare_Win::MemShare_set_key(const char key_name[])
{
    strcpy(m_hMapFile_key, key_name);
}

bool MemShare_Win::MemShare_Init_cores()
{
    //原来 Global\\ 保证创建的对象是全局的，对权限要求比较高，而使用Local可以保证服务对象仅对当前用户中的进程和线程可见。这就是总是返回权限失败问题的原因。
//    TCHAR szName[]=TEXT("Local\\Master_MappingObject");    //指向同一块共享内存的名字
    TCHAR szName[200];
    #ifdef UNICODE
        MultiByteToWideChar(CP_ACP, 0, m_hMapFile_key, -1, szName, 200);
    #else
        strcpy(Name, strUsr);
    #endif
    m_hMapFile = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, szName);
    if (nullptr == m_hMapFile)
    {   // 创建共享文件句柄
        m_hMapFile = CreateFileMapping(
               INVALID_HANDLE_VALUE,    // 物理文件句柄
               NULL,                    // 默认安全级别
               PAGE_READWRITE,          // 可读可写
               0,                       // 高位文件大小
               this->MemShare_get_memSize(),                // 地位文件大小
               szName                   // 共享内存名称
               );

        if(m_hMapFile == nullptr){
            qDebug() << "m_hMapFile == nullptr";
           return false;
        }
    }

    m_hMapBaseAddr = (char *)MapViewOfFile(
          m_hMapFile,            // 共享内存的句柄
          FILE_MAP_ALL_ACCESS, // 可读写许可
          0,
          0,
          this->MemShare_get_memSize()
          );

    if(m_hMapBaseAddr == nullptr){
        qDebug() << "m_hMapBaseAddr == nullptr";

        CloseHandle(m_hMapFile);
        m_hMapFile = nullptr;

       return false;
    }



    return true;
}

bool MemShare_Win::MemShare_Destroy_cores()
{
    if(m_hMapBaseAddr != nullptr){
        UnmapViewOfFile(m_hMapBaseAddr);
        CloseHandle(m_hMapFile);

        m_hMapBaseAddr = nullptr;
        m_hMapFile = nullptr;
    }

    return true;
}
