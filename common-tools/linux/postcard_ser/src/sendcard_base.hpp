#ifndef SENDCARD_BASE_HPP
#define SENDCARD_BASE_HPP

extern "C" 
{
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

class CSendCardBase
{
public:
    virtual bool init() = 0;
    virtual void onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess) = 0;
    virtual void onProcPkgSer(int fd, void* pkg, int pkglen) = 0;
    virtual void onLinkDown(int fd) = 0;
    virtual ~CSendCardBase(){}
};

#endif

