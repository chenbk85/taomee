#ifndef GAMESERVER_DLL_H_
#define GAMESERVER_DLL_H_

#include "game.h"

int	load_dlls(const char *);
void	unload_dlls();

int	join_game(sprite_t *p, int gameid, gamegrpid_t grpid);
int	one_exit_group(sprite_t *p);
void end_multiplayer_game(game_group_t *ggp, sprite_t *p, uint8_t reason);
void free_game_group (game_group_t *ggp);


#endif // GAMESERVER_DLL_H_
