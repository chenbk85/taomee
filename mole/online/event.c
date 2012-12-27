#include <libtaomee/time/time.h>
#include <statistic_agent/msglog.h>

#include <libtaomee/log.h>
#include "proto.h"
#include "util.h"
#include "central_online.h"
#include "mole_pasture.h"

#include "event.h"

static char holiday_map[7]={0,0,0,0,0,0,0};

static event_t all_events[EVENT_max_events];
static timer_cb_func_t event_handlers[EVENT_max_events];

uint32_t show_status = 0;

// Event Conf File Parser
static int load_event_config(xmlNodePtr cur_node);
// Utils
static inline int   pack_event(event_t* ev, void* buf);
static inline void  send_to_affected_maps(map_t* maps[], void* buf, int len);
static inline void  setup_event_handlers();

static char CloDiscotDay[12][10] = {"20100123", "20100220", "20100320", "20100417", "20100522", "20100619", "20100724", "20100821", "20100918", "20101023", "20101120", "20101218"};
static char FurDiscotDay[12][10] = {"20100109", "20100206", "20100306", "20100403", "20100508", "20100605", "20100710", "20100807", "20100912", "20101009", "20101106", "20101204"};

//------------------ Event Handlers Begin ------------------
int event_dispatcher(sprite_t* sp, uint32_t evid)
{
	event_t* ev = get_event(evid);
	if (!ev) {
		ERROR_RETURN(("Invalid EventID=%u Uid=%u", evid, sp->id), -1);
	}
	return event_handlers[ev->type](sp, &evid);
}

static int event_handler_0(void* owner, void* data)
{
	event_t* ev;

	if (!data) {
		ev           = owner;
		ev->status   = ++(ev->status) % ev->max_status;
		time_t exptm = now.tv_sec + ev->duration[ev->status];
		ev->tmr      = ADD_TIMER_EVENT(ev, event_handler_0, 0, exptm);
	} else {
		ev = get_event(*(uint32_t*)data);
	}

	int len = pack_event(ev, msg);

	if (!data) {
		if ( !(ev->affected_map[0]) ) {
			send_to_all_idle(msg, len, ev->idle_time);
		} else {
			send_to_affected_maps(ev->affected_map, msg, len);
		}
	} else {
		send_to_self(owner, msg, len, 1);
	}

	return 0;
}

// TODO - Can Only Deal With Events Which Owns Only 2 kinds of Status
static int event_handler_1(void* owner, void* data)
{
	event_t* ev;

	if (!data) {
		ev           = owner;
		ev->status   = ++(ev->status) % ev->max_status;

		time_t exptm;
		if (!ev->status) {
			struct tm tm_tmp = tm_cur;
			tm_tmp.tm_hour = ev->start_time;
			tm_tmp.tm_min  = 0;
			tm_tmp.tm_sec  = 0;
			exptm          = mktime(&tm_tmp);
			tm_tmp.tm_hour = ev->end_time;
			time_t endtm   = mktime(&tm_tmp);
			if (exptm < now.tv_sec) {         // If Event Has Begun
				if (endtm > now.tv_sec) { // and Hasn't Ended
					ev->status = ++(ev->status) % ev->max_status;
					exptm      = endtm;
				} else { // Event Has Ended
					exptm += 86400;
				}
			}
		} else {
			exptm = now.tv_sec + 3600 * (ev->end_time - ev->start_time);
		}

		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_1, 0, exptm);
	} else {
		ev = get_event(*(uint32_t*)data);
	}

	int len = pack_event(ev, msg);

	if (!data) {
		send_to_affected_maps(ev->affected_map, msg, len);
	} else {
		send_to_self(owner, msg, len, 1);
	}

	return 0;
}

// TODO - Hourly Event Only - Can handle Only Time Info event currently
static int event_handler_2(void* owner, void* data)
{
	event_t* ev;

	if (!data) {
		ev           = owner;
		ev->status   = tm_cur.tm_hour;
		time_t exptm = mk_integral_tm_hr(tm_cur) + ev->duration[0];
		ev->tmr      = ADD_TIMER_EVENT(ev, event_handler_2, 0, exptm);
	} else {
		ev = get_event(*(uint32_t*)data);
	}

	int len = pack_event(ev, msg);

	if (!data) {
		send_to_affected_maps(ev->affected_map, msg, len);
//-----------------------------------
		static map_id_t maps[11] = {8, 9, 10, 16, 35, 37, 51, 52, 77, 47, 61};
		notify_systime(0, maps);
//-----------------------------------
	} else {
		send_to_self(owner, msg, len, 1);
	}

	return 0;
}

// Kick All Users Offline
static int event_handler_3(void* owner, void* data)
{
	event_t* ev      = owner;
	struct tm tm_tmp = *get_now_tm();

	if (!data) {
		tm_tmp.tm_hour = 23;
		tm_tmp.tm_min  = 54;
		tm_tmp.tm_sec  = 0;
		time_t exptm   = mktime(&tm_tmp);

		//exptm += 86400;
#if 0
		if (tm_tmp.tm_wday != 4) {
			exptm += 86400;
		} else {
			exptm += 79200;
		}

		if (exptm < get_now_tv()->tv_sec) {
			exptm += 93600;
		}
#endif
		DEBUG_LOG("KICK OFFLINE TIME\t[exptm=%lu]", exptm);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_3, (void*)1, exptm);
	} else if ( data == (void*)1 ) {
		time_t exptm = get_now_tv()->tv_sec;
#if 0
		if (tm_tmp.tm_wday == 4) {
			exptm += ((tm_tmp.tm_hour == 0) ? 79200 : 93600);
		} else {
			exptm += 86400;
		}
#endif

		exptm += 86400;
		DEBUG_LOG("KICK OFFLINE TIME\t[exptm=%lu]", exptm);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_3, (void*)1, exptm);
		//
		exptm = get_now_tv()->tv_sec + (60 * 5);
		ADD_TIMER_EVENT(ev, kick_all_users_offline, (void*)0x00100002, exptm);
	} else {
		ERROR_RETURN( ("Invalid Req to Internal Event %d", 3), -1 );
	}

	return 0;
}
static inline int
pack_event_4(event_t* ev, void* buf, int year, int mon, int day1, int day2)
{
	uint32_t tmdiff = ev->tmr->expire - now.tv_sec;;
	if (!ev->status) {
		if (day1<day2) {
			tmdiff += 86400*(day2-day1-1);
		} else {
			struct tm future;
			time_t t_of_day;
			future.tm_year = year-((mon == 12)?1899:1900);
			future.tm_mon = (mon == 12)?0:(mon);
			future.tm_mday = day2;
			future.tm_hour = 0;
			future.tm_min = 0;
			future.tm_sec = 0;
			t_of_day = mktime(&future);

			tmdiff = difftime(t_of_day, get_now_tv()->tv_sec);
		}
	}
	int len = sizeof(protocol_t);
	PKG_UINT32(buf, ev->id, len);
	PKG_UINT32(buf, ev->status, len);
	PKG_UINT32(buf, tmdiff, len);
	init_proto_head(buf, PROTO_EVENT_STATUS, len);

	return len;
}
int event_handler_4_ex(void* owner, void* data)
{
	event_t* ev;
	struct tm tm_now, tm_just_now;
	time_t tval;
	time(&tval);
	tm_just_now = tm_now = *localtime(&tval);

	int year = tm_now.tm_year + 1900;
	int mon = tm_now.tm_mon+1;
	int day = tm_now.tm_mday;
	char future[9] = {0};

	if (!data) {
		ev  = owner;
		strncpy(future, ev->id== 5?CloDiscotDay[mon-1]:
				    (ev->id == 10?FurDiscotDay[mon-1]:"foo"), 8);
		char tmp[9] = {0};
		sprintf(tmp, "%d%02d%02d", year,mon, day);
		if (!strcmp(future, tmp)) {
			ev->status = 1;
		} else {
			ev->status = 0;
		}
		tm_just_now.tm_hour = 0;
		tm_just_now.tm_min  = 0;
		tm_just_now.tm_sec  = 0;
		tval = mktime(&tm_just_now);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_4_ex, 0, tval+86400);
		DEBUG_LOG("STAT EVENT_4_EX TIMER\t[mon=%u  day=%u]", mon, day);
		DEBUG_LOG("STAT EVENT_4_EX TIMER\t[status=%u	  day=%s  type=%u]", ev->status, tmp, ev->id);
	}else {
		ev = get_event(*(uint32_t*)data);
	}
	int day_future = atoi(future+6);
	if (day >= day_future) {
		memset(future, 0, 9);
		strncpy(future, ev->id== 5?CloDiscotDay[mon]:
				    (ev->id == 10?FurDiscotDay[mon]:"foo"), 8);
		day_future = atoi(future+6);
	}
	int len = pack_event_4(ev, msg, year, mon, day, day_future);

	if (!data) {
		if ( !(ev->affected_map[0]) ) {
			send_to_all(msg, len);
		} else {
			send_to_affected_maps(ev->affected_map, msg, len);
		}
	} else {
		send_to_self(owner, msg, len, 1);
	}
	return 0;

}

// Event Which Uses Day as its Time Unit - Can Only Deal With Events Which Owns Only 2 kinds of Status
static int event_handler_4(void* owner, void* data)
{
	event_t* ev;

	if (!data) {
		ev           = owner;
		ev->status   = ++(ev->status) % ev->max_status;

		struct tm tm_tmp = *get_now_tm();
		time_t now_sec   = get_now_tv()->tv_sec, exptm;
		if (!ev->status) {
			exptm        = mk_integral_tm_day(tm_tmp, ev->start_day, 0);
			time_t endtm = mk_integral_tm_day(tm_tmp, ev->end_day, 0);
			if (exptm < now_sec) {         // If Event Has Begun
				if (endtm > now_sec) { // and Hasn't Ended
					ev->status = ++(ev->status) % ev->max_status;
					exptm      = endtm;
				} else { // Event Has Ended
					exptm = mk_integral_tm_day(tm_tmp, ev->start_day, 1);
				}
			}
		} else {
			exptm = mk_integral_tm_day(tm_tmp, ev->end_day, 0);
		}
		DEBUG_LOG("DISCOUNT\t[status=%d exptm=%ld]", ev->status, exptm);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_4, 0, exptm);
	} else {
		ev = get_event(*(uint32_t*)data);
	}

	int len = pack_event(ev, msg);

	if (!data) {
		if ( !(ev->affected_map[0]) ) {
			send_to_all(msg, len);
		} else {
			send_to_affected_maps(ev->affected_map, msg, len);
		}
	} else {
		send_to_self(owner, msg, len, 1);
	}

	return 0;
}


static int event_handler_5(void* owner, void* data)
{
	event_t* ev      = owner;
	struct tm tm_tmp = *get_now_tm();

	struct tm tm_now = tm_tmp;

	tm_tmp.tm_sec  = 1;
	time_t exptm   = mktime(&tm_tmp);

	if (!data) {
		exptm += 60;
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_5, (void*)1, exptm);
	}else if ( data == (void*)1 ) {
		exptm += 60;
		//DEBUG_LOG("STAT PLAYER NUM\t[exptm=%lu count=%u]", exptm, sprites_count);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_5, (void*)1, exptm);

		{
			uint32_t animal[7][2] = {{0, 480}, {1, 450}, {2, 430}, {3, 420}, {4, 410}, {5, 460}, {6, 470}};
			item_t * itm_man = get_item_prop(190257);
			itm_man->sell_price = animal[tm_now.tm_wday][1];

			uint32_t plant[7][2] = {{0, 240}, {1, 220}, {2, 210}, {3, 210}, {4, 210}, {5, 230}, {6, 240}};
			item_t * itm_pepper_man = get_item_prop(190167);
			itm_pepper_man->sell_price = plant[tm_now.tm_wday][1];
		}

		//show open-close
		DEBUG_LOG("SHOW STATUS\t[exptm=%lu count=%u]", exptm, show_status);
		int tm_wday = get_now_tm()->tm_wday;
		//if (tm_now.tm_wday == 0 || tm_now.tm_wday == 6) {
		 if(is_holiday(tm_wday)) {
			if ((tm_now.tm_min == 0 && tm_now.tm_hour == 13) || (tm_now.tm_min == 0 && tm_now.tm_hour == 19)) {
				show_status = 1;
			} else if((tm_now.tm_min == 0 && tm_now.tm_hour == 15) || (tm_now.tm_min == 0 && tm_now.tm_hour == 21)) {
				show_status = 0;
			}
			int i = sizeof(protocol_t);
			PKG_UINT32(msg, show_status, i);
			init_proto_head(msg, PROTO_BROADCAST_SHOW_STAT, i);
			send_to_map3(35, msg, i);
			DEBUG_LOG("SHOW STATUS\t[exptm=%lu count=%u]", exptm, show_status);
		}
	} else {
		ERROR_RETURN( ("Invalid Req to Internal Event %d", 5), -1 );
	}

	return 0;
}

/*
*@ statistic the number of players per minute
*/
static int event_handler_6(void* owner, void* data)
{
	event_t* ev      = owner;
	struct tm tm_tmp = *get_now_tm();
	time_t exptm   = mktime(&tm_tmp);

	if (!data) {
		exptm += 30;
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_6, (void*)1, exptm);
	}else if ( data == (void*)1 ) {
		exptm += 30;
		send_to_central_online(COCMD_keep_alive, 0, 0, NULL, 0);
		//DEBUG_LOG("keepalive with switch\t[exptm=%lu]", exptm);
		ev->tmr = ADD_TIMER_EVENT(ev, event_handler_6, (void*)1, exptm);
	} else {
		ERROR_RETURN( ("Invalid Req to Internal Event %d", 5), -1 );
	}


	return 0;
}

// TODO - Hourly Event Only - youyou add animal every hour
static int event_handler_7(void* owner, void* data)
{
	event_t* ev;

	ev           = owner;
	ev->status   = tm_cur.tm_hour;
	time_t exptm = mk_integral_tm_hr(tm_cur) + 3600;
	ev->tmr      = ADD_TIMER_EVENT(ev, event_handler_7, 0, exptm);

	if (config_cache.bc_elem->online_id == 3)
		db_add_animal_num(0, 1270007, 20000, 30000);

	return 0;
}

//------------------ Event Handlers End ------------------


//------------------ Parse Event Conf File Begin ------------------
int load_events(const char* file)
{
	setup_event_handlers();

	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) ERROR_RETURN(("Failed to Load Events Config"), -1);

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	err = load_event_config(cur);
exit:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load Events File %s", file);
}

void destroy_events()
{
	int i = 0;
	for (; (i != EVENT_max_events) && (all_events[i].id == i); ++i ) {
		REMOVE_TIMERS(&(all_events[i]));
	}
}

static int load_event_config(xmlNodePtr cur_node)
{
	event_t* ev;
	uint32_t eventid;

	cur_node = cur_node->xmlChildrenNode;
	while (cur_node) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Event")) {
			DECODE_XML_PROP_UINT32(eventid, cur_node, "ID");
			if (eventid >= EVENT_max_events) {
				ERROR_RETURN( ("Failed to Parse Event Conf File, EventID=%u", eventid), -1 );
			}

			ev      = &(all_events[eventid]);
			ev->id  = eventid;
			INIT_LIST_HEAD(&ev->timer_list);

			DECODE_XML_PROP_INT(ev->type, cur_node, "Type");
			DECODE_XML_PROP_INT_DEFAULT(ev->status, cur_node, "Status", 0);
			DECODE_XML_PROP_INT(ev->max_status, cur_node, "MaxStatus");
			DECODE_XML_PROP_INT_DEFAULT(ev->start_day, cur_node, "StartDay", 0);
			DECODE_XML_PROP_INT_DEFAULT(ev->end_day, cur_node, "EndDay", 0);
			DECODE_XML_PROP_INT_DEFAULT(ev->start_time, cur_node, "StartTime", 0);
			DECODE_XML_PROP_INT_DEFAULT(ev->end_time, cur_node, "EndTime", 0);
			decode_xml_prop_arr_int_default(ev->duration, EVENT_max_duration_num, cur_node, "Durations", 0);
			DECODE_XML_PROP_INT_DEFAULT(ev->idle_time, cur_node, "IdleTime", -1);
			// load affected maps
			xmlChar* str;
			DECODE_XML_PROP(cur_node, "AffectedMaps", str);
			int i = 0, cnt = 0, k, mid;
			size_t slen = strlen((const char*)str);
			for (; (i != EVENT_max_affected_map) && (cnt != slen); ++i, cnt += k) {
				sscanf((const char*)str + cnt, "%d%n", &mid, &k);
				ev->affected_map[i] = get_map(mid);
			}
			xmlFree(str);

			event_handlers[ev->type](ev, 0);
		}
		cur_node = cur_node->next;
	}

	return 0;
}
//------------------ Parse Event Conf File End ------------------

//------------------ Utils Begin ------------------
event_t* get_event(uint32_t evid)
{
	return (((evid < EVENT_max_events) && (all_events[evid].id == evid)) ? &(all_events[evid]) : 0);
}

static inline int
pack_event(event_t* ev, void* buf)
{
	uint32_t tmdiff = ev->tmr->expire - now.tv_sec;

	int len = sizeof(protocol_t);
	PKG_UINT32(buf, ev->id, len);
	PKG_UINT32(buf, ev->status, len);
	PKG_UINT32(buf, tmdiff, len);
	init_proto_head(buf, PROTO_EVENT_STATUS, len);

	//DEBUG_LOG("send event\t[%u %u]", ev->id, ev->status);
	return len;
}

static inline void
send_to_affected_maps(map_t* maps[], void* buf, int len)
{
	int i = 0;
	for (; (i != EVENT_max_affected_map) && maps[i]; ++i) {
		if (maps[i]->sprite_num) {
			send_to_map2(maps[i], buf, len);
		}
	}
}

static inline void
setup_event_handlers()
{
	event_handlers[0] = event_handler_0;
	event_handlers[1] = event_handler_1;
	event_handlers[2] = event_handler_2;
	event_handlers[3] = event_handler_3;
	//event_handlers[4] = event_handler_4;
	event_handlers[4] = event_handler_4_ex;
	event_handlers[5] = event_handler_5;
	event_handlers[6] = event_handler_6;
	event_handlers[7] = event_handler_7;
}

int load_xml_holidays(const char *file)
{
	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) ERROR_RETURN(("Failed to Load Holidays Config"), -1);

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while ( cur ) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Holidays")) {
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[0], cur, "Sunday", 1);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[1], cur, "Monday", 0);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[2], cur, "Tuesday", 0);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[3], cur, "Wednesday", 0);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[4], cur, "Thursday", 0);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[5], cur, "Friday", 1);
			DECODE_XML_PROP_INT_DEFAULT(holiday_map[6], cur, "Saturday", 1);
			err = 0;
		} else if (!xmlStrcmp(cur->name, (const xmlChar *)"CloDiscotDays")) {
			DECODE_XML_PROP_STR(CloDiscotDay[0], cur, "Jan");
			DECODE_XML_PROP_STR(CloDiscotDay[1], cur, "Feb");
			DECODE_XML_PROP_STR(CloDiscotDay[2], cur, "Mar");
			DECODE_XML_PROP_STR(CloDiscotDay[3], cur, "Apr");
			DECODE_XML_PROP_STR(CloDiscotDay[4], cur, "May");
			DECODE_XML_PROP_STR(CloDiscotDay[5], cur, "Jun");
			DECODE_XML_PROP_STR(CloDiscotDay[6], cur, "Jul");
			DECODE_XML_PROP_STR(CloDiscotDay[7], cur, "Aug");
			DECODE_XML_PROP_STR(CloDiscotDay[8], cur, "Sep");
			DECODE_XML_PROP_STR(CloDiscotDay[9], cur, "Oct");
			DECODE_XML_PROP_STR(CloDiscotDay[10], cur, "Nov");
			DECODE_XML_PROP_STR(CloDiscotDay[11], cur, "Dec");
			err = 0;
		} else if (!xmlStrcmp(cur->name, (const xmlChar *)"FurDiscotDays")) {
			DECODE_XML_PROP_STR(FurDiscotDay[0], cur, "Jan");
			DECODE_XML_PROP_STR(FurDiscotDay[1], cur, "Feb");
			DECODE_XML_PROP_STR(FurDiscotDay[2], cur, "Mar");
			DECODE_XML_PROP_STR(FurDiscotDay[3], cur, "Apr");
			DECODE_XML_PROP_STR(FurDiscotDay[4], cur, "May");
			DECODE_XML_PROP_STR(FurDiscotDay[5], cur, "Jun");
			DECODE_XML_PROP_STR(FurDiscotDay[6], cur, "Jul");
			DECODE_XML_PROP_STR(FurDiscotDay[7], cur, "Aug");
			DECODE_XML_PROP_STR(FurDiscotDay[8], cur, "Sep");
			DECODE_XML_PROP_STR(FurDiscotDay[9], cur, "Oct");
			DECODE_XML_PROP_STR(FurDiscotDay[10], cur, "Nov");
			DECODE_XML_PROP_STR(FurDiscotDay[11], cur, "Dec");
			err = 0;
		}
		cur = cur->next;
	}

exit:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load Holidays File %s", file);
	return err;
}

int is_holiday(int weekday)
{
	if (weekday < 0 || weekday > 6) {
		ERROR_LOG("crazy,weekday is %d?",weekday);
		return 0;
	}

	return holiday_map[weekday];
}


//------------------ Utils End ------------------
