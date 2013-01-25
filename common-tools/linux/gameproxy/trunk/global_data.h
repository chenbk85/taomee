
#ifndef _GLOBAL_DATE_
#define _GLOBAL_DATE_
#include "clientsmgr.h"

extern  void*		g_pClientsMgr;
extern	uint32_t	g_serialNum;
#define pClientsMgr ((ClientsMgr*)g_pClientsMgr)
#endif