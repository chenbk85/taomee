#ifndef SERVERBENCH_BENCHAPI_H_
#define SERVERBENCH_BENCHAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/cdefs.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>

#undef __attr_cdecl__
#define __attr_cdecl__ __attribute__((__cdecl__))

/*
#define rdtscll(val) do { \
			unsigned long __a,__d; \
			long long __b; \
			asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
			__b = __d; \
			(val) = (__a) | (__b<<32); \
		} while (0)
*/

#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif

#undef __attr_pure__
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __attr_pure__   /* */
#else
#define __attr_pure__   __attribute__((__pure__))
#endif

#undef __attr_nonnull__
#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __attr_nonnull__(x)     /* */
#else
#define __attr_nonnull__(x) __attribute__((__nonnull__(x)))
#endif

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

#define __init          __attribute__ ((__section__ (".init.text")))
#define __initdata      __attribute__ ((__section__ (".init.data")))
#define __exitdata      __attribute__ ((__section__(".exit.data")))
#define __exit_call     __attribute_used__ __attribute__ ((__section__ (".exitcall.exit")))

#ifdef MODULE
#define __exit          __attribute__ ((__section__(".exit.text")))
#else
#define __exit          __attribute_used__ __attribute__ ((__section__(".exit.text")))
#endif

enum {
	PROC_MAIN = 0,
	PROC_CONN,
	PROC_WORK,
};
/**
 * skinfo_t - socket infomation
 * @fd - socket description
 * @type - sock type (%SOCK_STREAM, %SOCK_DGRAM, etc)
 * @local - connection local address
 * @remote - connection peer address
 */
typedef struct skinfo_struct {
	int sockfd;
	int type;
//	long long recvtm;
//	long long sendtm;
	time_t recvtm;
	time_t sendtm;

	u_int    local_ip;
	u_short  local_port;
	u_int    remote_ip;
	u_short  remote_port;
} skinfo_t;

/*
 *  struct app_config - application program interface
 */
typedef struct dll_func_struct {
	void *handle;
	int (*handle_init) (int, char **, int);
	int (*handle_input) (const char*, int, const skinfo_t*);
	int (*handle_process) (char *, int , char **, int *, const skinfo_t*);
	int (*handle_open) (char **, int *, const skinfo_t*);
	int (*handle_close) (const skinfo_t*);
	int (*handle_timer) (int *);
	void (*handle_fini) (int);
} dll_func_t;

extern dll_func_t dll;

/*
 * shmq api
 */
//push a packet into the receiving queue (recvq)
int push_a_pkg_to_worker(const void* pkg, u_int pkglen);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // SERVERBENCH_BENCHAPI_H_
