#ifndef BATTLE_ROUND_HPP_
#define BATTLE_ROUND_HPP_

#include <stdint.h>
#include <map>
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include <string.h>

using namespace taomee;


class battle_round
{
public:
	battle_round()
	{
		memset(&m_begin_time, 0, sizeof(m_begin_time)) ;	
		m_duration_time = 0;
	}
	virtual ~battle_round(){}

	void init_battle_round(struct timeval begin_time, uint32_t duration_time)
	{
		m_begin_time = begin_time;
		m_duration_time = duration_time;
	}
	
	bool is_battle_round_finish(struct timeval cur_time)
	{
		return taomee::timediff2(cur_time, m_begin_time) >  (int)m_duration_time*1000;
	}

	void round_finish(struct timeval cur_time)
	{
		m_begin_time = cur_time;	
	}

	void kill_battle_round( struct timeval cur_time )
	{
		m_begin_time = cur_time;		
	}

	struct timeval  m_begin_time;
	uint32_t    	m_duration_time;
};




#endif
