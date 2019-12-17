#ifndef MEMSHARE_WIN_H
#define MEMSHARE_WIN_H

#include "MemShare_EtherCAT.h"

#include <Windows.h>

class MemShare_Win : public MemShare_EtherCAT
{
public:
    MemShare_Win();
    virtual ~MemShare_Win();

    virtual char *MemShare_get_baseAddr();
    virtual void MemShare_set_baseAddr(char *addr);

    virtual void MemShare_release_baseAddr();
    virtual bool MemShare_baseAddr_isRelease();

    virtual char* MemShare_get_key();
    virtual void MemShare_set_key(const char key_name[]);

    virtual bool MemShare_Init_cores();
    virtual bool MemShare_Destroy_cores();

private:
    HANDLE m_hMapFile;
    char *m_hMapBaseAddr;
    char m_hMapFile_key[100];
};

#endif // MEMSHARE_WIN_H
