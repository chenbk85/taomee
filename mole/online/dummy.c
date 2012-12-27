#include <time.h>
#include "benchapi.h"

int on_game_begin (sprite_t *p)
{
	return 0;
}

void on_game_end (game_group_t *p)
{
}

int on_game_data(struct sprite* p, int cmd, uint8_t *body, int len)
{
	return 0;
}

int game_init()
{
	return 0;
}

void game_destroy()
{
}

