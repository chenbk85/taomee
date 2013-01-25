#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include <time.h>

#if 0 /* NEED add libevent `src-dir' to include  */
#include "defer-internal.h"
#include "evdns.h"
#include "event.h"
#include "evhttp.h"
#include "evsignal-internal.h"
#include "evthread-internal.h"
#include "evutil.h"
#include "ht-internal.h"
#include "http-internal.h"
#include "iocp-internal.h"
#include "ipv6-internal.h"
#include "log-internal.h"
#include "minheap-internal.h"
#include "mm-internal.h"
#include "ratelim-internal.h"
#include "strlcpy-internal.h"
#include "util-internal.h"
#include "bufferevent-internal.h"
#include "event-internal.h"
#include "changelist-internal.h"
#include "evbuffer-internal.h"
#include "evmap-internal.h"

void print_basic_struct_sizes(void);
#endif


void print_libevent_version(void);
void print_supported_backend(void);
void print_sys_info(void);
void print_event_base_info(struct event_base *base);
