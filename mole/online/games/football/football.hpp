/*
 * author : spark
 * date : 20080504
 */


#ifndef  FOOTBALL_INC
#define  FOOTBALL_INC

//standard headers inheried from ISO C
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

//ISO C++ headers
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <stdexcept>

extern "C" {
#include <libtaomee/timer.h>
#include "proto.h"
#include "benchapi.h"
#include <libtaomee/log.h>
#include "dll.h"
#include "map.h"
#include "dbproxy.h"
#include "service.h"
#include <libtaomee/crypt/qdes.h>
}

/*
 * The rate of the ball cannot be too large !!!
 * if the rate is too large, the space of the collision points information  which will be written to the `msg` array will be
 * more than the `msg` array. So this data will occupy the other data section space of this process. some data will be 
 * destroyed and will access the original data which has been destroyed later, this data will be garbage !!!
 */

/*
 * some other problem if the rate of the ball is too large :
 * because the rate is too large, so when the ball move from an edge to another edge, the rate difference between these two 
 * collisions will be ignored ! (because it cannot be expressed here) ! So when compute the track of the ball, there will
 * be an infinity loop !
 */

/*
 * Maybe, the space of the collision points information is more than the `msg` array even if the rate of the ball is not too 
 * large, some error will exist ! So judge the space before write it to the array. if so, ignore the track of the ball this 
 * time.
 */

namespace spk {

class football {
	public:
		typedef std::pair< uint32_t, uint32_t > int_pos_t;
		typedef std::pair< double, double > flt_pos_t;
	private:

		//player team
		enum BallTeam {
			RED = 0,
			BLUE
		};

#define IS_VALID_TEAM(id) (id==RED || id==BLUE)
		
		//ball attributes
		enum BallStatus {
			NON_CTL_NON_ACT = 0,
			IS_CTL_NON_ACT,
			IS_CTL_IS_ACT,
			NON_CTL_IS_ACT 
		};

		typedef struct ball {
			double		b_rate;
			flt_pos_t 	b_pos;
			flt_pos_t 	dir_pos;
			uint32_t 	pertain;
			BallStatus	status;
			bool		is_in_goal;
			BallTeam	team_id;  
			uint64_t 	b_tm;
			bool		kick; //kicked by player --true; by goal --false, Only valid when the status is ACT
			ball () { }
			ball (double b_r, flt_pos_t b_p, int_pos_t d_p, uint32_t p, BallStatus s, uint64_t bt) 
				: b_pos (b_p) , dir_pos (d_p){
				b_rate = b_r;  pertain = p; status = s; is_in_goal = false; b_tm = bt;
			}
			ball (const struct ball &copy) : b_pos (copy.b_pos) {
				b_rate = copy.b_rate; b_pos = copy.b_pos; dir_pos = copy.dir_pos;
				pertain = copy.pertain; status = copy.status; is_in_goal = copy.is_in_goal;
				b_tm = copy.b_tm;  team_id = copy.team_id; kick = copy.kick;
			}
			struct ball &operator= (struct ball &copy) {
				b_rate = copy.b_rate; b_pos = copy.b_pos; dir_pos = copy.dir_pos;
				pertain = copy.pertain; status = copy.status; is_in_goal = copy.is_in_goal;
				b_tm = copy.b_tm;  team_id = copy.team_id;  kick = copy.kick;
				return (*this);
			}
		} ball_t;


		std::queue<ball_t>  ball_snap;
		ball_t      trace_ball;

		//player and player team attributes
		typedef struct player_team {
			uint32_t player_num;
			uint32_t goal_ball_cnt;
			player_team () { player_num = 0; goal_ball_cnt = 0; }
		} player_team_t;

		typedef std::map<BallTeam, player_team_t> ball_team_t;

#define set_cur_time(curr_time)  do{\
	curr_time = static_cast<uint64_t>(::now.tv_sec) * 1000 + static_cast<uint64_t>(::now.tv_usec) / 1000;\
}while(0)


#define add_cur_time(stamp, val) do{\
	stamp += val;\
}while(0)

#define cvt_cur_time(millisecond, second, rmmillsecond) do {\
	second = static_cast<uint32_t>(millisecond/1000);\
	rmmillsecond = static_cast<uint32_t>(millisecond%1000);\
}while(0)

#define CTL_DIS 20
#define IS_CTL_RANGE(dis) (dis< CTL_DIS)

#define CTL_PRO	 0.7
#define CTL_VAL	 10

		//In this distance, donot consider the control-ball occur, except the collision, shooting or stillness
		static const double non_ctl_dis = 1.5 * CTL_DIS;

		typedef struct player {
			BallTeam 	team_id;
			uint64_t  	enter_tm;  //The time when the player enter the court
			uint64_t  	cur_tm;   //record the current time when the player is moving
			uint32_t    goal_ball_cnt;  
			uint32_t    bar_ball_cnt;   
			flt_pos_t   cur_pos;    //record the current position when the player is moving
			flt_pos_t   des_pos;    //desitination position
			player() { }
			player (BallTeam ti, const flt_pos_t &pos) {
				set_cur_time(enter_tm); cur_tm = enter_tm;
				team_id = ti; goal_ball_cnt = bar_ball_cnt = 0; cur_pos = des_pos = pos;
			}
			inline bool operator< (const struct player &com) {
				return (cur_tm<com.cur_tm);
			}
			inline bool operator> (const struct player &com) {
				return (cur_tm>com.cur_tm);
			}
			inline bool operator== (const struct player &com) {
				return (cur_tm==com.cur_tm);
			}
		} player_t;

		typedef std::map< uint32_t, player_t > ball_player_t;

		struct goal_t {
			uint32_t pos_x, pos_y_l, pos_y_h;
			goal_t (uint32_t x, uint32_t y_l, uint32_t y_h) { pos_x = x; pos_y_l = y_l; pos_y_h = y_h; }
			goal_t () { }
		};
		typedef std::map< BallTeam, struct goal_t> ball_goal_t;


		///////////////////
		typedef struct game_bonus {
			uint32_t score;
			uint32_t mole_bean;
			uint32_t exp;
			uint32_t strong;
			uint32_t iq;
			uint32_t charm;
			uint32_t xiaomee;
		}__attribute__((packed)) game_bonus_t;

	private:
#define SND_JOIN_GM     431
#define SND_BALL_GOAL   432
#define SND_CTL_PLAYER    433
#define SND_GM_REMAIN   434
#define CHG_POS_REQ     435
#define SND_BALL_POS   436
#define SND_SHOT_BALL	  437
#define QUIT_GAME       407
#define QUIT_TO_MAP	441
#define SND_CRT_USER 440
#define SND_GM_OVER     10006
#define SND_USER_POS  438
	
	private:
		static const uint32_t scan_time = 100; //The interval time the platform scans the event 
		static const uint32_t interval_time = 100 + scan_time; //The factual interval time when check court status
		static const uint32_t trace_time = interval_time / 4;
		static const uint32_t ball_remain_time = 1900 + scan_time; //stay for 2 seconds

		static const uint32_t   game_last_time =  10 *60 * 1000;
		//static const uint32_t   game_last_time = 1 * 60 * 1000 + 5 * 1000;
		struct timeval tm_start;
		struct timeval tm_remain, tm_remain1;
		struct timeval tm_end;
		static const double a = 0.00008;  //加速度， 相当于摩擦系数 m/ms^2
		//static const uint32_t ROW = 843;
		//static const uint32_t COL = 312;
		static const uint32_t ROW = 818;
		static const uint32_t COL = 292;
		double ball_move_rate[2];

		//坐标转换之用
		//the following is only to the ball, not the players
		static const uint32_t CVT_LD_X_PLAYER = 50;  //for the players
		static const uint32_t CVT_LD_X = 65;

		static const uint32_t CVT_LD_Y_PLAYER = 65;
		static const uint32_t CVT_LD_Y = 85;

		static const uint32_t CVT_RU_X_PLAYER = 893;
		static const uint32_t CVT_RU_X = 883;

		static const uint32_t CVT_RU_Y_PLAYER = 377;
		static const uint32_t CVT_RU_Y = 377;

		//player attributes
		static uint32_t const MAX_PLAYER_NUM = 5;
		static const double p_rate = 0.10; //player rate 
		ball_team_t     ball_team;
		ball_player_t   ball_player;

#define CHK_POS_VALID(pos) do{\
	if (pos.first>=CVT_LD_X_PLAYER && pos.first<=CVT_RU_X_PLAYER && pos.second>=CVT_LD_Y_PLAYER && \
			pos.second<=CVT_RU_Y_PLAYER);\
	else {\
		DEBUG_LOG("invalid player position from the client here : [%u, %u]",pos.first, pos.second);\
		return (0);\
	}\
}while(0)

//pre_condition : the position has already been valid here
#define IS_EDGE_POS(pos) ((pos.first>=CVT_LD_X_PLAYER && pos.first<=CVT_LD_X) || \
		(pos.first>=CVT_RU_X && pos.first<=CVT_RU_X_PLAYER) || \
		(pos.second>=CVT_LD_Y_PLAYER && pos.second<=CVT_LD_Y) || \
		(pos.second>=CVT_RU_Y && pos.second<=CVT_RU_Y_PLAYER))

#define CVT_EDGE_POS(pos) do {\
	if (pos.first>=CVT_LD_X_PLAYER&&pos.first<=CVT_LD_X) pos.first=CVT_LD_X;\
	if (pos.first>=CVT_RU_X&&pos.first<=CVT_RU_X_PLAYER) pos.first=CVT_RU_X;\
	if (pos.second>=CVT_LD_Y_PLAYER&&pos.second<=CVT_LD_Y) pos.second=CVT_LD_Y;\
	if (pos.second>=CVT_RU_Y&&pos.second<=CVT_RU_Y_PLAYER) pos.second=CVT_RU_Y;\
}while(0)

#define IS_GOAL_POS(pos) ((pos.first<=CVT_LD_X && (pos.second>=65+10&&pos.second<=377-10)) || \
		(pos.first>=CVT_RU_X && (pos.second>=65+10&&pos.second<=377-10)))

//pre_condition : the position has already been in out
#define CVT_GOAL_POS(pos) do {\
		if (pos.first<=CVT_LD_X) pos.first = CVT_LD_X;\
		if (pos.first>=CVT_RU_X) pos.first = CVT_RU_X;\
}while(0)

#define MAP_TO_CRT(pos) do{\
	pos.first = pos.first - CVT_LD_X;\
	pos.second = pos.second - CVT_LD_Y;\
}while(0)

#define CRT_TO_MAP(pos) do{\
	pos.first = pos.first + CVT_LD_X;\
	pos.second = pos.second + CVT_LD_Y;\
}while(0)

#define SAME_POINT_DIS  2
#define IS_LESS(s1,s2) (s1-s2<-SAME_POINT_DIS)
#define IS_MORE(s1,s2) (s1-s2>SAME_POINT_DIS)
#define IS_EQUAL(s1,s2) (fabs(s1-s2)<=SAME_POINT_DIS)



#define GEN_RAND_RATE(CoPlIcaTicAte) do{\
	CoPlIcaTicAte = 0.3; \
}while(0)

//CoPlIcaTicAte = 0.225; 
#define MAX_MOV_DIS		500
#define GEN_RAND_RATE_BY_DIS(CoPlIcaTicAte,dis) do{\
	dis = (dis+100 > 500) ? 500 : (dis+100);\
	CoPlIcaTicAte = sqrt(2 * a * dis);\
}while(0)

//reserve 100 bytes
#define MAX_TRK_POT ((sizeof(msg)-4-4-1-17-100)/20)

#define GEN_RAND_POS(player_pos, team_id) do{\
	player_pos.first = (team_id==RED) ? (ROW/2 - 100) : (ROW/2 + 100);\
	player_pos.second = random () % COL;\
}while(0)

		ball_t 		ball;
		ball_goal_t	ball_goal;

		uint64_t game_start_time;
		game_group_t *m_gamegrp;
		map_t *tile;
		uint8_t msg[4096];
		bool game_start_token;

		inline void initBallGoalPos (); 
		inline void initBallPlayer (); 
		inline void initBallTeam (); 
		inline void initBallStatus ();
	protected:
		inline bool IS_SHOOT (flt_pos_t &pos, BallTeam &goal);
		inline bool IS_CTL_PRO ();
		inline void set_ball_init_status();
		inline void set_ball_rand_status();
		inline void set_ball_stop_status(flt_pos_t &pos, uint64_t tt);
		inline void set_ball_shoot_status(BallTeam goal, uint64_t tt);
		inline void set_ball_move_status(flt_pos_t &pos, flt_pos_t &dir, double rate, uint64_t tt);
		inline void set_ball_coll_move_status (flt_pos_t &pos, flt_pos_t &direction, uint32_t t);
		inline void set_ball_shoot_infor();
		inline double get_next_stop_pos (uint32_t &t_stop, flt_pos_t &pos_stop);
		inline double get_next_coll_pos (uint32_t &t_stop, flt_pos_t &pos_stop);
		inline double get_next_move_pos (uint32_t &t_stop, flt_pos_t &pos_stop);
		inline double get_next_ball_rate (uint32_t t); //if the result is less than zero, return zero
		inline flt_pos_t get_dir_pos (const flt_pos_t &cur, const flt_pos_t &des); //used for the same line direction
		inline flt_pos_t get_next_ball_dir (); //used for the next collision direction
		inline void send_game_bonus (sprite_t *p, game_bonus_t *gbt);
		//inline void change_back_clothes (sprite_t *p);
		inline int get_game_bonus (uint32_t user_id, game_bonus_t *gbt);
		inline uint32_t get_ball_degree ();
		inline void do_shoot_special_pos (ball_t &trace_ball, BallTeam shoot_goal, uint64_t &);
		inline int submit_game_score (sprite_t *p);
		//inline void update_player_attr (sprite_t *p, game_score_t *pgs);
		inline int add_player (sprite_t *p, BallTeam team_id);
		inline void occur_start_pos (flt_pos_t &pos);
		inline int trace_ball_move_snap (std::vector< ball_t > &coll_ball, bool &shoot);
		inline void snd_ball_move_snap (std::vector< ball_t > &coll_ball, bool flag);
		inline void snd_join_team_packet (sprite_t *p);
		inline void snd_crt_infor (sprite *p);
		inline void snd_ball_shoot_packet (BallTeam , uint64_t);
		inline void snd_ctl_player_packet ();
		inline void snd_ball_pos_packet ();
		inline int snd_crt_user ();
		inline int snd_user_pos (sprite_t *p);
		//处理player点击鼠标时间， 处理POS位置改变
		inline int change_player_pos (sprite_t *p, const uint8_t body[], int len);
		inline void set_ctl_ball_status (int_pos_t &, double dis);
		//处理S-->C，事件触发时，才发送packet
		inline int check_court_status ();
		inline int check_player_status ();
		inline bool check_ball_status ();
		inline int check_ctl_ball_status ();
		inline int quit_game_req (sprite_t* p);
	public:
		void  init (sprite_t* p);
		int   handle_data (sprite_t* p, int cmd, const uint8_t body[], int len);
		bool is_game_start () const {
			 return (game_start_token); 
		};
		void set_game_start ();
		int join_game_req (sprite_t* p);
		football (game_group_t* grp, map_t *tl);
		~football ();

		//used for the calling of the function pointers
		void set_game_remain (void *, void *arg);
		void set_game_remain1 (void *arg, void *arg1);
		void set_game_over (void *, void *arg);
		void set_ball_init_status_event ();
		void chk_game_status ();
		friend int notify_game_remain (void *, void *);
		friend int notify_game_remain1 (void *, void *);
		friend int notify_game_over (void *, void *);
		friend int notify_game_status (void *, void *);
		friend int notify_ball_init_status (void *, void *);
};

}
#endif   /* ----- #ifndef FOOTBALL_INC  ----- */

