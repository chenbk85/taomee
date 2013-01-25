/*
 *============================================================
 *  file      id_counter.hpp
 *  brief     保存某个ID出现的次数，只有主登录在使用
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_MAIN_LOGIN_ID_COUNTER_HPP_
#define LIBTAOMEEPP_MAIN_LOGIN_ID_COUNTER_HPP_

#include <cstring>
#include <map>
#include <vector>

#include <libtaomee++/thread/mutex.hpp>
#include <libtaomee++/thread/thread.hpp>
#include <libtaomee++/thread/threadcondition.hpp>
#include <libtaomee++/verification_image/verification_image.hpp>

extern "C" {
#include <stdint.h>

#include <libtaomee/timer.h>
#include <libtaomee/interprocess/shmq.h>
#include <libtaomee/project/types.h>
}

#pragma pack(1)

/**
 * @brief 存储ID统计的次数和第一次统计的时间值
 */
struct id_info_t {
	/**
	* @brief 构造函数，初始化id_cnt为零和first_time为当前时间
	*/
	id_info_t()
		{ id_cnt = 0; first_time = get_now_tv()->tv_sec; }

	/*! 存储ID地址的次数 */
	uint32_t	id_cnt;
	/*! 存储IP第一次统计的时间 */
	time_t		first_time;
};

/**
  * @brief Hold a verification code and the related verification image returned from VigWorker::get_image()
  */
struct ImagePackage {
	/*! verification code, null terminated */
	char		verif_code[6];
	/*! id of verification code */
	char		verif_id[16];
	/*! size in bytes of the verification image */
	uint32_t	img_size;
	/*! verification image */
	char		img[];
};

#pragma pack()

/**
  * @brief VerificationImageGenerator worker
  */
class VigWorker : public taomee::Thread {
private:
	friend int purge_id_cnt(void* owner, void* data);

private:
	struct VerifInfo {
		VerifInfo(uint32_t t = 0, int num = 0, const char* code = "")
		{
			gen_tm    = t;
			magic_num = num;
			strncpy(verif_code, code, sizeof(verif_code));
		}

		uint32_t	gen_tm;
		uint32_t	magic_num;
		char		verif_code[6];
	};
	// holds verification infomation
	typedef std::map<uint32_t, VerifInfo> VerifInfoMap;

public:
	VigWorker(int flag, const char* path);

	/**
	  * @brief Get a verification image. !!- Not thread-safe -!!
	  * @return pointer to a verification image on success, 0 otherwise
	  */
	const ImagePackage* get_image();

public:
	/**
	  * @brief verifys a verification code. !!- Not thread-safe -!!
	  * @param code_id id to locate a verification code
	  * @param code verification code to verify
	  * @return true on valid code, false otherwise
	  */
	static bool verify_code(const void* code_id, const char* code);

private:
	void run();

private:
	static void purge_verifinfo();

private:
	static const unsigned int sc_bufsize = 8 * 1024;
	static const unsigned int sc_img_max_size = sc_bufsize - 500;
	static const unsigned int sc_idx_min_val  = 10000000;
	static const unsigned int sc_idx_max_val  = 4000000000U;
	static const unsigned int sc_cache_max    = 1000;

private:
	const int m_flag; // generate easy image (0) or hard image (1)

private:
	shm_cq_t* m_shmq;
	taomee::VerificationImageGenerator m_vig;
	taomee::Mutex m_mtx;
	taomee::ThreadCond m_cond;
	char m_buf[sc_bufsize];
    std::vector<ImagePackage*> m_image_cached;

private:
	static uint32_t sm_verifinfo_idx;
    static uint32_t sm_cache_used_cnt;
	static VerifInfoMap sm_verifinfo;

private:
	static const char sc_key[];
};


//---------------------------------------------

void id_counter_init();
void id_counter_erase(userid_t id);
void id_counter_add(userid_t id);
const id_info_t* id_counter_info(userid_t id);

void ip_counter_erase(uint32_t id);
void ip_counter_add(uint32_t id);
const id_info_t* ip_counter_info(uint32_t id);

//---------------------------------------------
extern VigWorker g_vig_worker;
extern VigWorker g_vig_hard_worker;

#endif // LIBTAOMEEPP_MAIN_LOGIN_ID_COUNTER_HPP_
