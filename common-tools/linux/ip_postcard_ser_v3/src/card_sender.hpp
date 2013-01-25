#ifndef CARD_SENDER_HPP
#define CARD_SENDER_HPP

extern "C" 
{
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
}
#include <ip_dict.hpp>
#include <common.hpp>

class CCardSender
{
public:
    bool init();
    void onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess);
    void onProcPkgSer(int fd, void* pkg, int pkglen);
    void onLinkDown(int fd);
    ~CCardSender(){}
    
    void onPostCard(void* pkg, int pkglen);
    void onReload(void* pkg, int pkglen);
    
    void checkTimeout();

private:
    void sendCardByIp(userid_t userid, uint16_t game_zone, 
                   const code_t &code, 
                   std::map<uint32_t, std::vector<ip_conf_t> > &ip_confs,
                   std::map<uint32_t, std::vector<std::set<uint32_t> > > &ip_flags);
    void sendCardByUid(userid_t userid, uint16_t game_zone,
                   uid_conf_t &uid_conf, std::set<uint32_t> &uid_flags);
    
    void onMolePostCard(uint32_t uid, post_card_t *pc);
    void onSeerPostCard(uint32_t uid, post_card_t *pc);
    void onFairyPostCard(uint32_t uid, post_card_t *pc);
    void onGongFuPostCard(uint32_t uid, post_card_t *pc);
    void onHeroPostCard(uint32_t uid, post_card_t *pc);

private:
    time_t m_next_time;
    int m_mail_fd;
};

#endif

