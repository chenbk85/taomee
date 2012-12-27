/*
 * =====================================================================================
 *
 *       Filename:  bench_api.h
 *
 *    Description:  bench_api.h
 *
 *        Version:  1.0
 *        Created:  2011年08月10日 15时10分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_BENCH_API_H_20110810
#define H_BENCH_API_H_20110810

extern "C"
{
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
}

#include <benchapi.h>


extern "C" int handle_init(int , char **, int);
extern "C" int handle_input(const char *, int , const skinfo_t *);
extern "C" int handle_filter_key (const char*, int, uint32_t*);
extern "C" int handle_process(char *, int , char **, int *, const skinfo_t *);
extern "C" int handle_open(char **, int *, const skinfo_t *);
extern "C" int handle_close(const skinfo_t *);
extern "C" int handle_timer(int *intvl);
extern "C" void handle_fini(int);


#endif //H_BENCH_API_H_20110810
