#ifndef BASE_TIMER_H
#define BASE_TIMER_H

#include <stdint.h>
#include <map>
#include <libtaomee/timer.h>
#include <libtaomee++/time/time.hpp>
#include <libtaomee++/memory/mempool.hpp>
#include <string.h>

class base_duration_timer
{
public:
		base_duration_timer()
		{
			memset(&m_begin_time, 0, sizeof(m_begin_time)) ;
			m_duration_time = 0;
		}
		virtual ~base_duration_timer(){}
public:
		virtual void init_base_duration_timer(struct timeval begin_time, uint32_t duration_time)
		{
			m_begin_time = begin_time;
			m_duration_time = duration_time;
		}
public:
		virtual void reset_timer(struct timeval begin_time)
		{
			m_begin_time = begin_time;
		}
		virtual bool is_timer_finish(struct timeval cur_time)
		{
			return taomee::timediff2(cur_time, m_begin_time) >  (int)m_duration_time;
		}
		virtual void set_duration_time( uint32_t duration_time)
		{
			m_duration_time = duration_time;
		}
protected:
		struct timeval  m_begin_time;         //记录计时器的开始时间戳
		uint32_t    	m_duration_time;      //记录计时器的持续时间单位豪秒
};

class base_switch_timer: public base_duration_timer
{
public:
	base_switch_timer()
	{
		flag = 0;	
	}
	~base_switch_timer(){}
public:
	void init_base_duration_timer(struct timeval begin_time, uint32_t duration_time)
	{
		base_duration_timer::init_base_duration_timer(begin_time, duration_time);
		flag = 1;	
	}
	void reset_timer(struct timeval begin_time)
	{
		base_duration_timer::reset_timer(begin_time);
		flag = 1;
	}
	bool is_timer_finish(struct timeval cur_time)
	{
		bool ret = base_duration_timer::is_timer_finish(cur_time);
		flag = ret ? 0:1;
		return ret;
	}
	bool check_timer_flag()
	{
		return flag;	
	}
	void set_timer_flag(uint32_t flg)
	{
		flag = flg;	
	}
protected:
	uint32_t flag;
};

class base_trigger_timer
{
public:
		base_trigger_timer()
		{
			memset(&m_begin_time, 0, sizeof(m_begin_time));
			m_interval_time = 0;
		}
		virtual ~base_trigger_timer(){}
public:
		void init_base_trigger_timer(struct timeval begin_time, uint32_t interval_time)
		{
			m_begin_time = begin_time;
			m_interval_time = interval_time;
		}
public:
		virtual bool to_next_time(struct timeval cur_time)
		{
			if( taomee::timediff2(cur_time, m_begin_time) >(int)m_interval_time)
			{
				m_begin_time = cur_time;
				return true;
			}
			return false;
		}

		virtual bool to_next_time(struct timeval cur_time, uint32_t& millisecond)
		{
			millisecond = taomee::timediff2(cur_time, m_begin_time);
			if( millisecond > m_interval_time)
			{
				m_begin_time = cur_time;
				return true;
			}
			return false;
		}

		virtual void reset_timer(struct timeval cur_time)
		{
			m_begin_time = cur_time;
		}
private:
		struct timeval  m_begin_time;        //计时器开始的时间戳
		uint32_t    m_interval_time;         //多长时间触发一次，单位毫秒
};





#endif
