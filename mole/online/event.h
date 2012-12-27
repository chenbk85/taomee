#ifndef ONLINE_EVENT_H_
#define ONLINE_EVENT_H_

// C89 headers
#include <time.h>
// C99 headers
#include <stdint.h>

#include <libxml/tree.h>

#include "map.h"
#include <libtaomee/timer.h>

extern uint32_t show_status;

enum EventConstVal {
	EVENT_max_duration_num	= 10,
	EVENT_max_affected_map	= 10,

	EVENT_max_events	= 128
};

typedef struct Event {
	uint32_t id;

	uint16_t type,status, max_status;

	int start_time, end_time, start_day, end_day;

	int     idle_time;
	int     duration[EVENT_max_duration_num];
	map_t*  affected_map[EVENT_max_affected_map];

	timer_struct_t* tmr;

	list_head_t timer_list;
} event_t;



// parse events.xml
int  load_events(const char* file);
void destroy_events();
// dispatch events
int event_dispatcher(sprite_t* sp, uint32_t evid);
//
event_t* get_event(uint32_t evid);

int load_xml_holidays(const char *file);
int is_holiday(int weekday);


#endif // ONLINE_EVENT_H_
