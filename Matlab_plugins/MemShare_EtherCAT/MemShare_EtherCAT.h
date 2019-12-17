#ifndef MEMSHARE_ETHERCAT_H
#define MEMSHARE_ETHERCAT_H

class MemShare_EtherCAT
{
public:
    MemShare_EtherCAT() { m_shareMem_size = 1025;}
    virtual ~MemShare_EtherCAT(){}

    virtual char *MemShare_get_baseAddr() = 0;
    virtual void MemShare_set_baseAddr(char *addr) = 0;

    virtual void MemShare_release_baseAddr() = 0;
    virtual bool MemShare_baseAddr_isRelease() = 0;

    virtual char* MemShare_get_key() = 0;
    virtual void MemShare_set_key(const char key_name[]) = 0;

    virtual bool MemShare_Init_cores() = 0;
    virtual bool MemShare_Destroy_cores() = 0;

    virtual int MemShare_get_memSize() { return m_shareMem_size;}
    virtual void MemShare_set_memSize(int size) { m_shareMem_size = size; }
protected:
    int m_shareMem_size;
};

#endif // MEMSHARE_ETHERCAT_H
