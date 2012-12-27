#include "fwd_decl.hpp"

/* hold common timers */
timer_head_t g_events;

/* pathname where to store the statistic log info */
char* statistic_logfile;
/*! event manager */
taomee::EventMgr ev_mgr;

