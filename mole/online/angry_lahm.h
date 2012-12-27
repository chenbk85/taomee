//angry_lahm.h
#ifndef ANGRY_LAHM_H
#define ANGRY_LAHM_H

#define AL_MAX_AWARD_KIND	5
#define AL_MAX_TASK_SIZE	256

//data from db
typedef struct al_task_result_back_s
{
	uint32_t task_id;
	uint32_t cur_score;
	uint32_t bast_score;
	uint32_t has_got_award;
	uint32_t is_pass;
	uint32_t passed_counter;
} __attribute__((packed)) al_task_result_back_t;

typedef struct al_task_back_info_s
{
	uint32_t best_score;
	uint32_t has_got_award;
	uint32_t is_pass;
}__attribute__((packed)) al_task_back_info_t;
//end

typedef struct al_award_info_s
{
	uint32_t item_id;
	uint32_t item_count;
} __attribute__((packed)) al_award_info_t;


typedef struct al_task_info_s
{
	uint32_t level1;
	uint32_t level2;
	uint32_t level3;
	uint32_t award_score;
	uint32_t award_count;
	al_award_info_t	award_info[AL_MAX_AWARD_KIND];
} __attribute__((packed)) al_task_info_t;

typedef struct al_all_task_info_s
{
	uint32_t task_count;
	al_task_info_t task_info[AL_MAX_TASK_SIZE];
} __attribute__((packed)) al_all_task_info_t;

int al_get_task_info_cmd(sprite_t* p, const uint8_t* body, int len);
int al_get_task_info_callback(sprite_t* p, uint32_t id, char* buf, int len);


int al_update_score_cmd(sprite_t* p, const uint8_t* body, int len);
int al_update_score_callback(sprite_t* p, uint32_t id, char* buf, int len);
int al_load_conf_file();
#endif
