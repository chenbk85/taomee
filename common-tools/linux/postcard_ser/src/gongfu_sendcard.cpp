#include "postcard_util.hpp"
#include "gongfu_sendcard.hpp"
#include "proto_head.hpp"
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

#define MAX_MAIL_TITLE_LEN 40
#define MAX_MAIL_BODY_LEN 100

struct gf_send_system_mail_in
{
    uint32_t sender_id;
    uint32_t receive_id;
    uint32_t mail_templet;
    uint32_t max_mail_limit;
    char     mail_title[MAX_MAIL_TITLE_LEN+1];
    char     mail_body[MAX_MAIL_BODY_LEN + 1];
    char     mail_num_enclosure[1024];
    char     mail_item_enclosure[1024];
    char     mail_equip_enclosure[1024];
}__attribute__((packed));

std::map<uint32_t, std::vector<CGongfuSendCard::conf_t> > CGongfuSendCard::m_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > CGongfuSendCard::m_flags;

bool CGongfuSendCard::init()
{
    m_proxy_fd = -1;
    m_switch_fd = -1;

    if (load_xmlconf("../etc/gongfu.xml", CGongfuSendCard::load_cache_xml) == -1)
    {
        ERROR_LOG("load gongfu.xml failed");
        return false;
    }

    return true;
}

void CGongfuSendCard::onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess)
{
    if (pkglen != PROTO_HEAD_LEN + 4)
    {
        ERROR_LOG("invalid pkg len pkglen = %u", pkglen);
        return;
    }

    proto_head_t *ph = (proto_head_t*)pkg;
    uint32_t *ip = (uint32_t*)((char*)pkg + PROTO_HEAD_LEN);

    switch (ph->cmd)
    {
    case 0xC080:
        {
            code_t code;
            if (!g_ipDict.find(*ip, code))
            {
                DEBUG_LOG("ipdict can't find ip=%u.%u.%u.%u ip = %u",
                      *(((char*)ip) + 0),
                      *(((char*)ip) + 1),
                      *(((char*)ip) + 2),
                      *(((char*)ip) + 3));
                return;
            }
            DEBUG_LOG("cmd:%u uid:%u ip=%u.%u.%u.%u", ph->cmd, ph->id,
                      *(((char*)ip) + 0),
                      *(((char*)ip) + 1),
                      *(((char*)ip) + 2),
                      *(((char*)ip) + 3));    

            sendCard(ph->id, code);
            break;
        }
    default:
        ERROR_LOG("invalid cmd:%u", ph->cmd);
        break;
    }

    return;
}

void CGongfuSendCard::sendCard(userid_t userid, const code_t &code)
{
    std::map<uint32_t, std::vector<conf_t> >::iterator it;
    it = m_confs.find(code.province_code);
    if (it == m_confs.end())
        return;

    time_t cur = time(0);

    for (uint32_t i = 0; i < (*it).second.size(); ++i)
    {
        if ((*it).second[i].city_code == 0 || (*it).second[i].city_code == code.city_code)
        {
            if (cur < (*it).second[i].begin || cur > (*it).second[i].end)
                continue;
            
            std::map<uint32_t, std::vector<std::set<uint32_t> > >::iterator its;
            its = m_flags.find(code.province_code);
            if (its == m_flags.end())
            {
                ERROR_LOG("flags can't find code.province_code");
                continue; 
            }

            // 已经发过就不发了.
            if ((*its).second[i].find(userid) != (*its).second[i].end())
            {
                continue;
            }

            if (m_proxy_fd == -1)
            {
                DEBUG_LOG("m_proxy_fd == -1 connect_to_service");
                m_proxy_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
            }

            proto_head_t *ph = (proto_head_t *)((*it).second[i].buf);
            ph->len = (*it).second[i].proto_len;
            ph->id = userid;
            // receive_id
            gf_send_system_mail_in *p = (gf_send_system_mail_in*)((*it).second[i].buf + PROTO_HEAD_LEN);
            p->receive_id = userid;            

            if (-1 == net_send(m_proxy_fd, (char*)ph, ph->len))
            {
                ERROR_LOG("net_send error uid=%u", ph->id);
            }
            else
            {
                DEBUG_LOG("sendcard uid=%u", ph->id);
                (*its).second[i].insert(userid);
            }
        }
    }
}

void CGongfuSendCard::onProcPkgSer(int fd, void* pkg, int pkglen)
{
    if (m_proxy_fd == fd)
    {
        if ((uint32_t)pkglen < PROTO_HEAD_LEN)
        {
            ERROR_LOG("proxy return len error");
            return;
        }

        proto_head_t *ph = (proto_head_t *)(pkg);
        if (ph->ret != 0)
        {
            ERROR_LOG("proxy ret=%u cmd=%u uid=%u", ph->ret, ph->cmd, ph->id);
            return;
        }
    }
    else if (m_switch_fd == fd)
    {
    }
}

void CGongfuSendCard::onLinkDown(int fd)
{
    if (m_proxy_fd == fd)
    {
        DEBUG_LOG("proxy link down");
        m_proxy_fd = -1;
    }
    else if (m_switch_fd == fd)
    {
        m_switch_fd = -1;
    }
    else
        return;
}

int CGongfuSendCard::load_cache_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char begin[64];
            char end[64];
            uint32_t province;
            uint32_t city;

            gf_send_system_mail_in gssm;
            memset(&gssm, 0, sizeof(gssm));

            DECODE_XML_PROP_STR(begin,      cur, "begin");
            DECODE_XML_PROP_STR(end,        cur, "end");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(gssm.mail_templet, cur, "templet");
            DECODE_XML_PROP_STR(gssm.mail_title,      cur, "title");
            DECODE_XML_PROP_STR(gssm.mail_body,       cur, "body");

            conf_t conf;
            memset(&conf, 0, sizeof(conf));

            if (!stringToTime(begin, conf.begin))
            {
                ERROR_LOG("read begin time failed");
                return -1;
            }
            if (!stringToTime(end, conf.end))
            {
                ERROR_LOG("read end time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            proto_head_t *ph = (proto_head_t *)(conf.buf);
            ph->seq = 0;
            ph->cmd = 0x063A;
            ph->ret = 0;
            
            DEBUG_LOG("==========load conf==========");           
            DEBUG_LOG("load conf templet = %u", gssm.mail_templet);
            DEBUG_LOG("load conf title = %s", gssm.mail_title);
            DEBUG_LOG("load conf body = %s", gssm.mail_body);

            int p = sizeof(proto_head_t);
          
            gssm.sender_id = 0;
            gssm.receive_id = 0;
            gssm.max_mail_limit = 100;
            
            memcpy(&conf.buf[p], &gssm, sizeof(gssm));
            p += sizeof(gssm);

            conf.proto_len = p;

            std::map<uint32_t, std::vector<conf_t> >::iterator it;
            it = m_confs.find(conf.province_code);
            if (it == m_confs.end())
            {
                m_confs[conf.province_code] = std::vector<conf_t>();
                m_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            m_confs[conf.province_code].push_back(conf);
            m_flags[conf.province_code].push_back(std::set<uint32_t>());
        }
        cur = cur->next;
    }

    return 0; 
}

