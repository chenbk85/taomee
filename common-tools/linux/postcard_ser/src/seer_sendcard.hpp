#ifndef SEER_SENDCARD_HPP
#define SEER_SENDCARD_HPP

#include <vector>
#include <map>
#include <set>

extern "C" 
{
#include <libtaomee/project/types.h>
}
#include "sendcard_base.hpp"
#include "xmlutils.hpp"
#include "ip_dict.hpp"

class CSeerSendCard : public CSendCardBase
{
public:
    virtual bool init();
    virtual void onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess);
    virtual void onProcPkgSer(int fd, void* pkg, int pkglen);
    virtual void onLinkDown(int fd);

private:
    void sendCard(userid_t userid, const code_t &code);
    static int load_cache_xml(xmlNodePtr cur);

private:
    int m_proxy_fd;
    int m_switch_fd;

    struct conf_t
    {
        time_t begin;
        time_t end;
        uint32_t province_code;
        uint32_t city_code;
        uint32_t proto_len;
        char buf[8192];
    };

    static std::map<uint32_t, std::vector<conf_t> > m_confs;
    static std::map<uint32_t, std::vector<std::set<uint32_t> > > m_flags;
};

#endif

