#include "postcard_util.hpp"
#include "mole_sendcard.hpp"
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

std::map<uint32_t, std::vector<CMoleSendCard::conf_t> > CMoleSendCard::m_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > CMoleSendCard::m_flags;

bool CMoleSendCard::init()
{
    m_proxy_fd = -1;
    m_switch_fd = -1;

    if (load_xmlconf("../etc/mole.xml", CMoleSendCard::load_cache_xml) == -1)
    {
        ERROR_LOG("load mole.xml failed");
        return false;
    }

    return true;
}

void CMoleSendCard::onProcPkgCli(void* pkg, int pkglen, fdsession_t* fdsess)
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
                DEBUG_LOG("ipdict can't find ip=%d.%d.%d.%d", 
                      *((uint8_t*)ip + 0), 
                      *((uint8_t*)ip + 1),
                      *((uint8_t*)ip + 2),
                      *((uint8_t*)ip + 3));
                return;
            }
            DEBUG_LOG("cmd:%u uid:%u ip=%d.%d.%d.%d", ph->cmd, ph->id, 
                      *((uint8_t*)ip + 0), 
                      *((uint8_t*)ip + 1),
                      *((uint8_t*)ip + 2),
                      *((uint8_t*)ip + 3));
 
            //DEBUG_LOG("sendCard"); 
            sendCard(ph->id, code);
            break;
        }
    default:
        ERROR_LOG("invalid cmd:%u", ph->cmd);
        break;
    }

    return;
}

void CMoleSendCard::sendCard(userid_t userid, const code_t &code)
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
            {           
                DEBUG_LOG("post card timeout uid:%u", userid); 
                continue;
            }

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
                DEBUG_LOG("is send uid = %u", userid);
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

void CMoleSendCard::onProcPkgSer(int fd, void* pkg, int pkglen)
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

void CMoleSendCard::onLinkDown(int fd)
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

int CMoleSendCard::load_cache_xml(xmlNodePtr cur)
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
            uint32_t type;
            uint32_t senderid;
            char sendernick[16];
            uint32_t mapid;
            char msg[4096];

            DECODE_XML_PROP_STR(begin,      cur, "begin");
            DECODE_XML_PROP_STR(end,        cur, "end");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");
            DECODE_XML_PROP_UINT32(type,    cur, "type");
            DECODE_XML_PROP_UINT32(senderid,cur, "senderid");
            DECODE_XML_PROP_STR(sendernick, cur, "sendernick");
            DECODE_XML_PROP_UINT32(mapid,   cur, "mapid");
            DECODE_XML_PROP_STR(msg,        cur, "msg");

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
            ph->cmd = 0xE101;
            ph->ret = 0;
            
            DEBUG_LOG("==========load conf==========");           

            int p = sizeof(proto_head_t);
           
            *(uint32_t *)(&conf.buf[p]) = type;
            p += sizeof(uint32_t);
            DEBUG_LOG("load conf type = %u", type);           

            time_t t = time(0);
            *(uint32_t *)(&conf.buf[p]) = t;
            p += sizeof(uint32_t);

            *(uint32_t *)(&conf.buf[p]) = senderid;
            p += sizeof(uint32_t);
            DEBUG_LOG("load conf senderid = %u", senderid);

            strncpy(&conf.buf[p], sendernick, sizeof(sendernick)); 
            p += sizeof(sendernick);
            DEBUG_LOG("load conf sendernick = %s", sendernick);           
 
            *(uint32_t *)(&conf.buf[p]) = mapid;
            p += sizeof(uint32_t);
            DEBUG_LOG("load conf mapid = %u", mapid);

            int l = strlen(msg);
            *(uint32_t *)(&conf.buf[p]) = l;
            p += sizeof(uint32_t);
            DEBUG_LOG("load conf msg_len = %u", l);

            memcpy(&conf.buf[p], msg, l);
            p += l;
            DEBUG_LOG("load conf msg = %s", msg);

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

