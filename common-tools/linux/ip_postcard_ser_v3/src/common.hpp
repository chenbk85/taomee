#ifndef COMMON_HPP
#define COMMON_HPP

#include <stdint.h>
#include <map>
#include <set>
#include <vector>
#include <xmlutils.hpp>
#include <proto_head.hpp>

struct ip_conf_t
{
    time_t modify_timestamp;
    time_t start_time;
    time_t stop_time;
    uint32_t province_code;
    uint32_t city_code;

    mail_proto_t mail;
};

struct uid_conf_t
{
    time_t modify_timestamp;
    time_t start_time;
    time_t stop_time;

    mail_proto_t mail;
};

extern char *mail_ip;
extern uint16_t mail_port;

extern std::map<uint32_t, std::vector<ip_conf_t> > mole_ip_confs;
extern std::map<uint32_t, std::vector<std::set<uint32_t> > > mole_ip_flags;
extern uid_conf_t mole_uid_conf;
extern std::set<uint32_t> mole_uid_flags;

extern std::map<uint32_t, std::vector<ip_conf_t> > seer_ip_confs;
extern std::map<uint32_t, std::vector<std::set<uint32_t> > > seer_ip_flags;
extern uid_conf_t seer_uid_conf;
extern std::set<uint32_t> seer_uid_flags;

extern std::map<uint32_t, std::vector<ip_conf_t> > fairy_ip_confs;
extern std::map<uint32_t, std::vector<std::set<uint32_t> > > fairy_ip_flags;
extern uid_conf_t fairy_uid_conf;
extern std::set<uint32_t> fairy_uid_flags;

extern std::map<uint32_t, std::vector<ip_conf_t> > gongfu_ip_confs;
extern std::map<uint32_t, std::vector<std::set<uint32_t> > > gongfu_ip_flags;
extern uid_conf_t gongfu_uid_conf;
extern std::set<uint32_t> gongfu_uid_flags;

extern std::map<uint32_t, std::vector<ip_conf_t> > hero_ip_confs;
extern std::map<uint32_t, std::vector<std::set<uint32_t> > > hero_ip_flags;
extern uid_conf_t hero_uid_conf;
extern std::set<uint32_t> hero_uid_flags;

extern int mole_load_ip_xml(xmlNodePtr cur);
extern int mole_load_uid_xml(xmlNodePtr cur);
extern int seer_load_ip_xml(xmlNodePtr cur);
extern int seer_load_uid_xml(xmlNodePtr cur);
extern int fairy_load_ip_xml(xmlNodePtr cur);
extern int fairy_load_uid_xml(xmlNodePtr cur);
extern int gongfu_load_ip_xml(xmlNodePtr cur);
extern int gongfu_load_uid_xml(xmlNodePtr cur);
extern int hero_load_ip_xml(xmlNodePtr cur);
extern int hero_load_uid_xml(xmlNodePtr cur);

extern void load_uid_dict(const char *filename, std::set<uint32_t> &uid_flags);

extern void clear_mole_timeoutrule(time_t t);
extern void clear_seer_timeoutrule(time_t t);
extern void clear_fairy_timeoutrule(time_t t);
extern void clear_gongfu_timeoutrule(time_t t);
extern void clear_hero_timeoutrule(time_t t);

#endif
