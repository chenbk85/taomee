typedef struct _q_t{
    uint32_t id;
    uint32_t difficulty;
    char content[256];
    char option_a[64];
    char option_b[64];
    char option_c[64];
    char option_d[64];
    uint32_t ans;
} __attribute__((packed)) ques_t;

extern ques_t ques[1000];
extern uint32_t ques_max;

int load_questions(const char *file);
int answer_question_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int get_ask_answer_info(sprite_t *p);
int get_ask_answer_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
