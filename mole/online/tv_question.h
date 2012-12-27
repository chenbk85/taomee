#ifndef TV_QUESTION_H
#define TV_QUESTION_H

typedef struct tv_q_t{
    uint32_t id;
	uint32_t date;
	uint32_t prize_id;
	uint32_t prize_cnt;
    char content[256];
    char option_a[64];
    char option_b[64];
    char option_c[64];
    char option_d[64];
    uint32_t answer;
} __attribute__((packed)) tv_ques_t;

int load_tv_questions(const char *file, uint32_t type);

int get_tv_question_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_tv_question_callback(sprite_t* p, uint32_t id, char* buf, int len);

int answer_tv_question_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int answer_tv_question_callback(sprite_t* p, uint32_t id, char* buf, int len);
#endif
