#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "util.h"
#include <libtaomee/log.h>

int pipe_handles[4];
static const char default_ifs[256] = {[9] = 1,[10] = 1,[13] = 1,[32] = 1 };

void inline
build_ifs (char *tifs, const u_char * ifs0)
{
	const u_char *ifs = ifs0;
	memset (tifs, 0, 256);
	while (*ifs)
		tifs[*ifs++] = 1;
}

/*
 * NULL IFS: default blanks
 * first byte is NULL, IFS table
 * first byte is NOT NULL, IFS string
 */
int
str_explode (const char *ifs, char *line0, char *field[], int n)
{
	char *line = line0;
	int i;

	if (ifs == NULL)
	{
		ifs = default_ifs;
	}
	else if (*ifs)
	{
		char *implicit_ifs = alloca (256);
		build_ifs (implicit_ifs, (const u_char *) ifs);
		ifs = implicit_ifs;
	}

	i = 0;
	while (1)
	{
		while (ifs[(u_char) * line])
			line++;
		if (!*line)
			break;
		field[i++] = line;
		if (i >= n)
		{
			line += strlen (line) - 1;
			while (ifs[(u_char) * line])
				line--;
			line[1] = '\0';
			break;
		}
		while (*line && !ifs[(u_char) * line])
			line++;
		if (!*line)
			break;
		*line++ = '\0';
	}

	return i;
}


int
pipe_create ()
{
	int ret_code = -1;
	if (pipe (pipe_handles) == -1)
		goto create_pipe_exit;

	if (pipe (&pipe_handles[2]) == -1)
		goto create_pipe_exit;

	fcntl (pipe_handles[0], F_SETFL, O_NONBLOCK | O_RDONLY);
	fcntl (pipe_handles[1], F_SETFL, O_NONBLOCK | O_WRONLY);

	fcntl (pipe_handles[0], F_SETFD, FD_CLOEXEC);
	fcntl (pipe_handles[1], F_SETFD, FD_CLOEXEC);

	fcntl (pipe_handles[2], F_SETFL, O_NONBLOCK | O_RDONLY);
	fcntl (pipe_handles[3], F_SETFL, O_NONBLOCK | O_WRONLY);

	fcntl (pipe_handles[2], F_SETFD, FD_CLOEXEC);
	fcntl (pipe_handles[3], F_SETFD, FD_CLOEXEC);

	return 0;
create_pipe_exit:
	BOOT_LOG (ret_code, "Create notify pipe, fd=%d,%d", pipe_handles[0],
			pipe_handles[1]);
}

static void
hex2char (unsigned char what, char *dest)
{
	char sHex[20] = "0123456789ABCDEF";
	dest[0] = sHex[what / 16];
	dest[1] = sHex[what % 16];
}

void
hex2str (char *what, int len, char *dest)
{
	int i;
	for (i = 0; i < len; i++)
		hex2char (what[i], dest + i * 2);
}

	static char
char2hex (char first, char second)
{
	register char digit;

	digit = (first >= 'A' ? (first - 'A') + 10 : (first - '0'));
	digit *= 16;
	digit += (second >= 'A' ? (second - 'A') + 10 : (second - '0'));
	return (digit);
}

void
str2hex (const char *what, unsigned char *dest)
{
	int i, length = strlen (what);
	for (i = 0; i < length; i += 2)
		dest[i / 2] = char2hex (what[i], what[i + 1]);
}
