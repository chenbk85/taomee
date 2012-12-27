#ifndef PLAY_DRUM_H
#define PLAY_DRUM_H

#include "item.h"

typedef struct play_drum_info{
    uint32_t    begin_time;
    uint32_t    end_time;
	uint32_t    team_cnt[4];
	uint32_t    win_team;
}__attribute__(( packed )) play_drum_info_t;

typedef struct play_drum_timer {
	list_head_t timer_list;
}__attribute__(( packed )) play_drum_timer_t;

int init_play_drum_timer();
int user_play_drum_cmd(sprite_t* p, uint8_t* body, int bodylen);
int notify_play_drum_begin_end(uint32_t flag, uint32_t team_win );
int play_drum_end_game();
int play_drum_end_timeout();
int play_drum_begin();
int get_play_drum_time_left();
int play_drum_get_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_play_drum_team_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen);


#endif

