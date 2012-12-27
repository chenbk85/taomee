#ifndef  CTIMER_H
#define  CTIMER_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include  <map>
#include "Ctimeval.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/log.h>
#ifdef __cplusplus
}
#endif

enum {
	timer_key_map_time	= 1,
};
class Ctimer_node_key {
public:
	uint32_t 	key;//一般是userid, 全局的设置为0 
	uint32_t 	seqid;//自己的定时器中的唯一序列号
	Ctimer_node_key(uint32_t kid, uint32_t sid) : key(kid), seqid(sid){}
	bool operator<(const Ctimer_node_key &t)const
	{
		return key < t.key || (key == t.key && seqid < t.seqid);
   	} 
};

class Ctimer_node_val{
public:
	uint32_t 	function_index;
	char 		data[16];
	//数据扩展模式
	uint32_t 	data_len;
	char*   	data_ex;
};

typedef std::map<Ctimer_node_key, Ctimer_node_val> Ctimer_node_map;

typedef   void (*event_func_t)(uint32_t key, void*data,int data_len);

class Ctimer{
	private:
		static const uint32_t max_function_count=1000;
    	event_func_t function_list[max_function_count ];
		uint32_t cur_seqid;//用于保存唯一键值
		uint32_t  get_seqid(){
			this->cur_seqid++;				
			return this->cur_seqid;
		};
		//key <->map<seq, time> ; 保存用户的定时器,key一般就是userid
		std::map<uint32_t, std::map<uint32_t, TimeVal> > key_map;

		//time <->
		std::map< TimeVal, Ctimer_node_map > time_map;

		uint32_t add_seq_to_key_map(uint32_t key, const TimeVal& deal_time){
			uint32_t seqid;
			std::map<uint32_t, std::map<uint32_t, TimeVal> >::iterator it;
			it=this->key_map.find(key);
			if ( it==this->key_map.end() ){//没有
				seqid=this->get_seqid();
				this->key_map[key][seqid] = deal_time;
				return seqid;
			}else{
				std::map<uint32_t, TimeVal> & seqid_set= it->second;
				while (true){
					seqid=this->get_seqid();
					std::pair<std::map<uint32_t, TimeVal>::iterator,bool> ret;
					ret=seqid_set.insert(std::pair<uint32_t, TimeVal>(seqid, deal_time));
					if(ret.second){//加入成功
						return seqid;	
					}
				}

			}
		}
	public:		
		Ctimer() { this->cur_seqid=0;}
		void init_function_list()
		{
			memset(&this->function_list,0,sizeof(this->function_list));
		}
		bool add_function(uint32_t id, event_func_t event_func  ){
			if (id<this->max_function_count){
				this->function_list[id]	=event_func;
				return true;
			}else{
				return false;
			}
		}
		//一般是userid,在用户离开时调用 
		void del_by_key(uint32_t key)
		{
			if(key==0){
                DEBUG_LOG("ERROR TIME key=0");
                return;
            }

			std::map<uint32_t,std::map<uint32_t, TimeVal> >::iterator it;
			it=this->key_map.find(key);
			if ( it!=this->key_map.end() ){//找到了
				std::map<uint32_t, TimeVal>::iterator set_it;
				std::map<uint32_t, TimeVal> &key_set= it->second;
				for(set_it=key_set.begin();set_it!=key_set.end();++set_it ){
					//删除time_map 中的数据
					Ctimer_node_key timer_node(key, set_it->first);
					uint32_t del_count;
					del_count=this->time_map[set_it->second].erase( timer_node );
				}
				//删除用户的节点
				this->key_map.erase(it);
			}

		}

		//如果是用户自身的定时器，则key 设置为userid,   
		//   在用户下线时调用 del_by_key(userid),才能将其相关的定时器清除
		uint32_t add_timer(const TimeVal&  deal_time, uint32_t function_id  ,
				uint32_t  key,void* data=NULL,uint32_t datalen=0 )
		{
			if (function_id  >= this->max_function_count  ){
				return 0;
			}
			if (this->function_list[function_id]==NULL ){
				return 0;
			}

			uint32_t seq = this->add_seq_to_key_map(key, deal_time);
			Ctimer_node_key node_key(key, seq);
			Ctimer_node_val node_val;

			//设置定时器的seqid
			node_val.function_index=function_id;
			node_val.data_len=datalen;
			node_val.data_ex=NULL;

			if (datalen<=sizeof( node_val.data) ){
				memcpy(node_val.data,data,datalen  );
			}else{
				node_val.data_ex=(char*) malloc(datalen );
				memcpy(node_val.data_ex,data,datalen  );
			}

			this->time_map[deal_time].insert(std::pair<Ctimer_node_key, Ctimer_node_val>(node_key, node_val));
			//KDEBUG_LOG(key,"ADD TIME  deal_time=%u, function_id=%u, seqid=%u",deal_time ,function_id, timer_node.seqid  );
			return node_key.seqid;
		}

		bool mod_expired_time(uint32_t key, uint32_t seqid, const TimeVal& dealtime)
		{
			std::map<uint32_t,std::map<uint32_t, TimeVal> >::iterator it;
			it=this->key_map.find(key);
			if ( it==this->key_map.end() )
				return false;
			
			std::map<uint32_t, TimeVal>::iterator set_it = it->second.find(seqid);
			if (set_it == it->second.end())
				return false;

			if (set_it->second == dealtime)
				return false;

			Ctimer_node_key node_key(key, seqid);
			Ctimer_node_val node_val = this->time_map[set_it->second][node_key];
			
			this->time_map[dealtime].insert(std::pair<Ctimer_node_key, Ctimer_node_val>(node_key, node_val));	
			this->time_map[set_it->second].erase(node_key);
			this->key_map[key][seqid] = dealtime;
			return true;
		}

		inline void deal_timer(const TimeVal& now)
		{
			std::map<TimeVal, Ctimer_node_map >::iterator it;

			for (it=this->time_map.begin();it!=this->time_map.end();){
				if (it->first < now){
					Ctimer_node_map& node_map = it->second;
					Ctimer_node_map::iterator tit;
					for (tit = node_map.begin(); tit != node_map.end(); tit ++) {
						const Ctimer_node_key& key = tit->first;
						Ctimer_node_val& val = tit->second;
						if ( val.data_len<=sizeof(val.data) ){
							(*(this->function_list[val.function_index]))(key.key, val.data, val.data_len);		
						}else{
							(*(this->function_list[val.function_index]))(key.key, val.data_ex, val.data_len);		
							free(val.data_ex);
						}
					}
					//it++,不是++it
					this->time_map.erase(it++);	
				}else{//没有需要操作的
					break;
				}
			}
		}
};
#endif  /*CTIMER_H*/
