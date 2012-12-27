/*
 * author : spark
 * date : 20080504
 */


#ifndef  FOOTBALLDEF_INC
#define  FOOTBALLDEF_INC

#include "football.hpp"

namespace spk {

football::football (game_group_t *grp, map_t *tl) : m_gamegrp(grp), tile(tl)
{
	initBallGoalPos ();
	initBallTeam ();
	initBallPlayer ();
	initBallStatus ();

	game_start_token = false;
}

football::~football ()
{
	if (!ball_snap.empty ()) 
		DEBUG_LOG ("The track of the ball has not been traversed out [%u]", (uint32_t)ball_snap.size());
}

void football::initBallGoalPos () 
{
	if (!ball_goal.empty())  
		ball_goal.clear();

	ball_goal.insert (std::pair<BallTeam, goal_t>(RED, goal_t(0, 75, 191)));
	ball_goal.insert (std::pair<BallTeam, goal_t>(BLUE, goal_t(ROW, 75, 191)));
}
	
void football::initBallPlayer () 
{
	if (!ball_player.empty())  //Maybe other function will call this interface later
		ball_player.clear();
}
	
void football::initBallTeam () 
{
	if (!ball_team.empty())
		ball_team.clear();

	ball_team.insert (std::pair<BallTeam, player_team_t>(RED, player_team ()));
	ball_team.insert (std::pair<BallTeam, player_team_t>(BLUE, player_team ()));
}

void football::initBallStatus () 
{
	set_ball_init_status();
}

void football::set_game_start ()
{
	srand (time(NULL)); 

//set the game start time
	tm_start.tv_sec = ::now.tv_sec;
	tm_start.tv_usec = ::now.tv_usec;

//set the game remain time
	tm_remain.tv_sec = tm_start.tv_sec + (game_last_time - 1000 * 60) / 1000;
	tm_remain.tv_usec = tm_start.tv_usec;

//set the game remain1 time
	tm_remain1.tv_sec = tm_start.tv_sec + (game_last_time - 1000 * 30) / 1000;
	tm_remain1.tv_usec = tm_start.tv_usec;

//set the game over time
	tm_end.tv_sec = tm_start.tv_sec + game_last_time / 1000;
	tm_end.tv_usec = tm_start.tv_usec;

	set_cur_time (game_start_time);

	game_start_token = true;

	DEBUG_LOG ("===============GAME START[%u : %u]================", (uint32_t)::now.tv_sec, (uint32_t)::now.tv_usec);
}


void football::init (sprite_t *p)
{

}

int notify_game_remain (void *obj, void *arg)
{
	game_group_t *ggp = reinterpret_cast<game_group_t *>(obj);
	spk::football *pball = reinterpret_cast<spk::football *> (ggp->game_handler);

	uint32_t t = 60;
	int i = sizeof (protocol_t);
	PKG_UINT32(pball->msg, t, i);
	init_proto_head(pball->msg, SND_GM_REMAIN, i);
	send_to_map2 (pball->tile, pball->msg, i);

	return (0);
}

int notify_game_remain1 (void *obj, void *arg)
{
	game_group_t *ggp = reinterpret_cast<game_group_t *>(obj);
	spk::football *pball = reinterpret_cast<spk::football *> (ggp->game_handler);

	uint32_t t = 30;
	int i = sizeof (protocol_t);
	PKG_UINT32(pball->msg, t, i);
	init_proto_head(pball->msg, SND_GM_REMAIN, i);
	send_to_map2 (pball->tile, pball->msg, i);

	return (0);
}

int notify_game_over (void *obj, void *arg)
{
	game_group_t *ggp = reinterpret_cast<game_group_t *>(obj);
	spk::football *pball = reinterpret_cast<spk::football *> (ggp->game_handler);
	sprite_t *p, *pp;

	DEBUG_LOG("==================GAME OVER[%u : %u]==============", (uint32_t)::now.tv_sec, (uint32_t)::now.tv_usec);

	for (spk::football::ball_player_t::iterator ite = pball->ball_player.begin(); \
		ite != pball->ball_player.end(); ++ite) {
		p = get_sprite (ite->first);
		if (p) {
			pp = p;  //keep a non-null pointer for use
			if (pball->submit_game_score (p)) {
				DEBUG_LOG("The player [%u] submit game socre error !", p->id);
			}
		}
	}

	pball->snd_crt_user();  //send the player id to the client when the game is over

	pball->set_ball_init_status();
	pball->snd_ball_pos_packet ();  
	pball->game_start_token = false;

	//The following threes interfaces are not need here
	pball->ball_player.clear ();
	pball->ball_team.clear ();
	pball->game_start_token = false;  //ready for the next game

	/////////////////////////////////////////////////////////////////


	end_game (ggp, pp, 0);  //Maybe some problem here FIXME

	return (-1);
}


/////////////CHECK ME ! TODO------------
int notify_game_status(void *obj, void *arg)
{
	struct timeval tv;
	uint64_t tt, tmp;

	game_group_t *ggp = reinterpret_cast<game_group_t *>(obj);
	spk::football *pball = reinterpret_cast<spk::football *> (ggp->game_handler);

	pball->check_court_status ();

	tv.tv_usec = now.tv_usec;
	tv.tv_sec = now.tv_sec;
	tt = static_cast<uint64_t>(tv.tv_usec) + static_cast<uint64_t>(pball->scan_time) * 1000;
	if (tt>=1000000) {
		tmp = tt / 1000000;
		tv.tv_sec += static_cast<uint32_t>(tmp);
		tv.tv_usec = static_cast<uint32_t>(tt - 1000000 * tmp);
	} else {
		tv.tv_usec = static_cast<uint32_t>(tt);
	}
	add_micro_event (&notify_game_status, &tv, ggp, NULL);

	return (0);
}

void football::set_game_remain (void *arg, void *arg1)
{
	add_micro_event (&notify_game_remain, &tm_remain, m_gamegrp, NULL);
}

void football::set_game_remain1 (void *arg, void *arg1)
{
	add_micro_event (&notify_game_remain1, &tm_remain1, m_gamegrp, NULL);
}


void football::set_game_over (void *arg, void *arg1)
{
	add_micro_event (&notify_game_over, &tm_end, m_gamegrp, NULL);
}

void football::chk_game_status ()
{
	add_micro_event (&notify_game_status, &tm_start, m_gamegrp, NULL);
}

int notify_ball_init_status (void *obj, void *arg)
{
	bool flag;
	std::vector< spk::football::ball_t> coll_ball;

	game_group_t *ggp = reinterpret_cast<game_group_t *>(obj);
	spk::football *pball = reinterpret_cast<spk::football *> (ggp->game_handler);

	//set the ball random status
	pball->set_ball_rand_status ();

	//trace the ball snap here
	if (!pball->trace_ball_move_snap (coll_ball, flag)) {
		//send the ball trace to the client
		pball->snd_ball_move_snap (coll_ball, flag);
	}

	return (0);
}


void football::set_ball_init_status_event ()
{
	struct timeval tv;
	uint64_t tt;

	tv.tv_sec = now.tv_sec;
	tv.tv_usec = now.tv_usec;
	tt = now.tv_usec + ball_remain_time * 1000;
	if (tt >= 1000000) {
		uint64_t tmp = tt / 1000000;
		tv.tv_sec += tmp;
		tv.tv_usec = tt - tmp * 1000000;
	}
	else {
		tv.tv_usec = tt;
	}

	add_micro_event (&notify_ball_init_status, &tv, m_gamegrp, NULL);
}
int football::handle_data (sprite_t *p, int cmd, const uint8_t body[], int len)
{
	//think the passed parameters are all valid here ! check them before calling this function
	TRACE_LOG ("Football Handle data : uid = %u, grpid = %u, cmd = %u, len = %d", \
			p->id, m_gamegrp->id, cmd, len);

	int err_code = 0;
	switch (cmd) {
		case (CHG_POS_REQ) :
			err_code = change_player_pos (p, body, len); 
			break;
		case (PROTO_GAME_LEAVE):
			err_code = quit_game_req (p);
			break;
		case (PROTO_SCENEGAME_INFO) :
			snd_crt_infor (p);
			break;
		default:
			ERROR_RETURN (("Football: Unrecognized Command ID %d from uid=%u", cmd, p->id), -1);
	}

	return (err_code);
}

void football::snd_crt_infor (sprite_t *p)
{
	int i = 0;
	uint32_t tm_remain, player_cnt;
	int_pos_t   pos;
	uint64_t tt;
	uint8_t tm_id;

	pos.first = static_cast<uint32_t>(round(ball.b_pos.first));
	pos.second = static_cast<uint32_t>(round(ball.b_pos.second));
	CRT_TO_MAP(pos);

	set_cur_time(tt);
	tm_remain = (!game_start_token) ? (game_last_time/1000) : \
				((game_last_time-(tt-game_start_time))/1000);

	i = sizeof (protocol_t);
	PKG_UINT32(msg, pos.first, i);
	PKG_UINT32(msg, pos.second, i);
	PKG_UINT32(msg, ball_team[RED].goal_ball_cnt, i);
	PKG_UINT32(msg, ball_team[BLUE].goal_ball_cnt, i);
	PKG_UINT32(msg, tm_remain, i);

	//add here
	player_cnt = ball_player.size();
	PKG_UINT32(msg, player_cnt, i);
	if (player_cnt != 0) {
		for (ball_player_t::iterator ite = ball_player.begin(); ite != ball_player.end(); ++ite) {
			PKG_UINT32(msg, ite->first, i);
			tm_id = (ite->second.team_id==RED) ? 0 : 1;
			PKG_UINT8(msg, tm_id, i);
		}
	}
	init_proto_head (msg, PROTO_SCENEGAME_INFO, i);

	send_to_self (p, msg, i, 1);
}

void football::snd_ball_pos_packet ()
{
	int i = sizeof (protocol_t);

	int_pos_t pos;

	pos.first = static_cast<uint32_t>(ball.b_pos.first);
	pos.second = static_cast<uint32_t>(ball.b_pos.second);

	CRT_TO_MAP(pos);

	PKG_UINT32(msg, pos.first, i);
	PKG_UINT32(msg, pos.second, i);
	init_proto_head (msg, SND_BALL_POS, i);

	send_to_map2(tile, msg, i);
}

int football::snd_crt_user ()
{
	int i = sizeof (protocol_t);
	sprite_t *p;
	struct entry_pos *pos = get_entry(tile, 0); 

	if (pos==NULL)  //TODO ---
		ERROR_RETURN(("Fail get the entry positon for map [%p]", tile), 0);

	PKG_UINT32 (msg, ball_player.size(), i);
	PKG_UINT32 (msg, pos->x, i);
	PKG_UINT32 (msg, pos->y, i);
	for (ball_player_t::iterator ite = ball_player.begin(); ite != ball_player.end(); ++ite){
		p = get_sprite (ite->first);
		PKG_UINT32 (msg, ite->first, i);
		if (p) {
			p->posX = static_cast<uint16_t>(pos->x);
			p->posY = static_cast<uint16_t>(pos->y);
		}
	}
	init_proto_head (msg, SND_CRT_USER, i);

	send_to_map2(tile, msg, i);

	return 0;
}

int football::change_player_pos (sprite_t *p, const uint8_t body[], int len)
{
	CHECK_BODY_LEN(len, 16);
	bool	 flag;

	if (ball_player.find (p->id) == ball_player.end ()) {
		ERROR_RETURN(("cannot find the player [%u] in teams !", p->id), -1);
	}

	p->waitcmd = 0;

	int i = 0;
	int_pos_t cur_pos, des_pos;

	UNPKG_UINT(body, cur_pos.first, i);
	UNPKG_UINT(body, cur_pos.second, i);
	UNPKG_UINT(body, des_pos.first, i);
	UNPKG_UINT(body, des_pos.second, i);

	CHK_POS_VALID (cur_pos);

	if (IS_GOAL_POS(des_pos)) //add in 20080522
		CVT_GOAL_POS(des_pos);
	CHK_POS_VALID(des_pos);

	//if the position is edge position , convert it here
	if (IS_EDGE_POS(cur_pos))
		CVT_EDGE_POS(cur_pos);
	if (IS_EDGE_POS(des_pos))
		CVT_EDGE_POS(des_pos);

	//Now, they are all valid position here

	//convert the map pos to court pos
	MAP_TO_CRT (cur_pos);
	MAP_TO_CRT (des_pos);  
		
	set_cur_time (ball_player[p->id].cur_tm);
	ball_player[p->id].cur_pos = cur_pos;
	ball_player[p->id].des_pos = des_pos;

	//if the person control the ball, the ball will move now; the player will keep stand

	if (cur_pos == des_pos) return (0); //Maybe this case cannot occur !
	if (p->id==ball.pertain && ball.status==IS_CTL_NON_ACT) {
		std::vector< ball_t > coll_ball;
		set_ctl_ball_status (des_pos, sqrt(pow(ball_player[p->id].des_pos.second-ball_player[p->id].cur_pos.second, 2)\
					+pow(ball_player[p->id].des_pos.first-ball_player[p->id].cur_pos.first, 2))); 
		if (!trace_ball_move_snap (coll_ball, flag))
			snd_ball_move_snap (coll_ball, flag);
		ball_player[p->id].des_pos = ball_player[p->id].cur_pos;  //set the player stand
	}

	return (0);
}

double football::get_next_stop_pos (uint32_t &t_stop, flt_pos_t &pos_stop)
{
	double x, y;
	double s1 = pow (trace_ball.b_rate, 2) / (2 * a);
	double s2 = sqrt (pow (trace_ball.dir_pos.second-trace_ball.b_pos.second, 2) + pow (trace_ball.dir_pos.first-\
				trace_ball.b_pos.first, 2));

	t_stop = static_cast<uint32_t>(round(trace_ball.b_rate / a));
	x = trace_ball.b_pos.first+s1*(trace_ball.dir_pos.first-trace_ball.b_pos.first)/s2;
	y = trace_ball.b_pos.second+s1*(trace_ball.dir_pos.second-trace_ball.b_pos.second)/s2;

	pos_stop = std::make_pair (x, y);

	return (s1);
}

double football::get_next_coll_pos (uint32_t &t_coll, flt_pos_t &pos_coll)
{
	double s = sqrt (pow (trace_ball.dir_pos.second-trace_ball.b_pos.second, 2) + \
			pow (trace_ball.dir_pos.first-trace_ball.b_pos.first, 2));

	//The max distance is less than the collision distance
	if (pow(trace_ball.b_rate,2)/(2*a)<s) 
		t_coll=static_cast<uint32_t>(round(trace_ball.b_rate/a)); //Maybe it is invliad for the use !
	else 
		t_coll = static_cast<uint32_t>(round ((trace_ball.b_rate - sqrt (pow (trace_ball.b_rate, 2) - 2 * a * s)) / a));

	pos_coll = trace_ball.dir_pos;

	return (s);
}

double football::get_next_move_pos (uint32_t &t_move, flt_pos_t &pos_move)
{
	double x, y;
	double s1, s2;

	//compute the max long time the ball can move
	uint32_t t_tmp = static_cast<uint32_t>(round(trace_ball.b_rate/a));
	t_move = t_tmp>(trace_time) ? (trace_time) : t_tmp;
	s1 = trace_ball.b_rate * t_move - a * pow (t_move, 2) / 2;
	s2 = sqrt (pow (trace_ball.dir_pos.second-trace_ball.b_pos.second, 2) + pow (trace_ball.dir_pos.first-\
				trace_ball.b_pos.first, 2));

	x = trace_ball.b_pos.first+s1*(trace_ball.dir_pos.first-trace_ball.b_pos.first)/s2;
	y = trace_ball.b_pos.second+s1*(trace_ball.dir_pos.second-trace_ball.b_pos.second)/s2;

	pos_move = std::make_pair (x, y);

	return (s1);
}


double football::get_next_ball_rate (uint32_t t)
{
	double rate = trace_ball.b_rate - t * a;

	if (rate < 0.0) {
		DEBUG_LOG ("Maybe error occur here : The ball coll rate is : %lf", rate);
		rate = 0.0;
	}
	return rate;
}


void football::set_ball_move_status (flt_pos_t &pos, flt_pos_t &dir, double rate, uint64_t tt)
{
	trace_ball.b_pos = pos;
	trace_ball.dir_pos = dir;
	trace_ball.b_rate = rate;
	trace_ball.b_tm = tt;
}

football::flt_pos_t football::get_next_ball_dir ()
{
	/*
	 * special cases : vertical or horizontal direction, intersection with the four vertexes
	 */
	if ((trace_ball.b_pos.first==trace_ball.dir_pos.first) || (trace_ball.b_pos.second==trace_ball.dir_pos.second) || \
			(trace_ball.dir_pos.first==0.0 && trace_ball.dir_pos.second==0.0) || \
			(trace_ball.dir_pos.first==0.0 && trace_ball.dir_pos.second==COL) || \
			(trace_ball.dir_pos.first==ROW && trace_ball.dir_pos.second==COL) || \
			(trace_ball.dir_pos.first==ROW && trace_ball.dir_pos.second==0.0)) 
	{
		return (get_dir_pos (trace_ball.dir_pos, trace_ball.b_pos));
	}

	flt_pos_t  pos;
	if (trace_ball.dir_pos.first==0.0 || trace_ball.dir_pos.first==ROW)
		pos = std::make_pair (trace_ball.b_pos.first, 2*trace_ball.dir_pos.second-trace_ball.b_pos.second);
	else
		pos = std::make_pair (2*trace_ball.dir_pos.first-trace_ball.b_pos.first, trace_ball.b_pos.second);

	return (get_dir_pos (trace_ball.dir_pos, pos));
}

void football::set_ball_shoot_status(BallTeam goal, uint64_t tt)
{
	trace_ball.is_in_goal = true;
	trace_ball.team_id = goal;
	trace_ball.b_tm = tt;
}

void football::set_ball_stop_status(flt_pos_t &pos, uint64_t tt)
{
	trace_ball.b_pos = pos;
	trace_ball.b_rate = 0.0;
	trace_ball.status = NON_CTL_NON_ACT;
	trace_ball.pertain = 0;
	trace_ball.b_tm = tt;
}

void football::do_shoot_special_pos (ball_t &trace_ball, BallTeam shoot_goal, uint64_t &tt)
{
//update the rate and timestamp together
	double s = pow(trace_ball.b_rate, 2) / (2 * a);
	double v;

	if (shoot_goal == RED) {
		v = (s>=30.0) ? (sqrt(pow(trace_ball.b_rate, 2) - 2 * a * 30.0)) : 0.0;
		tt += static_cast<uint32_t>((trace_ball.b_rate - v) / a);
		trace_ball.b_rate = v;
		trace_ball.b_pos.first = -30.0;
	}
	else {
		v = (s>=40.0) ? (sqrt(pow(trace_ball.b_rate, 2) - 2 * a * 40.0)) : 0.0;
		tt += static_cast<uint32_t>(((trace_ball.b_rate - v) / a));
		trace_ball.b_rate = v;
		trace_ball.b_pos.first = ROW+40.0;
	}
}


int football::trace_ball_move_snap (std::vector< ball_t > &coll_ball, bool &flag)
{
	if (!ball_snap.empty ()) {
		DEBUG_LOG ("***SO BAD HERE : when trace the new ball track, the ball_snap queue is not empty***");
		while (!ball_snap.empty ())
			ball_snap.pop ();
	}

	double s_coll, s_stop, s_move, s_rm, s_min, rate;
	uint64_t stamp;
	uint32_t t_rm;
	uint32_t t_stop, t_coll, t_move;
	flt_pos_t pos_stop, pos_coll, pos_move, dir;
	BallTeam shoot_goal;
	uint32_t trk_pot = 0;

	t_rm = 0;
	s_rm = 0.0;


	flag = false;
	stamp = ball.b_tm;
	trace_ball = ball; //for the check, The first status here
	//ball_snap.push (trace_ball); //cannot record it !!!!
	coll_ball.push_back (trace_ball);

	/*
	 * 在第一次球踢出的时候， 不监测， 因为可能会出现多人围球， 从而造成球被
	 * 频繁控制， 无法踢出去的情况。
	 */
	/*
	 *这段不监测时间内， 球运动的距离应该大于控球距离(多倍)
	 */

	//对射门位置做特殊处理！！!
	s_stop = get_next_stop_pos (t_stop, pos_stop);
	s_coll = get_next_coll_pos (t_coll, pos_coll);
	s_move = get_next_move_pos (t_move, pos_move);
	while (1) {
		if (++trk_pot>MAX_TRK_POT) {
			//so , if the coll_ball is too large , it will be ignored here !
			DEBUG_LOG("*********BAD CASE : trk_pot[%u] max_trk_pot[%lu]",trk_pot, MAX_TRK_POT);
			//record the information 
			DEBUG_LOG("***************BEGIN***********************");
			DEBUG_LOG ("b_rate[%lf] b_pos[%lf, %lf] dir_pos[%lf %lf] pertain[%u] status[%d] \
					is_in_goal[%d] team_id[%d] b_tm[%lu] kick[%d]", ball.b_rate, ball.b_pos.first, \
					ball.b_pos.second, ball.dir_pos.first, ball.dir_pos.second, ball.pertain, ball.status, \
					ball.is_in_goal, ball.team_id, ball.b_tm, ball.kick);
			DEBUG_LOG("***************END***********************");
			coll_ball.clear ();
			while (!ball_snap.empty ())
				ball_snap.pop ();
			return -1;  //ignore this track
		}
		s_min = (s_stop > s_coll) ? s_coll : s_stop;
		s_min = (s_min > s_move) ? s_move : s_min;

		if (IS_EQUAL(s_min, s_stop)) {
			t_rm += t_stop;
			s_rm += s_stop;
			add_cur_time(stamp, t_stop);
			if (IS_EQUAL(s_stop, s_coll) && IS_SHOOT(pos_coll, shoot_goal)) {
				do_shoot_special_pos (trace_ball, shoot_goal, stamp);
				set_ball_shoot_status (shoot_goal, stamp);
				ball_snap.push (trace_ball);
				coll_ball.push_back (trace_ball);
				flag = true; //射门
			} else {
				set_ball_stop_status (pos_stop, stamp);
				ball_snap.push (trace_ball);
				coll_ball.push_back (trace_ball);
				flag = false;
			}
			break;  
		}
		else if (IS_EQUAL(s_min, s_coll)) {
			t_rm += t_coll;
			s_rm += s_coll;
			add_cur_time(stamp, t_coll);
			if (IS_SHOOT(pos_coll, shoot_goal)) {
				do_shoot_special_pos (trace_ball, shoot_goal, stamp);
				set_ball_shoot_status (shoot_goal, stamp);
				ball_snap.push (trace_ball);
				coll_ball.push_back (trace_ball);
				flag = true;
				break;
			} 
			else{
				dir = get_next_ball_dir ();				
				rate = get_next_ball_rate (t_coll);
				set_ball_move_status (pos_coll, dir, rate, stamp);
				ball_snap.push (trace_ball);
				coll_ball.push_back (trace_ball);

				//continue check for the next time
				s_stop = get_next_stop_pos (t_stop, pos_stop);
				s_coll = get_next_coll_pos (t_coll, pos_coll);
				s_move = get_next_move_pos (t_move, pos_move);

				continue;
			}
		}
		else {
			//The min distance is move distance
			t_rm += t_move;
			s_rm += s_move;
			add_cur_time(stamp, t_move);
			dir = trace_ball.dir_pos;
			rate = get_next_ball_rate (t_move);
			set_ball_move_status (pos_move, dir, rate, stamp);
			if (s_rm>non_ctl_dis)  //FIXME
				ball_snap.push (trace_ball);

			//continue check for the next time
			s_stop = get_next_stop_pos (t_stop, pos_stop);
			s_coll = get_next_coll_pos (t_coll, pos_coll);
			s_move = get_next_move_pos (t_move, pos_move);

			continue;
		}
	}

	return 0;

}

void football::snd_ball_move_snap (std::vector< ball_t > &coll_ball, bool flag)
{
	int i = sizeof (protocol_t);
	uint32_t coll_cnt = coll_ball.size();
	uint8_t f = (flag) ? 1 : 0;
	uint32_t second, millisecond;
	int_pos_t pos;
	uint32_t rate;

	if (coll_cnt<2) { //Only for test
		DEBUG_LOG ("Serious error ! The collision count is [%d]", coll_cnt);
		return ;
	}

	PKG_UINT32(msg, ball.pertain, i);
	PKG_UINT8(msg, f, i);
	PKG_UINT32(msg, coll_cnt-2, i);
	for (std::vector< ball_t >::iterator ite=coll_ball.begin(); ite!=coll_ball.end(); ++ite) {
		cvt_cur_time(ite->b_tm, second, millisecond);
		pos.first = static_cast<uint32_t>(ite->b_pos.first);
		pos.second = static_cast<uint32_t>(ite->b_pos.second);
		CRT_TO_MAP(pos);
		rate = static_cast<uint32_t>(round(ball.b_rate * 1000));
		PKG_UINT32(msg, second, i);
		PKG_UINT32(msg, millisecond, i);
		PKG_UINT32(msg, rate, i);
		PKG_UINT32(msg, pos.first, i);
		PKG_UINT32(msg, pos.second, i);
	}
	init_proto_head (msg, SND_SHOT_BALL, i);
	
	send_to_map2(tile, msg, i);
}

void football::set_ctl_ball_status (int_pos_t &des_pos, double dis)
{
	//Now, the ball initialization position is the original position of the ball, not the player
	int_pos_t pos;

	GEN_RAND_RATE_BY_DIS(ball.b_rate, dis);
	pos.first = static_cast<uint32_t>(ball.b_pos.first);
	pos.second = static_cast<uint32_t>(ball.b_pos.second);
	ball.dir_pos = get_dir_pos (pos, des_pos);
	ball.status = IS_CTL_IS_ACT;
	set_cur_time(ball.b_tm);  
	ball.is_in_goal = false;
	ball.kick = true;
}


int football::join_game_req (sprite_t *p)
{
	//Only for test ! FIXME-------
	if (ball_player.size () >=10) 
		ERROR_RETURN(("Two teams are both full !"), -1);

	//judge the count of each team
	if ((ball_team[RED].player_num==ball_team[BLUE].player_num) && (ball_team[RED].player_num==MAX_PLAYER_NUM)) {
		ERROR_RETURN(("Two teams are both full !"), -1); //error occurs here ! Online will be charge of this !
	}
	BallTeam team = (ball_team[RED].player_num>ball_team[BLUE].player_num) ? (BLUE) : (RED);
	if (!add_player (p, team)) {
		snd_join_team_packet (p);
		DEBUG_LOG ("The player [%u] join the game team [%d] successfully", p->id, ball_player[p->id].team_id);
	} else {
		DEBUG_LOG ("The player [%u] join the game failed", p->id);
	}

	return (0);
}

void football::snd_join_team_packet (sprite_t *p)
{
	int i = sizeof (protocol_t);
	uint8_t team = ((ball_player[p->id].team_id == RED) ? 0 : 1 );
	uint64_t tt;
	uint32_t second, millisecond;
	int_pos_t pos;

	set_cur_time(tt);
	cvt_cur_time(tt, second, millisecond);
	pos.first = static_cast<uint32_t>(round(ball_player[p->id].cur_pos.first));
	pos.second = static_cast<uint32_t>(round(ball_player[p->id].cur_pos.second));
	CRT_TO_MAP(pos);
	PKG_UINT32(msg, p->id, i);
	PKG_UINT8(msg, team, i);
	PKG_UINT32(msg, pos.first, i);
	PKG_UINT32(msg, pos.second, i);
	PKG_UINT32(msg, second, i);
	PKG_UINT32(msg, millisecond, i);
	init_proto_head (msg, SND_JOIN_GM, i);

	DEBUG_LOG("P=%p PID=%u TEAM=%d", p, p->id, ball_player[p->id].team_id);
	send_to_map2 (tile, msg, i);
}


void football::send_game_bonus (sprite_t *p, game_bonus_t *gbt)
{
	uint32_t game_id = tile->gamegrp->id; // I think it is valid here
	char session[24] = {'\0'};

	int i = sizeof (protocol_t);

	PKG_UINT32(msg, game_id, i);
	PKG_STR(msg, session, i, sizeof(session));
	PKG_UINT32(msg, 0, i);
	PKG_UINT32(msg, gbt->exp, i);
	PKG_UINT32(msg, gbt->strong, i);
	PKG_UINT32(msg, 0, i);
	PKG_UINT32(msg, 0, i);
	PKG_UINT32(msg, gbt->mole_bean, i);
	init_proto_head (msg, QUIT_GAME, i);

	send_to_self (p, msg, i, 1);
}

void football::snd_ball_shoot_packet (BallTeam team_id, uint64_t tt)
{
	int i = sizeof (protocol_t);
	uint8_t id1, id = (team_id == RED) ? 0 : 1;

	//some problem here
	bool is_player_in = (ball_player.find (ball.pertain) != ball_player.end());  //modify in 20080622
	if (is_player_in)
		id1 = (ball_player[ball.pertain].team_id == RED) ? 0 : 1;
	else
		id1 = 0;  //default
	PKG_UINT32(msg, ball.pertain, i);
	PKG_UINT8(msg, id1, i);
	if (is_player_in) {
		PKG_UINT32(msg, ball_player[ball.pertain].goal_ball_cnt, i);
	} else {
		PKG_UINT32(msg, 0, i);
	}
	PKG_UINT8(msg, id, i);
	PKG_UINT32(msg, ball_team[RED].goal_ball_cnt, i);
	PKG_UINT32(msg, ball_team[BLUE].goal_ball_cnt, i);
	init_proto_head (msg, SND_BALL_GOAL, i);

	send_to_map2 (tile, msg, i);
}

void football::snd_ctl_player_packet ()
{
	int i = sizeof (protocol_t);
	int_pos_t pos;
	uint32_t second, millsecond;

	cvt_cur_time(ball.b_tm, second, millsecond);
	pos.first = static_cast<uint32_t> (ball.b_pos.first);
	pos.second = static_cast<uint32_t> (ball.b_pos.second);
	CRT_TO_MAP(pos);
	PKG_UINT32(msg, ball.pertain, i);
	PKG_UINT32(msg, pos.first, i);
	PKG_UINT32(msg, pos.second, i);
	PKG_UINT32(msg, second, i);
	PKG_UINT32(msg, millsecond, i);
	init_proto_head (msg, SND_CTL_PLAYER, i);

	send_to_map2 (tile, msg, i);
}

int football::submit_game_score (sprite_t *p)
{
	
	game_bonus_t gbt;
	memset (&gbt, 0x00, sizeof (game_bonus_t)); //need here
	if(get_game_bonus (p->id, &gbt)) 
		 ERROR_RETURN(("get game bonus error "), -1);

	p->sess_len = 73;
	//I will check here : modify in 20080622
	if (!p->group) {
		DEBUG_LOG ("************BAD CASE : The id[%u] game group pointer is NULL********", p->id);
		return -1;
	} 
	game_t* g = p->group->game;
	assert(g); //FIXME later
	int i = sizeof(protocol_t);
	PKG_UINT32(p->session, g->id, i);

	char outbuf[8], inbuf[8];
	//session: time + ip + time + score + userid + game_id
	// time
	memcpy(inbuf, &now.tv_sec, 4);
	// ip
	//uint32_t ipaddr = get_ip(fd);
	uint32_t ipaddr = get_ip2(p);
	memcpy(inbuf + 4, &ipaddr, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	// score
	uint32_t score = gbt.score;
	memcpy(inbuf + 4, &score, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	// userid & gameid
	memcpy(inbuf, &p->id, 4);
	memcpy(inbuf + 4, &g->id, 4);
	des_encrypt(DES_GAME_SCORE_KEY, inbuf, outbuf);
	PKG_STR(p->session, outbuf, i, sizeof outbuf);
	//score
	PKG_UINT32(p->session, score, i);

	PKG_UINT32(p->session, gbt.exp, i); 
	PKG_UINT32(p->session, gbt.strong, i); 
	PKG_UINT32(p->session, gbt.iq, i); 
	PKG_UINT32(p->session, gbt.charm, i); 
	PKG_UINT32(p->session, gbt.mole_bean, i); 
	PKG_UINT32(p->session, 0, i); 

	init_proto_head(p->session, PROTO_GAME_SCORE, i);
	p->waitcmd = PROTO_GAME_SCORE;

	return (db_attr_op(p, gbt.mole_bean, gbt.exp, gbt.strong, gbt.iq, gbt.charm, ATTR_CHG_play_game, m_gamegrp->game->id));
}


int football::quit_game_req (sprite_t* p)
{
	if (!IS_GAME_PLAYER (p)) return (-1);

	DEBUG_LOG ("The player [%u] in the team [%d] leave the game", p->id, ball_player[p->id].team_id);
	if (submit_game_score (p)) {
		DEBUG_LOG ("Submit game socre error ! ");
	}
	ball_team[ball_player[p->id].team_id].player_num--;
	ball_player.erase (p->id);

	if (ball_player.empty()) {
		set_ball_init_status();
		snd_ball_pos_packet ();  
		this->game_start_token = false;
		DEBUG_LOG ("The court is empty now, so end the game");
	}

	/*
	 * if the ball is shooten, add the socre to the team which includes the player, but ignore the player
	 */
	if (ball.pertain == p->id) {
		if (ball.status == IS_CTL_NON_ACT) {
			ball.status = NON_CTL_NON_ACT; 
		}
		else if (ball.status == IS_CTL_IS_ACT) {
			ball.status = NON_CTL_IS_ACT; 
		}
		ball.pertain = 0; //not forget here !!
	}


	//set the player out of the court
	snd_user_pos(p);

	return (0);
}

int football::snd_user_pos (sprite_t *p)
{
	//I donot think the p is invalid here
	int i = sizeof (protocol_t);
	struct entry_pos *pos = get_entry(tile, 0);

	if (pos==NULL)  //TODO---
		ERROR_RETURN(("Fail get the entry positon for map [%p]", tile), 0);

	p->posX = static_cast<uint16_t>(pos->x);
	p->posY = static_cast<uint16_t>(pos->y);

	PKG_UINT32 (msg, p->id, i);
	PKG_UINT32 (msg, pos->x, i);
	PKG_UINT32 (msg, pos->y, i);

	init_proto_head (msg, SND_USER_POS, i);

	send_to_map2 (tile, msg, i);

	return 0;
}

int football::get_game_bonus (uint32_t user_id, game_bonus_t *gbt)
{
	if (ball_player.find (user_id) == ball_player.end()) {
		ERROR_RETURN(("cannot find the player infor : %u", user_id), -1);
	}

	uint32_t t;
	uint64_t tt ;
	set_cur_time(tt);
	t = static_cast <uint32_t> (round ((tt-ball_player[user_id].enter_tm) * 1.0 / (60 * 1000)));
	DEBUG_LOG ("The player [%u] stay time in the game : %u", user_id, t);
	if (t<=3) 
		gbt->mole_bean = 3 * (ball_player[user_id].goal_ball_cnt);
	else
		gbt->mole_bean = 5 * (ball_player[user_id].goal_ball_cnt);
	gbt->mole_bean = (gbt->mole_bean > 100) ? 100 : gbt->mole_bean;
	gbt->exp = gbt->mole_bean / 5;
	gbt->strong = (gbt->mole_bean) / 10;
	gbt->score = 100 * (ball_player[user_id].goal_ball_cnt);
	gbt->score = (gbt->score > 60000) ? 60000 : gbt->score;

	return (0);
}

int football::add_player (sprite_t *p, BallTeam team_id)
{
	//initialize the player information
	flt_pos_t start_pos;
	std::pair<ball_player_t::iterator, bool> ret;

	GEN_RAND_POS(start_pos, team_id);

	ball_team[team_id].player_num++;

	//first, set the player is non-action status
	try {
		ret = ball_player.insert (std::pair< uint32_t, player_t >(p->id, player (team_id, start_pos)));
		if (ret.second == false) {
			ERROR_RETURN(("The player has joined the game %u, %u", p->id, \
						static_cast< uint32_t >(ball_player[p->id].team_id)), -1);
		}
	}
	catch (std::exception const &ex) {
		DEBUG_LOG("**************CATCH EXCEPTION HERE***************");
		DEBUG_LOG("The exception information is : [%s]", ex.what()); //const char *
		DEBUG_LOG ("The player num is : [%u] The player is: [%u]", (uint32_t)ball_player.size(), p->id);
		DEBUG_LOG("******************END*********************");
		ERROR_RETURN(("occur the exception [%d]", __LINE__), -1);
	}
	DEBUG_LOG("The player which has just joined the game : user_id [%u] team [%d]", p->id, \
		ball_player[p->id].team_id);

	return (0);
}

////////////////////////////////////////////////////////////////////////


//The cur and des points are not the same points !!!
football::flt_pos_t football::get_dir_pos (const flt_pos_t &cur, const flt_pos_t &des)
{
	flt_pos_t pos;

	//if the line decided by the current and desitinated points is vertical or horiziontal, proceed specially
	if (cur.first == des.first) {
		pos.first = cur.first;
		pos.second = (des.second > cur.second) ? (COL) : 0;
		return (pos);
	}
	if (cur.second == des.second) {
		pos.second = cur.second;
		pos.first = (des.first > cur.first) ? (ROW): (0);
		return (pos);
	}

	//handle the usual cases
	flt_pos_t intersect[4];
	//x = 0
	intersect[0].first = 0;
	intersect[0].second = (des.second*(0-cur.first)-cur.second*(0-des.first))/(des.first-cur.first);
	//y = COL 
	intersect[1].first = (des.first*(COL-cur.second)-cur.first*(COL-des.second))/(des.second-cur.second);
	intersect[1].second = COL;
	//x = ROW
	intersect[2].first = ROW;
	intersect[2].second = (des.second*(ROW-cur.first)-cur.second*(ROW-des.first))/(des.first-cur.first);
	//y = 0
	intersect[3].first = (des.first*(0-cur.second)-cur.first*(0-des.second))/(des.second-cur.second);
	intersect[3].second = 0;

	//some statements only for the test ! FIXME
	//check the position validity , so judge the line intersect which line
	//Only find two valid points here:
	int i, j, valid[4] = {0};
	for (i=0, j=0; i<4; ++i) {
		if ((intersect[i].first>=0 && intersect[i].first<=ROW) && \
				(intersect[i].second>=0 && intersect[i].second<=COL))
			valid[j++] = i;
	}
	if (j<2 || (j>2 && j != 4)) {
		DEBUG_LOG ("Serious error here : Get the intersection points error ! %d", j);
	}
	//Maybe j>2 is valid here , diagonal line here, 仅仅取前2个即可， 因为是逆时针顺序计算的
	//碰撞问题在这里!!!!  FIXME
	//因为cur_pos != des_pos, 所以方向位一定不等于cur_pos, 所以对于后2种情况，一定要保证不等于cur_pos 
	if ((des.first>cur.first && intersect[valid[0]].first>=des.first) || (des.first<cur.first && \
		intersect[valid[0]].first<=des.first) ||  (des.first>=intersect[valid[0]].first && \
			intersect[valid[0]].first>=cur.first && intersect[valid[0]].second!=cur.second) || \
			(des.first<=intersect[valid[0]].first && intersect[valid[0]].first<=cur.first && \
			intersect[valid[0]].second!=cur.second)) 
		{
			/*
			DEBUG_LOG ("The intersection points is here : [%lf, %lf]", intersect[valid[0]].first,\
				intersect[valid[0]].second);
				*/
			return (intersect[valid[0]]);
		}
	else
		{
			/*
			DEBUG_LOG ("The intersection points is here : [%lf, %lf]", intersect[valid[1]].first,\
				intersect[valid[1]].second);
				*/
			if(valid[1] > 3 || valid[1] < 0)
			{
				return (intersect[valid[0]]);
			}

			return (intersect[valid[1]]);
		}
}

bool football::IS_CTL_PRO()
{
	int tt = static_cast<int>(CTL_PRO * CTL_VAL);
	int tt1 = static_cast<int>(random() % CTL_VAL); //[0, CTL_VAL-1]

	return (tt1>=0 && tt1<=tt-1) ? true : false;
}


bool football::IS_SHOOT(flt_pos_t &pos, BallTeam &goal)
{
	if (pos.first != 0 || pos.first != ROW) {
		//DEBUG_LOG ("Maybe error here : the goal position is not right [%lf]", pos.first);
	}
	
	if (IS_EQUAL(pos.first, 0) && pos.second>=ball_goal[RED].pos_y_l && pos.second<=ball_goal[RED].pos_y_h) {
		//DEBUG_LOG ("SHOOT : The ball position is : [%lf, %lf] red goal", pos.first, pos.second);
		goal = RED;
		return (true);
	} 
	if (IS_EQUAL(pos.first, ROW) && pos.second>=ball_goal[BLUE].pos_y_l && pos.second<=ball_goal[BLUE].pos_y_h) {
		//DEBUG_LOG ("SHOOT : The ball position is : [%lf, %lf] blue goal", pos.first, pos.second);
		goal = BLUE;
		return (true);
	}

	return (false);
}

void football::set_ball_init_status ()
{
	ball.b_rate = 0.0; 
	ball.b_pos = std::make_pair (ROW/2 + 10, COL/2 - 50);
	ball.status = NON_CTL_NON_ACT;
	ball.pertain = 	0; //needed here
	ball.is_in_goal = false;
}

void football::set_ball_rand_status ()
{
#define rd_min (COL/2 - 120)
#define rd_max (COL/2 + 130)
	int_pos_t begin_pos, end_pos;

	GEN_RAND_RATE (ball.b_rate);
	//set the begin pos
	begin_pos.first = (ball.team_id==RED) ? 0 : ROW; 
	begin_pos.second = (75 + 191) / 2;
	//set the end pos
	//end_pos.first = (ball.team_id==RED) ? (0+20) : (ROW-20);
	end_pos.first = ROW/2 + 10;
	//end_pos.second = static_cast<uint32_t>((random()%(((rd_max)+1)-(rd_min)))+(rd_min));
	end_pos.second = static_cast<uint32_t>(random() % (rd_max-rd_min)+rd_min);
	ball.dir_pos = get_dir_pos (begin_pos, end_pos);
	ball.status = NON_CTL_IS_ACT;
	set_cur_time(ball.b_tm);
	ball.is_in_goal = false;
	ball.kick = false;
}


void football::set_ball_shoot_infor()
{
	BallTeam goal = (ball.team_id == RED) ? BLUE : RED;
	ball_team[goal].goal_ball_cnt++;
	DEBUG_LOG("The team [%d] get a ball , total [%d]", goal, ball_team[goal].goal_ball_cnt);
	//Maybe the player has already left the court
	if (ball_player.find (ball.pertain) != ball_player.end()) {
		if (ball.pertain != 0 && ball_player[ball.pertain].team_id != ball.team_id){
			ball_player[ball.pertain].goal_ball_cnt++;
			DEBUG_LOG("The player [%u] get a ball, total [%u]", ball.pertain, ball_player[ball.pertain].goal_ball_cnt);
		}
	}
}


int football::check_player_status ()
{
	//update the each player status information after t seconds, suppose the players not change their direction
	uint64_t cur_time;
	double s1, s2;
	double x,y;
	flt_pos_t cur_pos;
	std::map< uint32_t, player_t >::iterator ite;

	set_cur_time(cur_time);
	for (ite=ball_player.begin(); ite != ball_player.end(); ++ite) {
		if (ite->second.cur_pos == ite->second.des_pos) {
			continue; //non-action
		}

		s1 = p_rate * (cur_time - ite->second.cur_tm);  
		s2 = sqrt (pow ((ite->second.des_pos.first-ite->second.cur_pos.first),2) + \
				pow ((ite->second.des_pos.second-ite->second.cur_pos.second),2));
		ite->second.cur_tm = cur_time;
		if (!IS_LESS(s1,s2)) {
			//will stop at the destinated position
			ite->second.cur_pos = ite->second.des_pos;
		} else {
			x = ite->second.cur_pos.first+s1*(ite->second.des_pos.first-ite->second.cur_pos.first)/s2;
			y = ite->second.cur_pos.second+s1*(ite->second.des_pos.second-ite->second.cur_pos.second)/s2;

			ite->second.cur_pos = std::make_pair (x, y);
		}

	}

	return (0);
}

///////////////////////////////////////////////

bool football::check_ball_status ()
{
	if (ball.is_in_goal) 
		return (false); //neednot check

	if (ball.status == NON_CTL_NON_ACT || ball.status == IS_CTL_NON_ACT) 
		return (true); //need check 

	if (ball_snap.empty())  //Maybe only for a test FIXME
		return (true); //need check

	uint64_t tt;
	set_cur_time(tt);
	ball_t pre,next;

	ball_t b = ball_snap.front();

	//Maybe the last status cannot be handled, so proceed it specially
	if (ball_snap.size() == 1){ 
		ball = ball_snap.front();
		ball_snap.pop();
		//DEBUG_LOG("Only one ball snapshot in the queue : [%llu][%llu]", ball.b_tm, tt);
		goto Label;
	}

	/*
	 * avoid the players control the ball many times when the ball has just been ticked out ! so ignore to checke the
	 * ball status in the first period time
	 */
	if (tt < b.b_tm) { 
		//DEBUG_LOG("less than ball snap : [%llu][%llu][%llu]", b.b_tm, tt, b.b_tm-tt);
		return (false); 
	}
		
	//get the later snap
	pre = ball_snap.front();
	ball_snap.pop();
	next = pre; //if only one element in the queue
	while (!ball_snap.empty()) {
		next = ball_snap.front();
		ball_snap.pop(); 
		if (tt>=pre.b_tm && tt<=next.b_tm) {
			break;
		}
		pre = next;
	}

	if (tt>next.b_tm) {
		//The queue has been empty here
		DEBUG_LOG("Maybe a bad case here : cannot find the ball snap [%lu][%lu]", next.b_tm, tt);
	}

	ball = next;

Label:
	//2种情况可能同时发生
	//以下2种情况都是最后一个stampshot of the ball
	if (ball.status == NON_CTL_NON_ACT) {
		//DEBUG_LOG("Now, the queue is empty, the ball has stop");
		if (!ball_snap.empty())
			DEBUG_LOG("Serious error here [%d]", __LINE__);
	}

	if (ball.is_in_goal) {
		//DEBUG_LOG("Now, the queue is empty, the ball has been in goal");
		if (!ball_snap.empty())
			DEBUG_LOG("Serious error here [%d]", __LINE__);
		set_ball_shoot_infor();
		snd_ball_shoot_packet(ball.team_id, ball.b_tm);
		set_ball_init_status_event (); 
		return (false); //not check the ctl status
	}

	return (true);
}

typedef std::pair< uint32_t, uint64_t > dis_t;
struct  myclass {
	bool operator() (const dis_t &d1, const dis_t &d2) const { return (d1.second < d2.second); }
}myobj; 

int football::check_ctl_ball_status ()
{
	double v;
	std::vector< dis_t > dis;
	for (ball_player_t::iterator ite = ball_player.begin(); ite != ball_player.end(); ++ite)
	{
		v = sqrt (pow (ite->second.cur_pos.second-ball.b_pos.second, 2) + \
			pow (ite->second.cur_pos.first-ball.b_pos.first, 2));
		if (IS_CTL_RANGE(v)) 
			dis.push_back (std::make_pair (ite->first, ite->second.cur_tm));
	}
	if (dis.empty()) {
		return (0); //no ctl ball occur
	}

	std::vector< dis_t >::iterator max = std::max_element (dis.begin(), dis.end(), myobj);
	if ((ball.pertain!=max->first) || (ball.status==IS_CTL_IS_ACT))
	{
		//if ((ball.status==IS_CTL_IS_ACT) && !IS_CTL_PRO()) //The ball is moving and the probility----
		//	return 0;
		ball.b_rate = 0.0;  
		ball.pertain = max->first;
		ball.status = IS_CTL_NON_ACT;
		snd_ctl_player_packet (); //发送球的位置这儿 : 截球 或 静止 中

		ball_player[max->first].des_pos = ball_player[max->first].cur_pos; //set non-action
		
		//control the ball, so empty the trace information of the ball
		while(!ball_snap.empty())
			ball_snap.pop();
		//DEBUG_LOG("The player [%u] control the ball", ball.pertain);
	}

	return (0);
}


int football::check_court_status ()
{
	check_player_status ();
	if (check_ball_status ()) {
		if (!(ball.status == NON_CTL_IS_ACT && ball.kick == false))
			check_ctl_ball_status ();   //ignore the checking
	}

	return (0);
}

///////////////////////////////////////////global methods
extern "C" int game_init ()
{
	return (0);
}

}
#endif   /* ----- #ifndef FOOTBALLDEF_INC  ----- */

