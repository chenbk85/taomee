#include "fwd_decl.hpp"
#include "player.hpp"


/* hold common timers */
timer_head_t g_events;

/* pathname where to store the statistic log info */
char* statistic_logfile;
/*! event manager */
taomee::EventMgr ev_mgr;


bool SpecialTimeMrg::is_in_time(uint32_t time_id,time_t tv_sec)
{
	struct tm  *ptime = localtime(&tv_sec);
	ptime->tm_hour = 0;
	ptime->tm_min = 0;
	ptime->tm_sec = 0;
	
	time_t today_from = mktime(ptime);
	ptime = localtime(&tv_sec);
	uint32_t week_day = ptime->tm_wday;
//	uint32_t hour = ptime->tm_hour;
//	uint32_t min = ptime->tm_min;

	std::map<uint32_t, SpecialDropTimeInfo>::iterator it = time_limit_map.find(time_id);
	if ( it != time_limit_map.end()) {
		SpecialDropTimeInfo* pTimeInfo = &(it->second);
		if (pTimeInfo->time_type == 1) {
			std::vector<mon_drop_time_limit_t>::iterator it = pTimeInfo->limit_info_arr.begin();
			for (; it != pTimeInfo->limit_info_arr.end(); ++it) {
				if (it->week_day == week_day  && it->from_date_sec + today_from < tv_sec &&
					it->to_date_sec + today_from > tv_sec) {
					return true;
				}
			}
		} else if (pTimeInfo->time_type == 2) {
			std::vector<mon_drop_time_limit_t>::iterator it = pTimeInfo->limit_info_arr.begin();
			for (; it != pTimeInfo->limit_info_arr.end(); ++it) {
				if (it->from_date_sec + today_from < tv_sec &&
					it->to_date_sec + today_from > tv_sec) {
					return true;
				}
			}
		}
	}
	return false;
}

void SpecialTimeMrg::init()
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile("./conf/special_drop_time.xml");
	if(doc == NULL){
		throw XmlParseError(std::string("the stage xml file is not exist"));
		return ;
	}
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the stage xml file content is empty"));
		return ;
	}

	load_special_time(root);

}

int SpecialTimeMrg::load_special_time(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Time"))) {
			SpecialDropTimeInfo TimeInfo;
			get_xml_prop(TimeInfo.id, cur, "ID");
			get_xml_prop(TimeInfo.time_type, cur, "Type");
		
			xmlNodePtr chl = cur->xmlChildrenNode;
			uint32_t i = 0;
			
			while(chl) {
				if (!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar*>("Limit"))) {
					uint32_t week, from, to;
					get_xml_prop(week, chl, "WeekDay");
					get_xml_prop(from, chl, "From");
					get_xml_prop(to, chl, "To");

					mon_drop_time_limit_t time_limit;
					time_limit.week_day = week;
					time_limit.from_date_sec = 
						from / 100 * 60 * 60 + from % 100 * 60;
					time_limit.to_date_sec = 
						to / 100 * 60 * 60 + to % 100 * 60;	

					TimeInfo.limit_info_arr.push_back(time_limit);
					i ++;
				}
				chl = chl->next;	
			}
			time_limit_map.insert(std::map<uint32_t, SpecialDropTimeInfo>::value_type(TimeInfo.id, TimeInfo));
		}
		cur = cur->next;	
	}
	return 0;

}


