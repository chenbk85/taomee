#ifndef  SPRITE_H
#define  SPRITE_H

#include "util.h"
#include  "./proto/pop_online.h" 

class Csprite;

//缓存的报文
class Cbuffer_list{
	private:
		std::list<std::vector<char> > buffer_list;
	public:	
		void add_buffer(char *data ){
			this->buffer_list.push_back(
					std::vector<char>(data,data+*((uint32_t*)(data)))
			);
		}
	 	inline uint32_t size()	{
			return this->buffer_list.size();
		}
		char * get_first_buffer(){
			if(this->buffer_list.size()>0){
				return &((*this->buffer_list.begin())[0]);
			}else{
				return NULL;
			}
		}
		void pop_front(){
			this->buffer_list.pop_front();
		}

};

class Citem_ex{
	public:
		uint32_t itemid;
		uint32_t use_count;//使用个数
		uint32_t count;//总个数
};

class Cday_item {
	public:
		uint32_t itemid;
		uint32_t total;
		uint32_t daycnt;
};

class Ctask{
	public:
		uint32_t taskid;
		uint32_t task_nodeid;
};

class Cuser_log{
	public:
		uint32_t logtime;
		uint32_t v1;
		uint32_t v2;
};	
class Cuser_log_list {
	private:
		std::vector<Cuser_log> user_log_list;
	public:
		void init( std::vector<user_log_t> & log_list );
		void add(Cuser_log & item );
		void add(uint32_t v1, uint32_t v2);
		void get_list( std::vector<user_log_t> & log_list );

};	
class Citem_list {
	private:
		std::set<uint32_t> can_change_itemid_set;//个人可换的列表

	public:
		std::map<uint32_t ,Citem_ex >  item_list;
		bool check_enough(uint32_t itemid, uint32_t count );
		bool set_cloth_used_list(std::vector<uint32_t> itemid_list, 
				std::vector<uint32_t> &del_can_change_list,//需要删除的
				std::vector<uint32_t> &set_noused_list,//设置used_count=0
				std::vector<uint32_t> &add_can_change_list,//需要增加的
				std::vector<uint32_t> &set_used_list //设置used_count=1
				);

		bool set_effect_used(uint32_t itemid, uint32_t& unset_itemid);
		bool set_effect_unused(uint32_t itemid);

		bool add(uint32_t itemid, uint32_t count , bool is_set_use =false );
		bool del(uint32_t itemid, uint32_t count );
		void init(const std::vector<item_ex_t>  & item_list);
		void get_used_list( std::vector<uint32_t> & use_cloth_list,		// 使用中的装扮列表
							std::vector<uint32_t> & effect_list,		// 使用中的特效卡片列表
							std::vector<uint32_t> & medal_list);		// 已经得到的奖牌列表
		void get_used_list( std::vector<uint32_t> & use_cloth_list,		// 使用中的装扮列表
							std::vector<uint32_t> & effect_list);		// 使用中的特效卡片列表
		bool item_can_change(uint32_t itemid);

		void get_itemid_list(uint32_t startid, uint32_t endid, std::vector<uint32_t > & out_list );
		void get_comprise_item_list(uint32_t itemid, std::vector<item_t> & comprise_list);
};

class Cspec_item_list {
	private:
		std::map<uint32_t, Cday_item> item_list;
		Csprite* p;
	public:
		bool init(Csprite* obj_p, const std::vector<item_day_limit_t>& item_day_list);
		void get_list(uint32_t start, uint32_t end, std::vector<item_day_limit_t>& item_day_list);
		uint32_t add(uint32_t itemid, uint32_t count, bool is_sync_db = false);
		bool check_change_count(uint32_t itemid, int count);
		uint32_t del(uint32_t itemid, uint32_t count, bool is_sync_db = false);
		uint32_t get_count(uint32_t itemid);
		uint32_t get_day_count(uint32_t itemid);
		int change_count(uint32_t itemid, int count, bool is_sync_db = true);
};

class Ctask_list {
	private:

	public:
		//islanid ->set<tasknodeid>
		std::map<uint32_t ,std::set<uint32_t> >  taskmap;
		uint32_t finish_task_count;
		void show();

		void init(const std::vector<task_t>  & task_list );
		bool add(uint32_t taskid,uint32_t task_nodeid );
		bool del(uint32_t taskid,uint32_t task_nodeid );
		bool check_existed_node(uint32_t taskid,uint32_t task_nodeid );
		void get_nodeid_list (uint32_t taskid,std::vector<uint32_t> &nodeid_list  );
		//得到已经完成的任务列表
		void get_completed_taskid(std::vector<uint32_t> &taskid_list);

};

struct Cgame_stat {
		uint32_t		win_time;	// 赢的次数
		uint32_t		lose_time;	// 输的次数
		uint32_t		draw_time;	// 平的次数
};

class Cgame_stat_map {
	private:
		std::map<uint32_t, Cgame_stat> game_stat_map;	
		Csprite* p;
	public:
		void init(Csprite* obj_p, std::vector<game_info_t>& game_stat_list);
		void add_game_stat(uint32_t gameid, uint32_t win_flag, bool is_sync_db = true);
		void get_game_state_list(std::vector<game_info_t>& game_stat_list);
};

enum {
	user_flag_reg			= 0x01,
	user_flag_newid			= 0x02,
	user_flag_sample		= 0x04,
	user_flag_guide			= 0x08,
};


class Csprite {
	POP_PROPERTY_READONLY_DEFAULT(uint32_t, last_islandid)
	POP_PROPERTY_READONLY_DEFAULT(uint32_t, last_mapid)
	POP_PROPERTY_READONLY_DEFAULT(bool, is_in_muti_map)
	POP_PROPERTY_READONLY_DEFAULT(uint32_t, waitcmd)
	private:
		int32_t			waitseq;

	public:
		Csprite() {}
		void set_waitcmd(uint16_t cmdid, int seq );
		bool change_xiaomee(int32_t change_value );
		bool check_change_xiaomee(int32_t change_value );

		void change_experience(int count, bool sync_db = false);
		struct fdsession*	fdsess;
		uint32_t 			id;
		uint32_t 			register_time;
		uint32_t 			flag;//0x01:是否已经注册了
		uint32_t 			xiaomee;//钱
		uint32_t 			age ;
		uint32_t 			color ;
		char 				nick[NICK_LEN];
		uint32_t			level;

		//当前玩的游戏编号
		uint32_t 			game_map_id;
		uint32_t			wait_gameid;

		uint32_t			busy_state;

		void  set_islandid_and_mapid(uint32_t islandid,uint32_t mapid );
		void get_user_info( online_user_info_t * p_item	);

		void reset_level();

		uint32_t		last_x;
		uint32_t		last_y;

		uint32_t		last_login;
		uint32_t		online_time;
		uint32_t		login_time;

		// 获取本次登录时长
		uint32_t get_once_oltime();
		// 获取当日在线时长
		uint32_t get_day_oltime();
	
		inline bool is_logined(){
			return this->register_time >0 ;
		}
		//得到唯一的online_mapid 
		inline uint32_t get_onine_mapid( ){
			return (this->last_islandid<<16)+this->last_mapid;
		}

		Citem_list  item_list;
		Ctask_list task_list;
		Cspec_item_list spec_item_list;
		Cgame_stat_map game_stat_map;
		//
		std::map<uint32_t ,std::set<uint32_t> >  findmap_map;
 		Cbuffer_list 	cache_buffer_lst;
		Cuser_log_list  user_log_list;

		bool send_msg_ex( uint16_t cmdid,int ret, Cmessage *c_in,  bool completed );

		//用于用户请求的响应
		inline bool send_succ(Cmessage *c_in=NULL ){
			return this->send_msg_ex( this->waitcmd,0, c_in,true ) ;
		}
		//用于用户请求的响应
		inline bool send_err(  int ret  ){
			return this->send_msg_ex( this->waitcmd,ret, NULL,true) ;
		}
		//用于用户请求的响应
		inline bool send_db_err( int ret ){
			return this->send_err(ret+10000);
		}

		//发送系统通知,不是用户请求的响应
		inline bool send_succ_server_noti( uint16_t cmdid, Cmessage *c_in=NULL ){
			return this->send_msg_ex(cmdid,0, c_in,false) ;
		}

		//发送系统通知,不是用户请求的响应
		inline bool send_err_server_noti( uint16_t cmdid, int ret  ){
			return this->send_msg_ex(cmdid, ret, NULL,false) ;
		}

		void send_succ_login_response();
		void send_sync_time();
	
		void find_map_list_init(const std::vector<find_map_t>  & find_map_list );
		int find_map_list_add(uint32_t islandid,uint32_t mapid);
		bool find_map_list_check_existed(uint32_t islandid ,uint32_t mapid );
		void find_map_list_get_mapid_list (uint32_t islandid ,std::vector<uint32_t> &mapid_list  );
		void get_user_info( cli_get_user_info_out   &cli_out  );
		uint32_t get_valid_gamepoint();
		void noti_seat_changed(uint32_t side, uint32_t userid);
		void unseat_game();
		void noti_game_seat();
	
		void reset_online_timer();

		// 以下接口用户抽取一些物品
		/* used_item表示使用中的物品 */
		bool rand_get_item(uint32_t randid, std::vector<item_t> & item_list);
		bool rand_get_item(uint32_t randid, item_t& item_get);
		void add_login_count();
};


class Cfd_userid_map {
	private: 
		static const int max_fd=65536;
		uint32_t fd_userid_list[max_fd]; 
	public: 
		Cfd_userid_map(){
			memset(fd_userid_list,0,max_fd );
		};
		uint32_t get_userid_by_fd(int fd){
			if (fd>=0 && fd<this->max_fd ){
				return this->fd_userid_list[fd];
			}else{
				return 0;
			}
		}
		bool  map_fd_userid(int fd,  uint32_t userid){
			if (userid<=0) return false;
			if (fd>=0 && fd<this->max_fd ){
				this->fd_userid_list[fd]=userid;
				return true;
			}else{
				return false;
			}	
		}
		uint32_t unmap_fd_userid(int fd ){
			if (fd>=0 && fd<this->max_fd ){
				uint32_t userid= this->fd_userid_list[fd];
				this->fd_userid_list[fd]=0;
				return userid;
			}else{
				return 0;
			}	
		}
};

class Cgame_map;
class Csprite_map {
	private:
		std::map<uint32_t, Csprite > spritemap ; 
		Cfd_userid_map  fd_userid_map;
	public:
		Csprite_map();
		~Csprite_map();
		std::map<uint32_t, std::set<uint32_t> >online_map;

		//游戏数据
		Cgame_map* game_map;

		inline void get_userid_list(std::vector<uint32_t> &userid_list)
		{
			std::map<uint32_t, Csprite >::iterator it;
			for(it=this->spritemap.begin(); it!=this->spritemap.end();++it ){
				userid_list.push_back(it->second.id);
			}
		}

		void cd_map( uint32_t userid, cli_walk_in * p_in , bool is_login  );

		//给同一map 的其它人发信息
		void  noti_to_map_others(Csprite* p, uint32_t cmdid , 
		Cmessage * p_msg=NULL ){
			this->noti_to_map_ex(p,cmdid,p_msg,0,false);
		}
		void  noti_to_map_other(Csprite* p, uint32_t cmdid , 
		uint32_t obj_userid ,Cmessage * p_msg=NULL ){
			this->noti_to_map_ex(p,cmdid,p_msg,obj_userid,false );
		}
		void  noti_to_map_all(Csprite* p, uint32_t cmdid , Cmessage * p_msg=NULL ){
			this->noti_to_map_ex(p,cmdid,p_msg,0,true );
		}
	
		void  noti_to_map_ex(Csprite* p, uint32_t cmdid , 
		Cmessage * p_msg  ,uint32_t obj_userid ,bool is_all );

		//得同一个map的用户
		Csprite * get_user_in_same_map( Csprite *p,uint32_t obj_userid );

		inline Csprite * get_sprite(uint32_t userid )
		{
			std::map<uint32_t, Csprite > ::iterator it;
			it=this->spritemap.find(userid);			
			if( it!=this->spritemap.end() ){
				return &it->second;
			}else{
				return NULL;
			}
		}
		void set_all_user_offline();
		void noti_all(uint32_t cmd, Cmessage * p_msg, uint32_t except_uid = 0);
		inline Csprite * get_sprite_by_fd( int fd)
		{
			uint32_t userid=this->fd_userid_map.get_userid_by_fd(fd );
			if( userid==0 ){
				return NULL;
			}
			return this->get_sprite(userid);
		}

		Csprite * add_sprite (uint32_t userid, fdsession_t* fdsess )
		{
			if ( ! this->fd_userid_map.map_fd_userid(fdsess->fd,userid )){
				return NULL;
			}
			Csprite sprite;
			sprite.fdsess=fdsess;
			sprite.id=userid;
			sprite.register_time = 0;
			sprite.busy_state = 0;
			sprite.set_waitcmd(0, 0);
			std::pair< std::map<uint32_t ,Csprite >::iterator,bool > ret;
			ret=this->spritemap.insert(std::pair< uint32_t ,Csprite  >(userid,sprite ));

			if( ret.second ){//插入成功
				return &(ret.first->second);
			}else{
				return NULL;
			}
		}

		bool del_sprite_by_fd (  uint32_t fd ); 
};

class Ctask_node {
	public: 
		uint32_t task_nodeid;
		//前置任务id列表
		std::vector<uint32_t> req_taskid_list;
		//需要有哪些物品
		std::vector<Citem> 	req_item_list;
		//回收物品
		std::vector<Citem> 	recycle_item_list;
		//奖励物品
		std::vector<Citem> 	reward_item_list;
};
#include    "Citem_conf.h"
extern Citem_conf_map g_item_conf_map;

class Ctask_map {
	private:
		std::map<uint32_t, std::map<uint32_t, Ctask_node > >  taskmap; 
	public:
		void add_task_node(uint32_t islandid,Ctask_node task_node   ){
			this->taskmap[ islandid][task_node.task_nodeid]=task_node;
		}
		Ctask_node* get_task_node(uint32_t  islandid,uint32_t task_nodeid )
		{
			std::map<uint32_t, std::map<uint32_t, Ctask_node > >::iterator it;
			it=this->taskmap.find(   islandid);
			if( it==this->taskmap.end() ){
				return NULL;
			}
			std::map<uint32_t, Ctask_node >::iterator node_it; 
			node_it=it->second.find(task_nodeid);
			if( node_it==it->second.end() ){
				return NULL;	
			}
			return   &(node_it->second);
		}

		void show()
		{
			bool is_err=false;
			std::map<uint32_t, std::map<uint32_t, Ctask_node > >::iterator it;	
			for (it=this->taskmap.begin();it !=this->taskmap.end(); ++it ){
				uint32_t taskid=it->first;
				std::map<uint32_t, Ctask_node >::iterator node_it;
				std::map<uint32_t, Ctask_node > &node_map=it->second;
				for (node_it=node_map.begin();node_it !=node_map.end(); ++node_it ){
					uint32_t task_nodeid=node_it->first;
					Ctask_node *p_node=&node_it->second;
					DEBUG_LOG("TASK taskid=%u task_nodeid=%u",taskid,task_nodeid );
					uint32_t i=0;		
					for (i=0;i<p_node->req_taskid_list.size();i++  ){
						DEBUG_LOG("\t req_task_nodeid:%u",p_node->req_taskid_list[i] );
					}
					for (i=0;i<p_node->req_item_list.size();i++  ){
						DEBUG_LOG("\t req:%u",p_node->req_item_list[i].itemid );
					}

					for (i=0;i<p_node->recycle_item_list.size();i++  ){
						DEBUG_LOG("\t del:%u",p_node->recycle_item_list[i].itemid );
					}

					for (i=0;i<p_node->reward_item_list.size();i++  ){
						uint32_t itemid=p_node->reward_item_list[i].itemid;
						DEBUG_LOG("\t add:%u", itemid );
						if(g_item_conf_map.check_can_change(itemid)){
							is_err=true;
							ERROR_LOG("ERR:reward item is can change:itemid=%u",itemid );
						}
					}

				}
			}
			if(is_err){
				exit(-1);
			}
		}
};


#endif  /*SPRITE_H*/
