/*
 * author : spark
 * date : 20080504
 */

#include "football.hpp"

static spk::football *ball = NULL;

extern "C" int on_game_begin(sprite_t* p)
{
	/*
	 * I think p is valid and its fields are all valid here !!!
	 */
	game_group_t* grp = p->group; //set in Online
	assert (grp && p->tiles);

	//when the first person come in, I will create the game object, although the game has not started !
	if (!ball) {
		ball = new (std::nothrow) spk::football (grp, p->tiles); //判断grp和tiles here
		if ( !ball ) {
			ERROR_RETURN( ("Failed to new Game %s: grpid=%u",
						grp->game->name, grp->id), -1);
		}
	}
	if ( IS_GAME_PLAYER(p) ) {
			DEBUG_LOG ("The user [%u] request to join to the football game", p->id);
			//someone want to join the game, if he is the first one, now the game start !
			if (!(ball->is_game_start ())) {
				DEBUG_LOG ("set the football game initialization status");
				ball->set_game_start ();
				ball->set_game_remain (NULL, NULL);  //60s
				ball->set_game_remain1 (NULL, NULL); //30s
				ball->set_game_over (NULL, NULL);
				ball->chk_game_status ();
			}
			grp->game_handler = ball;
			ball->join_game_req (p);  
	} 

	grp->game_handler = ball;
	//ball->snd_crt_infor(p); 
	ball->init(p); 

	DEBUG_LOG("%s On Game Begin\t[grpid=%u, nplayers=%d]",
				grp->game->name, grp->id, grp->count);

	return 0;
}

extern "C" int on_game_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
	spk::football *ball = reinterpret_cast<spk::football*>(p->group->game_handler);

	if (cmd != 435)  //Only for test
		DEBUG_LOG("%s On Game Data\t[uid=%u, grpid=%u, cmd=%d, len=%d]",
						p->group->game->name, p->id, p->group->id, cmd, len);

	return ball->handle_data(p, cmd, body, len);
}


extern "C" void on_game_end(game_group_t* gamegrp)
{
	assert(gamegrp);

	DEBUG_LOG("%s On Game End\t[grpid=%u]", gamegrp->game->name, gamegrp->id);

	delete reinterpret_cast<spk::football*>(gamegrp->game_handler);
	gamegrp->game_handler = NULL;
	ball = NULL;
}


extern "C" void game_destroy()
{

}
