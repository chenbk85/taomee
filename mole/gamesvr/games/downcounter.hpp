#ifndef ANT_DOWNCOUNTER_HPP_
#define ANT_DOWNCOUNTER_HPP_

// headers inherited from ISO C
#include <cassert>
#include <ctime>
// headers from my lib
#include <ant/inet/pdumanip.hpp>

extern "C" {
#include <gameserv/game.h>
#include <gameserv/proto.h>
#include <gameserv/timer.h>
}

class DownCounterNotifier0 {
public:
	void operator() (void* owner, uint8_t nsecs)
	{
		int i = sizeof(protocol_t);
		ant::pack(dcpkg, nsecs, i);
		init_proto_head(dcpkg, proto_client_ready, sizeof dcpkg);
		send_to_players(reinterpret_cast<game_group_t*>(owner), dcpkg, sizeof dcpkg);
	}
private:
	uint8_t dcpkg[sizeof(protocol_t) + sizeof(uint8_t)];
};

class DownCounterNotifier1 {
public:
	void operator() (void* owner, uint8_t nsecs)
	{
		int i = sizeof(protocol_t);
		ant::pack(dcpkg, (uint32_t)get_now_tv()->tv_sec, i);
		ant::pack(dcpkg, nsecs, i);
		init_proto_head(dcpkg, proto_client_ready, sizeof dcpkg);
		send_to_players(reinterpret_cast<game_group_t*>(owner), dcpkg, sizeof dcpkg);
	}
private:
	uint8_t dcpkg[sizeof(protocol_t) + sizeof(uint32_t) + sizeof(uint8_t)];
};

template <typename NotiPkgPolicy, typename OwnerType = game_group_t>
class DownCounter {
public:
	DownCounter(uint8_t nsecs, OwnerType* owner, timer_cb_func_t func, void* data);

	void start();
private:
	static int DownCounterCallBack(void* owner, void* data);
	// for downcounting
	uint8_t nsecs_;
	// for sending pkg at each downcount
	NotiPkgPolicy send_noti_pkg;
	// for callback
	OwnerType* owner_;
	void*      data_;
	timer_cb_func_t callback_;
};

typedef DownCounter<DownCounterNotifier0> DownCounter0;
typedef DownCounter<DownCounterNotifier1> DownCounter1;

template <typename NotiPkgPolicy, typename OwnerType>
inline DownCounter<NotiPkgPolicy, OwnerType>::
DownCounter(uint8_t nsecs, OwnerType* owner, timer_cb_func_t func, void* data)
{
	assert((nsecs > 0) && owner && func);

	nsecs_    = nsecs;
	owner_    = owner;
	data_     = data;
	callback_ = func;
}

template <typename NotiPkgPolicy, typename OwnerType>
inline int DownCounter<NotiPkgPolicy, OwnerType>::
DownCounterCallBack(void* owner, void* data)
{
	DownCounter<NotiPkgPolicy, OwnerType>* downcounter
		= reinterpret_cast<DownCounter<NotiPkgPolicy, OwnerType>*>(data);

	if (downcounter->nsecs_) {
		downcounter->send_noti_pkg(downcounter->owner_, downcounter->nsecs_--);
	} else {
		downcounter->callback_(downcounter->owner_, downcounter->data_);
	}
	return 0;
}

template <typename NotiPkgPolicy, typename OwnerType>
inline void DownCounter<NotiPkgPolicy, OwnerType>::
start()
{
	time_t curtm = get_now_tv()->tv_sec;
	for (uint8_t i = 0; i != nsecs_; ++i) {
		ADD_TIMER_EVENT(owner_, DownCounterCallBack, this, curtm + i + 1);
	}
	DownCounterCallBack(0, this);
}

#endif // ANT_DOWNCOUNTER_HPP_
