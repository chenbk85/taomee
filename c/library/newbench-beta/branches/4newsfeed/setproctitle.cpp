/**
 * =====================================================================================
 *       @file  setproctitle.cpp
 *      @brief  设置ps中显示的进程标题，只能在linux下使用
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/17/2010 02:42:13 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  jasonwang (王国栋), jasonwang@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

/* proctitle code - we know this to work only on linux... */

/*
**  SETPROCTITLE -- set process title for ps (from sendmail)
**
**      Parameters:
**              fmt -- a printf style format string.
**
**      Returns:
**              none.
**
**      Side Effects:
**              Clobbers argv of our main procedure so ps(1) will
**              display the title.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "setproctitle.h"

#ifndef SPT_BUFSIZE
#define SPT_BUFSIZE     2048
#endif

extern char** environ;

static char** argv0;
static int argv_lth;

void initproctitle(int argc, char **argv)
{
	int i;
	char **envp = environ;

	/*
	 * Move the environment so we can reuse the memory.
	 * (Code borrowed from sendmail.)
	 * WARNING: ugly assumptions on memory layout here;
	 *          if this ever causes problems, #undef DO_PS_FIDDLING
	 */
	for (i = 0; envp[i] != NULL; i++)
	{
	    continue;
	}

	environ = (char **) malloc(sizeof(char *) * (i + 1));

	if (environ == NULL)
	{
        return;
	}

	for (i = 0; envp[i] != NULL; i++)
	{
		if ((environ[i] = strdup(envp[i])) == NULL)
		{
			return;
		}
	}
	environ[i] = NULL;

	argv0 = argv;
	if (i > 0)
	{
		argv_lth = envp[i-1] + strlen(envp[i-1]) - argv0[0];
	}
	else
	{
		argv_lth = argv0[argc-1] + strlen(argv0[argc-1]) - argv0[0];
	}
}

void setproctitle(const char *fmt,...)
{
	int i;
	char buf[SPT_BUFSIZE];
	va_list ap;

	if (!argv0)
	{
		return;
	}

	va_start(ap, fmt);
	(void) vsnprintf(buf, SPT_BUFSIZE, fmt, ap);
	va_end(ap);

	i = strlen (buf);
	if (i > argv_lth - 2)
	{
		i = argv_lth - 2;
		buf[i] = '\0';
	}

	memset(argv0[0], '\0', argv_lth);       /* clear the memory area */
	(void) strcpy (argv0[0], buf);

	argv0[1] = NULL;
}
