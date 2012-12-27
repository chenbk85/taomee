#ifndef  CTIMER_H
#define  CTIMER_H

#include <string.h>
#include  <stdint.h>
#include <stdlib.h>
#include  <time.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/log.h>
#ifdef __cplusplus
}
#endif

class Ctimer_node{
	public:
		uint32_t 	dealtime;//处理时间
		uint32_t 	function_index;
		uint32_t 	seqid;//自己的定时器中的唯一序列号
		uint32_t 	key;//一般是userid, 全局的设置为0 
		char 		data[16];
		//数据扩展模式
		uint32_t 	data_len;
		char*   	data_ex;
		bool operator==(const Ctimer_node &t1)const{
			return this->key==t1.key && this->seqid==this->seqid;
     	} 
		bool operator<(const Ctimer_node &t1)const{
			if (this->key<t1.key ){
				return true;
			}else if (this->key==t1.key ){
				return  this->seqid<t1.seqid;
			}else{
				return false;

			}
     	} 
};

class Ctimer_key_info{
	public:
		uint32_t 	seqid;//自己的定时器中的唯一序列号
		uint32_t 	deal_time;//处理时间
		bool operator==(const Ctimer_key_info &t1)const{
			return this->seqid==t1.seqid;
     	} 
		bool operator<(const Ctimer_key_info &t1)const{
			return this->seqid<t1.seqid;
     	} 
};



typedef   void (*event_func_t)(uint32_t key, void*data,int data_len);
//定时器
#include  <set>
#include  <map>
#include  <list>
//#include  <mutimap>

class Ctimer{
	private:
		static const uint32_t max_function_count=1000;
    	event_func_t function_list[max_function_count ];
		uint32_t cur_seqid;//用于保存唯一键值
		uint32_t  get_seqid(){
			this->cur_seqid++;				
			return this->cur_seqid;
		};
		//key <->set<seq> ; 保存用户的定时器,key一般就是userid
		std::map<uint32_t,std::set<Ctimer_key_info> > key_map;

		std::map<uint32_t,std::set<Ctimer_node> > time_map;

		uint32_t add_seq_to_key_map(uint32_t key,uint32_t deal_time){
			uint32_t seqid;
			std::map<uint32_t,std::set<Ctimer_key_info > >::iterator it;
			it=this->key_map.find(key);
			if ( it==this->key_map.end() ){//没有
				seqid=this->get_seqid();
				Ctimer_key_info item;
				item.deal_time=deal_time;
				item.seqid=seqid;
				this->key_map[key].insert(item);
				return seqid;
			}else{
				std::set<Ctimer_key_info > & seqid_set= it->second;
				while (true){
					seqid=this->get_seqid();
					std::pair<std::set<Ctimer_key_info >::iterator,bool> ret;
					Ctimer_key_info item;
					item.deal_time=deal_time;
					item.seqid=seqid;
					ret=seqid_set.insert(item);
					if(ret.second){//加入成功
						return seqid;	
					}
				}

			}
		
		}
		//会将到期的事件删除
		inline void get_event_list_and_del(uint32_t dealtime, 
				std::list<Ctimer_node> & time_node_list )
		{
			std::map<uint32_t, std::set<Ctimer_node> >::iterator it;

			for (it=this->time_map.begin();it!=this->time_map.end(); ){
				if (dealtime>=it->first ){
					time_node_list.insert(time_node_list.end() , it->second.begin(),it->second.end() );
					//it++,不是++it
					this->time_map.erase(it++);	
				}else{//没有需要操作的
					break;
				}
			}
		}
	public:		
		Ctimer(){
			this->cur_seqid=0;
		}
		~Ctimer(){
		}
		void init_function_list(){
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
		void del_by_key(uint32_t key ){
			if(key==0){
				DEBUG_LOG("ERROR TIME key=0");
				return;
			}
			
			std::map<uint32_t,std::set<Ctimer_key_info > >::iterator it;
			it=this->key_map.find(key);
			if ( it!=this->key_map.end() ){//找到了
				std::set<Ctimer_key_info >::iterator set_it;
				std::set<Ctimer_key_info > &key_set= it->second;
				for(set_it=key_set.begin();set_it!=key_set.end();++set_it ){
					//删除time_map 中的数据
					const Ctimer_key_info &key_item=*set_it;
					Ctimer_node timer_node;
 					timer_node.key=key;
 					timer_node.seqid=key_item.seqid;
					uint32_t del_count;
					del_count=this->time_map[key_item.deal_time].erase( timer_node );
				}
				//删除用户的节点
				this->key_map.erase(it);
			}

		}

		//如果是用户自身的定时器，则key 设置为userid,   
		//   在用户下线时调用 del_by_key(userid),才能将其相关的定时器清除
		bool add_timer(uint32_t  deal_time, uint32_t function_id  ,
				uint32_t  key,void* data=NULL,uint32_t datalen=0 )
		{
			if (data!=NULL && datalen==0  ){
				return false;
			}
			if (data==NULL && datalen!=0  ){
				return false;
			}
			if (function_id  >= this->max_function_count  ){
				return false;
			}
			if (this->function_list[function_id]==NULL ){
				return false;
			}

			Ctimer_node timer_node;
			//处理时间
			timer_node.dealtime=deal_time;
			//设置定时器的seqid
			timer_node.seqid= this->add_seq_to_key_map(key, deal_time );
			timer_node.key=key;
			timer_node.function_index=function_id;
			timer_node.data_len=datalen;
			timer_node.data_ex=NULL;

			if (datalen<=sizeof( timer_node.data) ){
				memcpy(timer_node.data,data,datalen  );
			}else{
				timer_node.data_ex=(char*) malloc(datalen );
				memcpy(timer_node.data_ex,data,datalen  );
			}

			this->time_map[deal_time].insert(timer_node );
			return true;
		}

		inline void deal_timer(uint32_t now)
		{
			std::list<Ctimer_node> timer_node_list;
			//得到要处理的事件列表
			this->get_event_list_and_del(now,timer_node_list  );
			std::list<Ctimer_node>::iterator it;
			for (it= timer_node_list.begin();it!=timer_node_list.end();++it ){
				Ctimer_node& timer_node=*it;
				if ( timer_node.data_len<=sizeof( timer_node.data) ){
					(*(this->function_list[ timer_node.function_index]))(
							 timer_node.key, timer_node.data, timer_node.data_len);		
				}else{
					(*(this->function_list[ timer_node.function_index]))(
							 timer_node.key, timer_node.data_ex, timer_node.data_len);		
					free(timer_node.data_ex);
				}

				Ctimer_key_info	 key_item;
				key_item.seqid=timer_node.seqid;
			};
		}
	

};
#endif  /*CTIMER_H*/
