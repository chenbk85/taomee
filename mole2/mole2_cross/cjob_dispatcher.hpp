#ifndef __C_JOB_DISPATCHER_H__
#define __C_JOB_DISPATCHER_H__
#include "ijob_dispatcher.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)
#include "cross_data_type.hpp"
class CJobDispatcher:public IJobDispatcher
{
public:
	CJobDispatcher();
	~CJobDispatcher();
public:
BEGIN_JOB_DISPATCHER_MAP()
	// normal job process by protocol
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_REPORT_ONLINE_INFO, report_online_info);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_ONLINE_KEEPALIVE, online_keepalive);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_CHANGEE_OTHER_INLINE, challenge_battle);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_APPLY_PK, onli_apply_pk);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_CANCEL_APPLY_PK, onli_cancel_pk_apply);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_AUTO_ACCEPT, onli_auto_accept);
	REGISTER_JOB_DISPATCH_HANDLE(CROSS_COMMID_AUTO_FIGHT, onli_auto_fight);
	// 


	// timeout process
	REGISTER_JOB_TIMOUT_HANDLE(CROSS_COMMID_ONLINE_TIMEOUT, make_online_dead);
	REGISTER_JOB_TIMOUT_HANDLE(CROSS_COMMID_ONLINE_PK_TRYAGAIN, try_match_again);
	REGISTER_JOB_EVENT_HANDLE(CROSS_COMMID_CLEAN_ONLINE_INFO, clear_online_info);
END_JOB_DISPATCHER_MAP()
	
public:
	/*!
	  * @Function:Release
	  * @Description: release all source that hold by object
	  * @Param:none
	  * @return:void
	  */
	virtual void Release();

	/*!
	  * @Function:GetObjId
	  * @Description: get object's id
	  * @Param:none
	  * @return:const char*
	  */
	virtual const char* GetObjId() const;

private:
	int report_online_info(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	int online_keepalive(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	int challenge_battle(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	
	int onli_apply_pk(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	int onli_auto_accept(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	int onli_auto_fight(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);
	int onli_cancel_pk_apply(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

// timer
private:
	int make_online_dead(void* owner, void* data);
	int try_match_again(void* owner, void* data);

// event
private:
	int clear_online_info(void* lparam, void* wparam);
	
private:
	int send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen);
	const online_info_t* get_online_info(uint16_t online_id) const;
	pkwait_info_t* try_to_match(pkwait_info_t* ppi, uint32_t& match);
	pkwait_info_t* get_pkwait_info(uint32_t uid);
	pkwait_info_t* match_again_team(pkwait_info_t* ppi);
	pkwait_info_t* match_again_single(pkwait_info_t* ppi);
	// m_usrid_2_onlineid
	uint16_t get_onlineid_by_usrid(userid_t uid);
	void  reset_onlineid_by_usrid(userid_t uid, uint16_t onlineid);
	
private:
	std::map<int, uint16_t> m_fd_map_online;
	uint16_t                m_max_online_id;
	online_info_t		    m_online_info[online_num_max];
	keepalive_timer_t		m_keepalive_tmr[online_num_max];
	uint8_t                 m_pkg[pkg_size];
	std::set<userid_t>      m_pkwait;
	std::map<userid_t, pkwait_info_t> m_start;
	std::map<userid_t, pkaccept_info_t> m_accept;
	pkwait_info_t m_single_pk_[pklv_phase_max];
	pkwait_info_t m_team_pk_[pk_team_cnt_max][pklv_phase_max];
	std::map<userid_t, uint16_t> m_usrid_2_onlineid;
};

END_NAME_SPACE()
#endif