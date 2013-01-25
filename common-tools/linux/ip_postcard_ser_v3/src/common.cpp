#include <fstream>
#include <common.hpp>
#include <postcard_util.hpp>

char *mail_ip;
uint16_t mail_port;

std::map<uint32_t, std::vector<ip_conf_t> > mole_ip_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > mole_ip_flags;
uid_conf_t mole_uid_conf;
std::set<uint32_t> mole_uid_flags;

std::map<uint32_t, std::vector<ip_conf_t> > seer_ip_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > seer_ip_flags;
uid_conf_t seer_uid_conf;
std::set<uint32_t> seer_uid_flags;

std::map<uint32_t, std::vector<ip_conf_t> > fairy_ip_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > fairy_ip_flags;
uid_conf_t fairy_uid_conf;
std::set<uint32_t> fairy_uid_flags;

std::map<uint32_t, std::vector<ip_conf_t> > gongfu_ip_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > gongfu_ip_flags;
uid_conf_t gongfu_uid_conf;
std::set<uint32_t> gongfu_uid_flags;

std::map<uint32_t, std::vector<ip_conf_t> > hero_ip_confs;
std::map<uint32_t, std::vector<std::set<uint32_t> > > hero_ip_flags;
uid_conf_t hero_uid_conf;
std::set<uint32_t> hero_uid_flags;

int mole_load_ip_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            uint32_t province;
            uint32_t city;
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id,cur, "type_id");
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            ip_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            conf.mail.ph.package_len = sizeof(conf.mail);
            conf.mail.ph.command_id = 1105;
            conf.mail.ph.seq_num = 0;
            conf.mail.ph.status_code = 0;
            conf.mail.ph.role_tm = 0;
            conf.mail.ph.game_flag = GAME_TYPE_MOLE;
            conf.mail.ph.game_svr = 1;
            conf.mail.ph.channel_id = 2;
            conf.mail.sender_id = sender_id;
            strncpy(conf.mail.nick, nick, sizeof(nick));
            conf.mail.send_time = time(0);
            conf.mail.type_id = type_id;
            conf.mail.title_len = 0;
            int l = strlen(msg);
            if (l > 100)
            {
                ERROR_LOG("mole msglen > 100");
                return -1;
            }
            conf.mail.msglen = l;
            memcpy(conf.mail.msg, msg, l);
            conf.mail.enclosure_cnt = 0;

            std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
            it = mole_ip_confs.find(conf.province_code);
            if (it == mole_ip_confs.end())
            {
                mole_ip_confs[conf.province_code] = std::vector<ip_conf_t>();
                mole_ip_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            
            it = mole_ip_confs.find(conf.province_code);
            
            bool need_add = true;

            for (uint32_t i = 0; i < (*it).second.size(); ++i)
            {
                if ((*it).second[i].province_code == conf.province_code &&
                    (*it).second[i].city_code == conf.city_code)
                {
                    if ((*it).second[i].modify_timestamp == conf.modify_timestamp)
                    {
                        need_add = false;
                        break;
                    }
                    else
                    {
                        (*it).second[i] = conf;
                        mole_ip_flags[conf.province_code][i].clear();
                        
                        need_add = false;
                        DEBUG_LOG("==========mole load ip conf==========");           
                        DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                        DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                        DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                        DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                        DEBUG_LOG("-------------------------------------");
                        break;
                    }
                }
            }
            if (need_add)
            {
                mole_ip_confs[conf.province_code].push_back(conf);
                mole_ip_flags[conf.province_code].push_back(std::set<uint32_t>());
                DEBUG_LOG("==========mole load ip conf==========");           
                DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int mole_load_uid_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            uid_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            if (conf.modify_timestamp != mole_uid_conf.modify_timestamp)
            {
                mole_uid_conf.modify_timestamp = conf.modify_timestamp;
                mole_uid_conf.start_time = conf.start_time;
                mole_uid_conf.stop_time = conf.stop_time;

                mole_uid_conf.mail.ph.package_len = sizeof(mole_uid_conf.mail);
                mole_uid_conf.mail.ph.command_id = 1105;
                mole_uid_conf.mail.ph.seq_num = 0;
                mole_uid_conf.mail.ph.status_code = 0;
                mole_uid_conf.mail.ph.role_tm = 0;
                mole_uid_conf.mail.ph.game_flag = GAME_TYPE_MOLE;
                mole_uid_conf.mail.ph.game_svr = 1;
                mole_uid_conf.mail.ph.channel_id = 2;
                mole_uid_conf.mail.sender_id = sender_id;
                strncpy(mole_uid_conf.mail.nick, nick, sizeof(nick));
                mole_uid_conf.mail.send_time = time(0);
                mole_uid_conf.mail.type_id = type_id;
                int l = strlen(msg);
                if (l > 100)
                {
                    ERROR_LOG("mole msglen > 100");
                    return -1;
                }
                mole_uid_conf.mail.msglen = l;
                memcpy(mole_uid_conf.mail.msg, msg, l);
                mole_uid_conf.mail.enclosure_cnt = 0;
                
                load_uid_dict("../etc/uid_mole.dict", mole_uid_flags);
                DEBUG_LOG("==========mole load uid conf==========");           
                DEBUG_LOG("load conf sender_id = %u", mole_uid_conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", mole_uid_conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", mole_uid_conf.mail.type_id);
                DEBUG_LOG("load conf msg = %s", mole_uid_conf.mail.msg);
                DEBUG_LOG("mole_uid size = %lu", mole_uid_flags.size());
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int seer_load_ip_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            uint32_t province;
            uint32_t city;
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id,cur, "type_id");
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            ip_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            conf.mail.ph.package_len = sizeof(conf.mail);
            conf.mail.ph.command_id = 1105;
            conf.mail.ph.seq_num = 0;
            conf.mail.ph.status_code = 0;
            conf.mail.ph.role_tm = 0;
            conf.mail.ph.game_flag = GAME_TYPE_SEER;
            conf.mail.ph.game_svr = 1;
            conf.mail.ph.channel_id = 2;
            conf.mail.sender_id = sender_id;
            strncpy(conf.mail.nick, nick, sizeof(nick));
            conf.mail.send_time = time(0);
            conf.mail.type_id = type_id;
            conf.mail.title_len = 0;
            int l = strlen(msg);
            if (l > 100)
            {
                ERROR_LOG("seer msglen > 100");
                return -1;
            }
            conf.mail.msglen = l;
            memcpy(conf.mail.msg, msg, l);
            conf.mail.enclosure_cnt = 0;

            std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
            it = seer_ip_confs.find(conf.province_code);
            if (it == seer_ip_confs.end())
            {
                seer_ip_confs[conf.province_code] = std::vector<ip_conf_t>();
                seer_ip_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            
            it = seer_ip_confs.find(conf.province_code);
            
            bool need_add = true;

            for (uint32_t i = 0; i < (*it).second.size(); ++i)
            {
                if ((*it).second[i].province_code == conf.province_code &&
                    (*it).second[i].city_code == conf.city_code)
                {
                    if ((*it).second[i].modify_timestamp == conf.modify_timestamp)
                    {
                        need_add = false;
                        break;
                    }
                    else
                    {
                        (*it).second[i] = conf;
                        seer_ip_flags[conf.province_code][i].clear();
                        
                        need_add = false;
                        DEBUG_LOG("==========seer load ip conf==========");           
                        DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                        DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                        DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                        DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                        DEBUG_LOG("-------------------------------------"); 
                        break;
                    }
                }
            }
            if (need_add)
            {
                seer_ip_confs[conf.province_code].push_back(conf);
                seer_ip_flags[conf.province_code].push_back(std::set<uint32_t>());
                DEBUG_LOG("==========seer load ip conf==========");           
                DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                DEBUG_LOG("-------------------------------------"); 
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int seer_load_uid_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            uid_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            if (conf.modify_timestamp != seer_uid_conf.modify_timestamp)
            {
                seer_uid_conf.modify_timestamp = conf.modify_timestamp;
                seer_uid_conf.start_time = conf.start_time;
                seer_uid_conf.stop_time = conf.stop_time;

                seer_uid_conf.mail.ph.package_len = sizeof(seer_uid_conf.mail);
                seer_uid_conf.mail.ph.command_id = 1105;
                seer_uid_conf.mail.ph.seq_num = 0;
                seer_uid_conf.mail.ph.status_code = 0;
                seer_uid_conf.mail.ph.role_tm = 0;
                seer_uid_conf.mail.ph.game_flag = GAME_TYPE_SEER;
                seer_uid_conf.mail.ph.game_svr = 1;
                seer_uid_conf.mail.ph.channel_id = 2;
                seer_uid_conf.mail.sender_id = sender_id;
                strncpy(seer_uid_conf.mail.nick, nick, sizeof(nick));
                seer_uid_conf.mail.send_time = time(0);
                seer_uid_conf.mail.type_id = type_id;
                int l = strlen(msg);
                if (l > 100)
                {
                    ERROR_LOG("seer msglen > 100");
                    return -1;
                }
                seer_uid_conf.mail.msglen = l;
                memcpy(seer_uid_conf.mail.msg, msg, l);
                seer_uid_conf.mail.enclosure_cnt = 0;
                
                load_uid_dict("../etc/uid_seer.dict", seer_uid_flags);
                DEBUG_LOG("==========seer load uid conf==========");           
                DEBUG_LOG("load conf sender_id = %u", seer_uid_conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", seer_uid_conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", seer_uid_conf.mail.type_id);
                DEBUG_LOG("load conf msg = %s", seer_uid_conf.mail.msg);
                DEBUG_LOG("seer_uid size = %lu", seer_uid_flags.size());
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int fairy_load_ip_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            uint32_t province;
            uint32_t city;
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            ip_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            conf.mail.ph.package_len = sizeof(conf.mail);
            conf.mail.ph.command_id = 1105;
            conf.mail.ph.seq_num = 0;
            conf.mail.ph.status_code = 0;
            conf.mail.ph.role_tm = 0;
            conf.mail.ph.game_flag = GAME_TYPE_FAIRY;
            conf.mail.ph.game_svr = 1;
            conf.mail.ph.channel_id = 2;
            conf.mail.sender_id = sender_id;
            strncpy(conf.mail.nick, nick, sizeof(nick));
            conf.mail.send_time = time(0);
            conf.mail.type_id = type_id;
            conf.mail.title_len = strlen(title);
            memcpy(conf.mail.title, title, conf.mail.title_len);
            int l = strlen(msg);
            if (l > 100)
            {
                ERROR_LOG("fairy msglen > 100");
                return -1;
            }
            conf.mail.msglen = l;
            memcpy(conf.mail.msg, msg, l);
            conf.mail.enclosure_cnt = 0;

            std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
            it = fairy_ip_confs.find(conf.province_code);
            if (it == fairy_ip_confs.end())
            {
                fairy_ip_confs[conf.province_code] = std::vector<ip_conf_t>();
                fairy_ip_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            
            it = fairy_ip_confs.find(conf.province_code);
            
            bool need_add = true;

            for (uint32_t i = 0; i < (*it).second.size(); ++i)
            {
                if ((*it).second[i].province_code == conf.province_code &&
                    (*it).second[i].city_code == conf.city_code)
                {
                    if ((*it).second[i].modify_timestamp == conf.modify_timestamp)
                    {
                        need_add = false;
                        break;
                    }
                    else
                    {
                        (*it).second[i] = conf;
                        fairy_ip_flags[conf.province_code][i].clear();
                        
                        need_add = false;
                        DEBUG_LOG("==========fairy load ip conf==========");           
                        DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                        DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                        DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                        DEBUG_LOG("load conf title = %s", conf.mail.title);
                        DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                        DEBUG_LOG("--------------------------------------"); 
                        break;
                    }
                }
            }
            if (need_add)
            {
                fairy_ip_confs[conf.province_code].push_back(conf);
                fairy_ip_flags[conf.province_code].push_back(std::set<uint32_t>());
                DEBUG_LOG("==========fairy load ip conf==========");           
                DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", conf.mail.title);
                DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                DEBUG_LOG("--------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int fairy_load_uid_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            uid_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            if (conf.modify_timestamp != fairy_uid_conf.modify_timestamp)
            {
                fairy_uid_conf.modify_timestamp = conf.modify_timestamp;
                fairy_uid_conf.start_time = conf.start_time;
                fairy_uid_conf.stop_time = conf.stop_time;

                fairy_uid_conf.mail.ph.package_len = sizeof(fairy_uid_conf.mail);
                fairy_uid_conf.mail.ph.command_id = 1105;
                fairy_uid_conf.mail.ph.seq_num = 0;
                fairy_uid_conf.mail.ph.status_code = 0;
                fairy_uid_conf.mail.ph.role_tm = 0;
                fairy_uid_conf.mail.ph.game_flag = GAME_TYPE_FAIRY;
                fairy_uid_conf.mail.ph.game_svr = 1;
                fairy_uid_conf.mail.ph.channel_id = 2;
                fairy_uid_conf.mail.sender_id = sender_id;
                strncpy(fairy_uid_conf.mail.nick, nick, sizeof(nick));
                fairy_uid_conf.mail.send_time = time(0);
                fairy_uid_conf.mail.type_id = type_id;
                fairy_uid_conf.mail.title_len = strlen(title);
                memcpy(fairy_uid_conf.mail.title, title, fairy_uid_conf.mail.title_len);
                int l = strlen(msg);
                if (l > 100)
                {
                    ERROR_LOG("fairy msglen > 100");
                    return -1;
                }
                fairy_uid_conf.mail.msglen = l;
                memcpy(fairy_uid_conf.mail.msg, msg, l);
                fairy_uid_conf.mail.enclosure_cnt = 0;
                
                load_uid_dict("../etc/uid_fairy.dict", fairy_uid_flags);
                DEBUG_LOG("==========fairy load uid conf==========");           
                DEBUG_LOG("load conf sender_id = %u", fairy_uid_conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", fairy_uid_conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", fairy_uid_conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", fairy_uid_conf.mail.title);
                DEBUG_LOG("load conf msg = %s", fairy_uid_conf.mail.msg);
                DEBUG_LOG("fairy_uid size = %lu", fairy_uid_flags.size());
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int gongfu_load_ip_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            uint32_t province;
            uint32_t city;
            
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            ip_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            conf.mail.ph.package_len = sizeof(conf.mail);
            conf.mail.ph.command_id = 1105;
            conf.mail.ph.seq_num = 0;
            conf.mail.ph.status_code = 0;
            conf.mail.ph.role_tm = 0;
            conf.mail.ph.game_flag = GAME_TYPE_GONGFU;
            conf.mail.ph.game_svr = 1;
            conf.mail.ph.channel_id = 2;
            conf.mail.sender_id = 0;
            conf.mail.send_time = time(0);
            conf.mail.type_id = type_id;
            conf.mail.title_len = strlen(title);
            memcpy(conf.mail.title, title, conf.mail.title_len);
            int l = strlen(msg);
            if (l > 100)
            {
                ERROR_LOG("gongfu msglen > 100");
                return -1;
            }
            conf.mail.msglen = l;
            memcpy(conf.mail.msg, msg, l);
            conf.mail.enclosure_cnt = 0;

            std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
            it = gongfu_ip_confs.find(conf.province_code);
            if (it == fairy_ip_confs.end())
            {
                gongfu_ip_confs[conf.province_code] = std::vector<ip_conf_t>();
                gongfu_ip_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            
            it = gongfu_ip_confs.find(conf.province_code);
            
            bool need_add = true;

            for (uint32_t i = 0; i < (*it).second.size(); ++i)
            {
                if ((*it).second[i].province_code == conf.province_code &&
                    (*it).second[i].city_code == conf.city_code)
                {
                    if ((*it).second[i].modify_timestamp == conf.modify_timestamp)
                    {
                        need_add = false;
                        break;
                    }
                    else
                    {
                        (*it).second[i] = conf;
                        fairy_ip_flags[conf.province_code][i].clear();
                        
                        need_add = false;
                        DEBUG_LOG("==========gongfu load ip conf==========");           
                        DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                        DEBUG_LOG("load conf title = %s", conf.mail.title);
                        DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                        DEBUG_LOG("---------------------------------------"); 
                        break;
                    }
                }
            }
            if (need_add)
            {
                gongfu_ip_confs[conf.province_code].push_back(conf);
                gongfu_ip_flags[conf.province_code].push_back(std::set<uint32_t>());
                DEBUG_LOG("==========gongfu load ip conf==========");           
                DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", conf.mail.title);
                DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                DEBUG_LOG("---------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int gongfu_load_uid_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");

            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            uid_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            if (conf.modify_timestamp != gongfu_uid_conf.modify_timestamp)
            {
                gongfu_uid_conf.modify_timestamp = conf.modify_timestamp;
                gongfu_uid_conf.start_time = conf.start_time;
                gongfu_uid_conf.stop_time = conf.stop_time;

                gongfu_uid_conf.mail.ph.package_len = sizeof(gongfu_uid_conf.mail);
                gongfu_uid_conf.mail.ph.command_id = 1105;
                gongfu_uid_conf.mail.ph.seq_num = 0;
                gongfu_uid_conf.mail.ph.status_code = 0;
                gongfu_uid_conf.mail.ph.role_tm = 0;
                gongfu_uid_conf.mail.ph.game_flag = GAME_TYPE_GONGFU;
                gongfu_uid_conf.mail.ph.game_svr = 1;
                gongfu_uid_conf.mail.ph.channel_id = 2;
                gongfu_uid_conf.mail.sender_id = 0;
                gongfu_uid_conf.mail.send_time = time(0);
                gongfu_uid_conf.mail.type_id = type_id;
                gongfu_uid_conf.mail.title_len = strlen(title);
                memcpy(gongfu_uid_conf.mail.title, title, gongfu_uid_conf.mail.title_len);
                int l = strlen(msg);
                if (l > 100)
                {
                    ERROR_LOG("gongfu msglen > 100");
                    return -1;
                }
                gongfu_uid_conf.mail.msglen = l;
                memcpy(gongfu_uid_conf.mail.msg, msg, l);
                gongfu_uid_conf.mail.enclosure_cnt = 0;
                
                load_uid_dict("../etc/uid_gongfu.dict", gongfu_uid_flags);
                DEBUG_LOG("==========gongfu load uid conf==========");           
                DEBUG_LOG("load conf type_id = %u", gongfu_uid_conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", gongfu_uid_conf.mail.title);
                DEBUG_LOG("load conf msg = %s", gongfu_uid_conf.mail.msg);
                DEBUG_LOG("gongfu_uid size = %lu", gongfu_uid_flags.size());
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int hero_load_ip_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            uint32_t province;
            uint32_t city;
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");
            DECODE_XML_PROP_UINT32(province,cur, "province");
            DECODE_XML_PROP_UINT32(city,    cur, "city");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            ip_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            conf.province_code = province;
            conf.city_code = city;

            conf.mail.ph.package_len = sizeof(conf.mail);
            conf.mail.ph.command_id = 1105;
            conf.mail.ph.seq_num = 0;
            conf.mail.ph.status_code = 0;
            conf.mail.ph.role_tm = 0;
            conf.mail.ph.game_flag = GAME_TYPE_HERO;
            conf.mail.ph.game_svr = 1;
            conf.mail.ph.channel_id = 2;
            conf.mail.sender_id = sender_id;
            strncpy(conf.mail.nick, nick, sizeof(nick));
            conf.mail.send_time = time(0);
            conf.mail.type_id = type_id;
            conf.mail.title_len = strlen(title);
            memcpy(conf.mail.title, title, conf.mail.title_len);
            int l = strlen(msg);
            if (l > 100)
            {
                ERROR_LOG("hero msglen > 100");
                return -1;
            }
            conf.mail.msglen = l;
            memcpy(conf.mail.msg, msg, l);
            conf.mail.enclosure_cnt = 0;

            std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
            it = hero_ip_confs.find(conf.province_code);
            if (it == hero_ip_confs.end())
            {
                hero_ip_confs[conf.province_code] = std::vector<ip_conf_t>();
                hero_ip_flags[conf.province_code] = std::vector<std::set<uint32_t> >();
            }
            
            it = hero_ip_confs.find(conf.province_code);
            
            bool need_add = true;

            for (uint32_t i = 0; i < (*it).second.size(); ++i)
            {
                if ((*it).second[i].province_code == conf.province_code &&
                    (*it).second[i].city_code == conf.city_code)
                {
                    if ((*it).second[i].modify_timestamp == conf.modify_timestamp)
                    {
                        need_add = false;
                        break;
                    }
                    else
                    {
                        (*it).second[i] = conf;
                        hero_ip_flags[conf.province_code][i].clear();
                        
                        need_add = false;
                        DEBUG_LOG("==========hero load ip conf==========");           
                        DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                        DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                        DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                        DEBUG_LOG("load conf title = %s", conf.mail.title);
                        DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                        DEBUG_LOG("-------------------------------------");
                        break;
                    }
                }
            }
            if (need_add)
            {
                hero_ip_confs[conf.province_code].push_back(conf);
                hero_ip_flags[conf.province_code].push_back(std::set<uint32_t>());
                DEBUG_LOG("==========hero load ip conf==========");           
                DEBUG_LOG("load conf sender_id = %u", conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", conf.mail.title);
                DEBUG_LOG("load conf msg = %s", conf.mail.msg);
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

int hero_load_uid_xml(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur)
    {
        if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("card")))
        {
            char modify_timestamp[64];
            char start_time[64];
            char stop_time[64];
            
            uint32_t    sender_id;
            char        nick[16];
            uint32_t    type_id;
            char        title[40];
            char        msg[100];

            DECODE_XML_PROP_STR(modify_timestamp, cur, "modify_timestamp");
            DECODE_XML_PROP_STR(start_time,       cur, "start_time");
            DECODE_XML_PROP_STR(stop_time,        cur, "stop_time");

            DECODE_XML_PROP_UINT32(sender_id,    cur, "sender_id");
            DECODE_XML_PROP_STR(nick, cur, "nick"); 
            DECODE_XML_PROP_UINT32(type_id, cur, "type_id");
            DECODE_XML_PROP_STR(title, cur, "title");            
            DECODE_XML_PROP_STR(msg,        cur, "msg");

            uid_conf_t conf;
            memset(&conf, 0, sizeof(conf));
            
            if (!stringToTime(modify_timestamp, conf.modify_timestamp))
            {
                ERROR_LOG("read modify_timestamp time failed");
                return -1;
            }
            if (!stringToTime(start_time, conf.start_time))
            {
                ERROR_LOG("read start_time time failed");
                return -1;
            }
            if (!stringToTime(stop_time, conf.stop_time))
            {
                ERROR_LOG("read stop_time time failed");
                return -1;
            }

            if (conf.modify_timestamp != hero_uid_conf.modify_timestamp)
            {
                hero_uid_conf.modify_timestamp = conf.modify_timestamp;
                hero_uid_conf.start_time = conf.start_time;
                hero_uid_conf.stop_time = conf.stop_time;

                hero_uid_conf.mail.ph.package_len = sizeof(hero_uid_conf.mail);
                hero_uid_conf.mail.ph.command_id = 1105;
                hero_uid_conf.mail.ph.seq_num = 0;
                hero_uid_conf.mail.ph.status_code = 0;
                hero_uid_conf.mail.ph.role_tm = 0;
                hero_uid_conf.mail.ph.game_flag = GAME_TYPE_HERO;
                hero_uid_conf.mail.ph.game_svr = 1;
                hero_uid_conf.mail.ph.channel_id = 2;
                hero_uid_conf.mail.sender_id = sender_id;
                strncpy(hero_uid_conf.mail.nick, nick, sizeof(nick));
                hero_uid_conf.mail.send_time = time(0);
                hero_uid_conf.mail.type_id = type_id;
                hero_uid_conf.mail.title_len = strlen(title);
                memcpy(hero_uid_conf.mail.title, title, hero_uid_conf.mail.title_len);
                int l = strlen(msg);
                if (l > 100)
                {
                    ERROR_LOG("hero msglen > 100");
                    return -1;
                }
                hero_uid_conf.mail.msglen = l;
                memcpy(hero_uid_conf.mail.msg, msg, l);
                hero_uid_conf.mail.enclosure_cnt = 0;
                
                load_uid_dict("../etc/uid_hero.dict", hero_uid_flags);
                DEBUG_LOG("==========hero load uid conf==========");           
                DEBUG_LOG("load conf sender_id = %u", hero_uid_conf.mail.sender_id); 
                DEBUG_LOG("load conf nick = %s", hero_uid_conf.mail.nick);             
                DEBUG_LOG("load conf type_id = %u", hero_uid_conf.mail.type_id);
                DEBUG_LOG("load conf title = %s", hero_uid_conf.mail.title);
                DEBUG_LOG("load conf msg = %s", hero_uid_conf.mail.msg);
                DEBUG_LOG("hero_uid size = %lu", hero_uid_flags.size());
                DEBUG_LOG("-------------------------------------");
            }
        }
        cur = cur->next;
    }

    return 0; 
}

void load_uid_dict(const char *filename, std::set<uint32_t> &uid_flags)
{
    uid_flags.clear();

    std::ifstream ifs;
    ifs.open(filename);
    if (!ifs.is_open())
        return;

    while (!ifs.eof())
    {
        uint32_t uid = 0;
        ifs >> uid;
        
        if (ifs.eof())
            break;
        
        if (uid != 0)
            uid_flags.insert(uid);
    }
}

void clear_mole_timeoutrule(time_t t)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    for (it = mole_ip_confs.begin(); it != mole_ip_confs.end(); ++it)
    {
        std::vector<ip_conf_t> vector_conf;
        std::vector<std::set<uint32_t> > vector_set;
        
        for (uint32_t i = 0; i < (*it).second.size(); ++i)
        {
            if (t > (*it).second[i].start_time && t < (*it).second[i].stop_time)
            {
                vector_conf.push_back((*it).second[i]);
                vector_set.push_back(mole_ip_flags[(*it).first][i]);
            }
        }
        
        (*it).second = vector_conf;
        mole_ip_flags[(*it).first] = vector_set;
    }
    
    if (t < mole_uid_conf.start_time || t > mole_uid_conf.stop_time)
    {
        memset(&mole_uid_conf, 0, sizeof(mole_uid_conf));
        mole_uid_flags.clear();
    }
}

void clear_seer_timeoutrule(time_t t)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    for (it = seer_ip_confs.begin(); it != seer_ip_confs.end(); ++it)
    {
        std::vector<ip_conf_t> vector_conf;
        std::vector<std::set<uint32_t> > vector_set;
        
        for (uint32_t i = 0; i < (*it).second.size(); ++i)
        {
            if (t > (*it).second[i].start_time && t < (*it).second[i].stop_time)
            {
                vector_conf.push_back((*it).second[i]);
                vector_set.push_back(seer_ip_flags[(*it).first][i]);
            }
        }
        
        (*it).second = vector_conf;
        seer_ip_flags[(*it).first] = vector_set;
    }
    
    if (t < seer_uid_conf.start_time || t > seer_uid_conf.stop_time)
    {
        memset(&seer_uid_conf, 0, sizeof(seer_uid_conf));
        seer_uid_flags.clear();
    }
}

void clear_fairy_timeoutrule(time_t t)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    for (it = fairy_ip_confs.begin(); it != fairy_ip_confs.end(); ++it)
    {
        std::vector<ip_conf_t> vector_conf;
        std::vector<std::set<uint32_t> > vector_set;
        
        for (uint32_t i = 0; i < (*it).second.size(); ++i)
        {
            if (t > (*it).second[i].start_time && t < (*it).second[i].stop_time)
            {
                vector_conf.push_back((*it).second[i]);
                vector_set.push_back(fairy_ip_flags[(*it).first][i]);
            }
        }
        
        (*it).second = vector_conf;
        fairy_ip_flags[(*it).first] = vector_set;
    }
    
    if (t < fairy_uid_conf.start_time || t > fairy_uid_conf.stop_time)
    {
        memset(&fairy_uid_conf, 0, sizeof(fairy_uid_conf));
        fairy_uid_flags.clear();
    }
}

void clear_gongfu_timeoutrule(time_t t)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    for (it = gongfu_ip_confs.begin(); it != gongfu_ip_confs.end(); ++it)
    {
        std::vector<ip_conf_t> vector_conf;
        std::vector<std::set<uint32_t> > vector_set;
        
        for (uint32_t i = 0; i < (*it).second.size(); ++i)
        {
            if (t > (*it).second[i].start_time && t < (*it).second[i].stop_time)
            {
                vector_conf.push_back((*it).second[i]);
                vector_set.push_back(gongfu_ip_flags[(*it).first][i]);
            }
        }
        
        (*it).second = vector_conf;
        gongfu_ip_flags[(*it).first] = vector_set;
    }
    
    if (t < gongfu_uid_conf.start_time || t > gongfu_uid_conf.stop_time)
    {
        memset(&gongfu_uid_conf, 0, sizeof(gongfu_uid_conf));
        gongfu_uid_flags.clear();
    }
}

void clear_hero_timeoutrule(time_t t)
{
    std::map<uint32_t, std::vector<ip_conf_t> >::iterator it;
    for (it = hero_ip_confs.begin(); it != hero_ip_confs.end(); ++it)
    {
        std::vector<ip_conf_t> vector_conf;
        std::vector<std::set<uint32_t> > vector_set;
        
        for (uint32_t i = 0; i < (*it).second.size(); ++i)
        {
            if (t > (*it).second[i].start_time && t < (*it).second[i].stop_time)
            {
                vector_conf.push_back((*it).second[i]);
                vector_set.push_back(hero_ip_flags[(*it).first][i]);
            }
        }
        
        (*it).second = vector_conf;
        hero_ip_flags[(*it).first] = vector_set;
    }
    
    if (t < hero_uid_conf.start_time || t > hero_uid_conf.stop_time)
    {
        memset(&hero_uid_conf, 0, sizeof(hero_uid_conf));
        hero_uid_flags.clear();
    }
}
