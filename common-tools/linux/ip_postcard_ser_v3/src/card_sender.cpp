#include <card_sender.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C"
{
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
}

bool CCardSender::init()
{
    time_t t = time(0);
    m_next_time = (t + 86400) - (t % 86400);
    m_mail_fd = -1;

    mail_ip = config_get_strval("mail_ip");
    if (!mail_ip)
    {
        ERROR_LOG("conf mail_ip failed");
        return false;
    }

    mail_port = config_get_intval("mail_port", 0);
    if (mail_port == 0)
    {
        ERROR_LOG("conf mail_port failed");
        return false;    
    }

    if (load_xmlconf("../etc/ip_mole.xml", mole_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_mole.xml failed");
        return false;
    }
    if (load_xmlconf("../etc/uid_mole.xml", mole_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_mole.xml failed");
        return false;
    }
    
    if (load_xmlconf("../etc/ip_seer.xml", seer_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_seer.xml failed");
        return false;
    }
    if (load_xmlconf("../etc/uid_seer.xml", seer_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_seer.xml failed");
        return false;
    }
    
    if (load_xmlconf("../etc/ip_fairy.xml", fairy_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_fairy.xml failed");
        return false;
    }
    if (load_xmlconf("../etc/uid_fairy.xml", fairy_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_fairy.xml failed");
        return false;
    }
    
    if (load_xmlconf("../etc/ip_gongfu.xml", gongfu_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_gongfu.xml failed");
        return false;
    }
    if (load_xmlconf("../etc/uid_gongfu.xml", gongfu_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_gongfu.xml failed");
        return false;
    }
    
    if (load_xmlconf("../etc/ip_hero.xml", hero_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_hero.xml failed");
        return false;
    }
    if (load_xmlconf("../etc/uid_hero.xml", hero_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_hero.xml failed");
        return false;
    }
    
    return true;
}

void CCardSender::onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess)
{
    online_proto_head_t *ph = (online_proto_head_t*)pkg;
    if (ph->len != (uint32_t)pkglen)
    {
        DEBUG_LOG("pkg len error");
        return;
    }

    online_proto_head_t ret;
    ret.len = sizeof(online_proto_head_t);
    ret.seq = ph->seq;
    ret.cmd = ph->cmd;
    ret.ret = 0;
    ret.id = ph->id;

    switch (ph->cmd)
    {
    case 0xC080:
        onPostCard(pkg, pkglen);
        break;
    case 0xC081:
        onReload(pkg, pkglen);
        break;
    default:
        ERROR_LOG("recv error cmd = %u", ph->cmd);
        break;
    }

    send_pkg_to_client(fdsess, &ret, ret.len);
}

void CCardSender::onProcPkgSer(int fd, void* pkg, int pkglen)
{
    if ((uint32_t)pkglen < PROTO_HEAD_LEN)
    {
        ERROR_LOG("proxy return len error");
        return;
    }

    proto_head_t *ph = (proto_head_t *)(pkg);
    if (ph->status_code != 0)
    {
        ERROR_LOG("proxy ret=%u cmd=%u uid=%u", ph->status_code, ph->command_id, ph->user_id);
        return;
    }
}

void CCardSender::onLinkDown(int fd)
{
    if (m_mail_fd == fd)
    {
        m_mail_fd = -1;
    }
}

void CCardSender::onPostCard(void* pkg, int pkglen)
{
    if (pkglen != ONLINE_PROTO_HEAD_LEN + sizeof(post_card_t))
    {
        ERROR_LOG("onPostCard pkglen error pkglen = %u", pkglen);
        return;
    }

    online_proto_head_t *ph = (online_proto_head_t*)pkg;
    post_card_t *pc = (post_card_t *)((uint8_t*)pkg + ONLINE_PROTO_HEAD_LEN);
    switch (pc->game_type)
    {
    case GAME_TYPE_MOLE:
        onMolePostCard(ph->id, pc);
        break;
    case GAME_TYPE_SEER:
        onSeerPostCard(ph->id, pc);
        break;
    case GAME_TYPE_FAIRY:
        onFairyPostCard(ph->id, pc);
        break;
    case GAME_TYPE_GONGFU:
        onGongFuPostCard(ph->id, pc);
        break;
    case GAME_TYPE_HERO:
        onHeroPostCard(ph->id, pc);
        break;
    default:
        ERROR_LOG("error onPostCard game_type = %u", pc->game_type);
        break;
    }
}

void CCardSender::onReload(void* pkg, int pkglen)
{
    if (pkglen != ONLINE_PROTO_HEAD_LEN)
    {
        ERROR_LOG("onReload pkglen error pkglen = %u", pkglen);
        return;
    }

    if (load_xmlconf("../etc/ip_mole.xml", mole_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_mole.xml failed");
    }
    if (load_xmlconf("../etc/uid_mole.xml", mole_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_mole.xml failed");
    }
    
    if (load_xmlconf("../etc/ip_seer.xml", seer_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_seer.xml failed");
    }
    if (load_xmlconf("../etc/uid_seer.xml", seer_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_seer.xml failed");
    }
    
    if (load_xmlconf("../etc/ip_fairy.xml", fairy_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_fairy.xml failed");
    }
    if (load_xmlconf("../etc/uid_fairy.xml", fairy_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_fairy.xml failed");
    }
    
    if (load_xmlconf("../etc/ip_gongfu.xml", gongfu_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_gongfu.xml failed");
    }
    if (load_xmlconf("../etc/uid_gongfu.xml", gongfu_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_gongfu.xml failed");
    }
    
    if (load_xmlconf("../etc/ip_hero.xml", hero_load_ip_xml) == -1)
    {
        ERROR_LOG("load ip_hero.xml failed");
    }
    if (load_xmlconf("../etc/uid_hero.xml", hero_load_uid_xml) == -1)
    {
        ERROR_LOG("load uid_hero.xml failed");
    }
}

void CCardSender::checkTimeout()
{
    time_t t = time(0);
    if (t > m_next_time)
    {
        m_next_time = (t + 86400) - (t % 86400);    
        clear_mole_timeoutrule(t);
        clear_seer_timeoutrule(t);
        clear_fairy_timeoutrule(t);
        clear_gongfu_timeoutrule(t);
        clear_hero_timeoutrule(t);
    }
}

void CCardSender::sendCardByIp(userid_t userid, uint16_t game_zone, 
                               const code_t &code, 
                               std::map<uint32_t, std::vector<ip_conf_t> > &ip_confs,
                               std::map<uint32_t, std::vector<std::set<uint32_t> > > &ip_flags)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    it = ip_confs.find(code.province_code);
    if (it == ip_confs.end())
        return;

    time_t cur = time(0);

    for (uint32_t i = 0; i < (*it).second.size(); ++i)
    {
        if ((*it).second[i].city_code == 0 || (*it).second[i].city_code == code.city_code)
        {
            if (cur < (*it).second[i].start_time || cur > (*it).second[i].stop_time)
            {
                DEBUG_LOG("not in start and stop");
                continue;
            }
            
            std::map<uint32_t, std::vector<std::set<uint32_t> > >::iterator its;
            its = ip_flags.find(code.province_code);
            if (its == ip_flags.end())
            {
                ERROR_LOG("flags can't find code.province_code");
                continue; 
            }

            // 已经发过就不发了.
            if ((*its).second[i].find(userid) != (*its).second[i].end())
            {
                continue;
            }

            if (m_mail_fd == -1)
            {
                DEBUG_LOG("m_mail_fd == -1 connect_to_svr");
                m_mail_fd = connect_to_svr(mail_ip, mail_port, 65535, 1);
                if (m_mail_fd == -1)
                {
                    ERROR_LOG("connect to proxy error");
                    continue;
                }
            }

            (*it).second[i].mail.ph.user_id = userid;
            (*it).second[i].mail.ph.game_zone = game_zone;
            if (-1 == net_send(m_mail_fd, (char*)(&((*it).second[i].mail)),
                               (*it).second[i].mail.ph.package_len))
            {
                ERROR_LOG("net_send error");
            }
            else
            {
                INFO_LOG("sendcard by ip uid=%u, game=%d", (*it).second[i].mail.ph.user_id, (*it).second[i].mail.ph.game_flag);
                (*its).second[i].insert(userid);
            }
        }
    }
}

void CCardSender::sendCardByUid(userid_t userid, uint16_t game_zone,
                   uid_conf_t &uid_conf, std::set<uint32_t> &uid_flags)
{
    if (uid_conf.modify_timestamp == 0)
        return;
    
    std::set<uint32_t>::iterator it = uid_flags.find(userid);
    if (it == uid_flags.end())
        return;

    if (m_mail_fd == -1)
    {
        DEBUG_LOG("m_mail_fd == -1 connect_to_svr");
        m_mail_fd = connect_to_svr(mail_ip, mail_port, 65535, 1);
        if (m_mail_fd == -1)
        {
            ERROR_LOG("connect to proxy error");
            return;
        }
    }

    uid_conf.mail.ph.user_id = userid;
    uid_conf.mail.ph.game_zone = game_zone;
    if (-1 == net_send(m_mail_fd, (char*)(&uid_conf.mail),
                       uid_conf.mail.ph.package_len))
    {
        ERROR_LOG("net_send error");
        return;
    }
    
    INFO_LOG("sendcard by uid uid=%u, game=%d", uid_conf.mail.ph.user_id, uid_conf.mail.ph.game_flag);
    uid_flags.erase(it);
}

void CCardSender::onMolePostCard(uint32_t uid, post_card_t *pc)
{
    code_t code;
    struct in_addr addr;
    addr.s_addr = pc->ip_address; 
    
    DEBUG_LOG("mole proc uid:%u ip=%s", uid, inet_ntoa(addr));

    sendCardByUid(uid, pc->game_zone, mole_uid_conf, mole_uid_flags);    
    
    if (!g_ipDict.find(pc->ip_address, code))
    {
        DEBUG_LOG("ipdict can't find ip=%s", inet_ntoa(addr)); 

        return;
    }

    sendCardByIp(uid, pc->game_zone, code, mole_ip_confs, mole_ip_flags);
}

void CCardSender::onSeerPostCard(uint32_t uid, post_card_t *pc)
{
    code_t code;
    struct in_addr addr;
    addr.s_addr = pc->ip_address; 
    
    DEBUG_LOG("seer proc uid:%u ip=%s", uid, inet_ntoa(addr));

    sendCardByUid(uid, pc->game_zone, seer_uid_conf, seer_uid_flags);    
    
    if (!g_ipDict.find(pc->ip_address, code))
    {
        DEBUG_LOG("ipdict can't find ip=%s", inet_ntoa(addr)); 

        return;
    }

    sendCardByIp(uid, pc->game_zone, code, seer_ip_confs, seer_ip_flags);
}

void CCardSender::onFairyPostCard(uint32_t uid, post_card_t *pc)
{
    code_t code;
    struct in_addr addr;
    addr.s_addr = pc->ip_address; 
    
    DEBUG_LOG("fairy proc uid:%u ip=%s", uid, inet_ntoa(addr));

    sendCardByUid(uid, pc->game_zone, fairy_uid_conf, fairy_uid_flags);    
    
    if (!g_ipDict.find(pc->ip_address, code))
    {
        DEBUG_LOG("ipdict can't find ip=%s", inet_ntoa(addr)); 

        return;
    }

    sendCardByIp(uid, pc->game_zone, code, fairy_ip_confs, fairy_ip_flags);
}

void CCardSender::onGongFuPostCard(uint32_t uid, post_card_t *pc)
{
    code_t code;
    struct in_addr addr;
    addr.s_addr = pc->ip_address; 
    
    DEBUG_LOG("gongfu proc uid:%u ip=%s", uid, inet_ntoa(addr));

    sendCardByUid(uid, pc->game_zone, gongfu_uid_conf, gongfu_uid_flags);    
    
    if (!g_ipDict.find(pc->ip_address, code))
    {
        DEBUG_LOG("ipdict can't find ip=%s", inet_ntoa(addr)); 

        return;
    }

    sendCardByIp(uid, pc->game_zone, code, gongfu_ip_confs, gongfu_ip_flags);
}

void CCardSender::onHeroPostCard(uint32_t uid, post_card_t *pc)
{
    code_t code;
    struct in_addr addr;
    addr.s_addr = pc->ip_address; 
    
    DEBUG_LOG("hero proc uid:%u ip=%s", uid, inet_ntoa(addr));

    sendCardByUid(uid, pc->game_zone, hero_uid_conf, hero_uid_flags);    
    
    if (!g_ipDict.find(pc->ip_address, code))
    {
        DEBUG_LOG("ipdict can't find ip=%s", inet_ntoa(addr)); 

        return;
    }

    sendCardByIp(uid, pc->game_zone, code, hero_ip_confs, hero_ip_flags);
}
