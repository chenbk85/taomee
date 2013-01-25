#include <algorithm>
#include <stdexcept>

extern "C" {
#include <strings.h>

#include <libtaomee/list.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}

#include "id_counter.hpp"

using namespace std;
using namespace taomee;

struct DailyTimer {
    list_head_t timer_list;
};

static DailyTimer daily_tmr;

typedef map<userid_t, id_info_t> UidMap;
/*! 记录每个id出现的次数，每天零点清空一次 */
static UidMap idcnt;
/*! ip counter */
static UidMap ipcnt;

VigWorker g_vig_worker(1, "login_vig_worker_png_easy_shm_cq");
VigWorker g_vig_hard_worker(0, "login_vig_worker_png_hard_shm_cq");

//----------------------------------

uint32_t VigWorker::sm_verifinfo_idx = sc_idx_min_val;
uint32_t VigWorker::sm_cache_used_cnt = 0;
VigWorker::VerifInfoMap VigWorker::sm_verifinfo;

const char VigWorker::sc_key[] = "<!Tv?3^/";

VigWorker::
VigWorker(int flag, const char* path)
	: Thread(true), m_flag(flag), m_vig("./fonts")
{
	// create a shm cq for holding verification images
	m_shmq = shm_cq_create(path, 1024 * 1024 * 100, sc_bufsize);

	if (m_shmq == 0) {
		m_shmq = shm_cq_attach(path);
	}

	if (m_shmq == 0) {
		throw runtime_error(string("VigWorker(): Failed to create shm cq!") + path);
	}

	if (!m_mtx || !m_cond) {
		throw runtime_error("VigWorker(): Failed to create mutex lock!");
	}
}

void VigWorker::
run()
{
	for ( ; ; ) {
		VerificationImage img = m_vig.generate(4, m_flag);
		const string& code  = img.get_verif_code();
		const string& image = img.get_image();
		if ((code.size() <= 5) && (image.size() < sc_img_max_size)) {
			ImagePackage* imgpkg = reinterpret_cast<ImagePackage*>(m_buf);
			imgpkg->img_size = image.size();
			strcpy(imgpkg->verif_code, code.c_str());
			copy(image.begin(), image.end(), imgpkg->img);
			if (shm_cq_push(m_shmq, imgpkg, imgpkg->img_size + sizeof(ImagePackage)) == -1) {
				m_mtx.lock();
				m_cond.timedwait(m_mtx, 30);
				m_mtx.unlock();
			}
		}
	}
}

void VigWorker::
purge_verifinfo()
{
	DEBUG_LOG("TOTAL VERIFINFO TODAY\t[%lu %lu]", sm_verifinfo.size(), sm_verifinfo_idx);

	sm_verifinfo.clear();
	sm_verifinfo_idx = sc_idx_min_val;
}

const ImagePackage* VigWorker::
get_image()
{
	ImagePackage* img = NULL;
	uint32_t pkglen = shm_cq_pop(m_shmq, reinterpret_cast<void**>(&img));
	if (pkglen) {
        //cache some code
        if(m_image_cached.size() < sc_cache_max) {
            char* data = new char[sizeof(ImagePackage) + img->img_size];
            memcpy(data, img, (sizeof(ImagePackage) + img->img_size));
            m_image_cached.push_back((ImagePackage*)data);
            DEBUG_LOG("BUILD CACHE CODE\t[%lu]", m_image_cached.size());
        }
        //rebuild cache code
        if(sm_cache_used_cnt >= (sc_cache_max * 2)) {
            for(size_t i = 0; i < m_image_cached.size(); i++) {
                delete m_image_cached[i];
            }
            DEBUG_LOG("REBUILD CACHE CODE\t[%u %lu]", sm_cache_used_cnt, m_image_cached.size());
            m_image_cached.clear();
            char* data = new char[sizeof(ImagePackage) + img->img_size];
            memcpy(data, img, (sizeof(ImagePackage) + img->img_size));
            m_image_cached.push_back((ImagePackage*)data);
            sm_cache_used_cnt = 0;
        }
	} else {
        if(m_image_cached.size() == 0) {
            return 0;
        }
        sm_cache_used_cnt++;
        DEBUG_LOG("USE CACHE CODE\t[%lu %u]", m_image_cached.size(), sm_cache_used_cnt);
        img = m_image_cached[rand()%m_image_cached.size()];
        m_cond.signal();
    }
    uint32_t buf[4];
    buf[0] = get_now_tv()->tv_sec;
    buf[2] = sm_verifinfo_idx;
    buf[3] = rand();
    des_encrypt_n(sc_key, buf, img->verif_id, 2);
    // record a verifinfo
    sm_verifinfo[sm_verifinfo_idx] = VerifInfo(buf[0], buf[3], img->verif_code);
    // erase some verifinfo
    if (sm_verifinfo.size() > 5000000) {
        VerifInfoMap::iterator low_it, up_it;
        VerifInfoMap::iterator it = sm_verifinfo.find(sm_verifinfo_idx);			
        if (++it == sm_verifinfo.end()) {
            low_it = sm_verifinfo.begin();
        } else {
            low_it = it;
        }
        up_it = sm_verifinfo.upper_bound(low_it->first + 200000);
        sm_verifinfo.erase(low_it, up_it);

        DEBUG_LOG("ERASE CACHE VERIFINFO\t[%lu]", sm_verifinfo.size());
    }

    if (++sm_verifinfo_idx == sc_idx_max_val) {
        sm_verifinfo_idx = sc_idx_min_val;
    }

	return img;
}

bool VigWorker::
verify_code(const void* code_id, const char* code)
{
	uint32_t buf[4];
	des_decrypt_n(sc_key, code_id, buf, 2);

	VerifInfoMap::iterator it = sm_verifinfo.find(buf[2]);
	if ((it != sm_verifinfo.end()) && (it->second.gen_tm == buf[0]) && (it->second.magic_num == buf[3])) {
		bool ret = !strcasecmp(code, it->second.verif_code);
		if (ret == false) {
			DEBUG_LOG("VERIFICATION CODE MISMATCH\t[sys=%s usr=%s]", it->second.verif_code, code);
		}
		sm_verifinfo.erase(it);
		return ret;
    }

	return false;
}

//----------------------------------------------------------

/**
 * @brief 清空idcnt。 
 *
 * @return 总是0。
 */
int purge_id_cnt(void* owner, void* data)
{
	DEBUG_LOG("TOTAL DIFF ID/IP NUM TODAY\t[%lu %lu]", idcnt.size(), ipcnt.size());

	idcnt.clear();
	ipcnt.clear();
	VigWorker::purge_verifinfo();

	ADD_TIMER_EVENT(&daily_tmr, purge_id_cnt, 0, get_now_tv()->tv_sec + 86400);
	return 0;
}

void id_counter_init()
{
	if (!g_vig_worker || !g_vig_hard_worker) {
		throw std::runtime_error("Failed to create VerificationImageGenerator Worker!");
	}
	g_vig_worker.start();
	g_vig_hard_worker.start();

	INIT_LIST_HEAD(&daily_tmr.timer_list);

	tm tm_tmp      = *get_now_tm();
	tm_tmp.tm_hour = 0;
	tm_tmp.tm_min  = 0;
	tm_tmp.tm_sec  = 0;
	time_t exptm   = mktime(&tm_tmp) + 86400;
	ADD_TIMER_EVENT(&daily_tmr, purge_id_cnt, 0, exptm);
}

void id_counter_erase(userid_t id)
{
	idcnt.erase(id);
}

void id_counter_add(userid_t id)
{
	idcnt[id].id_cnt++;
}

const id_info_t* id_counter_info(userid_t id)
{
	UidMap::iterator iter = idcnt.find(id);
	if (iter != idcnt.end()) {
		return &(iter->second);
	}

	return 0;
}

void ip_counter_erase(uint32_t ip)
{
	ipcnt.erase(ip);
}

void ip_counter_add(uint32_t ip)
{
	ipcnt[ip].id_cnt++;
}

const id_info_t* ip_counter_info(uint32_t ip)
{
	UidMap::iterator iter = ipcnt.find(ip);
	if (iter != ipcnt.end()) {
		return &(iter->second);
	}

	return 0;
}

