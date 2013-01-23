#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include "config.h"
#include "benchapi.h"
#include "list.h"
#include "util.h"

#define INIKEY_HASHBITS	8
static const char myifs[256] = { [' ']=1, ['\t']=1, ['\r']=1, ['\n']=1, ['=']=1 };
static list_head_t ini_key_head[1 << INIKEY_HASHBITS];
static int has_init = 0;

struct config_pair {
	struct list_head list;
	char *val;
	char key[0];
};

static struct list_head *config_key_hash (const char* key)
{
	u_long r5hash(const char *p) {
		unsigned long h = 0;
		while(*p) {
			h = h * 11 + (*p << 4) + (*p >> 4);
			p++;
		}
		return h;
	} 

	return &ini_key_head[r5hash (key) & ((1 << INIKEY_HASHBITS) - 1)];
}

static int str2int (const char *strval, int def)
{
	int ret_code = def;
	if (isdigit (strval[0]) || (strval[0] == '-' && isdigit(strval[1])))
		return atoi (strval);

	if (!strcasecmp (strval, "On"))
		ret_code = 1;
	else if (!strcasecmp (strval, "Off"))
		ret_code = 0;
	else if (!strcasecmp (strval, "Yes"))
		ret_code = 1;
	else if (!strcasecmp (strval, "No"))
		ret_code = 0;
	else if (!strcasecmp (strval, "True"))
		ret_code = 1;
	else if (!strcasecmp (strval, "False"))
		ret_code = 0;
	else if (!strcasecmp (strval, "Enable"))
		ret_code = 1;
	else if (!strcasecmp (strval, "Disable"))
		ret_code = 0;
	else if (!strcasecmp (strval, "Enabled"))
		ret_code = 1;
	else if (!strcasecmp (strval, "Disabled"))
		ret_code = 0;

	return ret_code;
}

static int config_put_value (const char* key, const char* val)
{
	struct config_pair *mc;
	int len = strlen (key) + 1;

	mc = (struct config_pair*) malloc (sizeof (struct config_pair) + len);
	if (mc == NULL) {
//		EMERG_LOG ("malloc failed, length=%d, %m");
		return -1;
	}
	memcpy (mc->key, key, len);

	mc->val = strdup (val);
	if (mc->val == NULL) {
		free (mc);
		EMERG_LOG ("strdup \"%s\" error, %m", val);
		return -1;
	}

	list_add (&mc->list, config_key_hash (key));

	return 0;
}

static int parse_config (char* buffer)
{
	char *field[2];
	char* start = buffer;
	int len = strlen (buffer);

	while (buffer + len > start) {
		char* end = strchr (start, '\n');
		if (*end) *end = '\0';
			
		
		if (*start != '#' && str_explode (myifs, start, field, 2) == 2) 
			config_put_value (field[0], field[1]);

		start = end + 1;
	}

	return 0;
}

char* config_get_strval (const char* key)
{
	list_head_t *hlist;
	struct config_pair *mc;

	hlist = config_key_hash (key);
	list_for_each_entry(mc, hlist, list) {
//		mc = list_entry(p, struct config_pair, list);
		if(!strcmp(key, mc->key))
			return mc->val;
	}

	return NULL;
}

int config_get_intval (const char* key, int def)
{
	char* val = config_get_strval (key);
	if (val == NULL)
		return def;

	return str2int (val, def);
}

int mmap_config_file (const char* file_name, char **buf)
{
	int fd, len, ret_code = -1;

	fd = open (file_name, O_RDONLY);
	if (fd < 0) {
		EMERG_LOG ("open %s error, cwd=%s, %m\n", file_name, get_current_dir_name ());
		return -1;
	}
	len = lseek (fd, 0L, SEEK_END);
	lseek (fd, 0L, SEEK_SET);

	*buf = mmap (0, len + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (*buf == MAP_FAILED) {
		EMERG_LOG ("mmap failed, length=%d, %m", len + 1);
	} else {
		read (fd, *buf, len);
		(*buf) [len] = 0;
		ret_code = len + 1;
	}

	close(fd);
	return ret_code;
}

int config_init (const char *file_name) 
{
	int len, i, ret_code = -1;
	char *buf;

	if (!has_init) {
		for (i = 0; i < (1<<INIKEY_HASHBITS); i++) 
			INIT_LIST_HEAD (&ini_key_head[i]);
	}	

	if ((len = mmap_config_file (file_name, &buf)) > 0) {
		ret_code = parse_config (buf);
		munmap (buf, len);
	}

	has_init = 1;

	BOOT_LOG (ret_code, "load config file:%s", file_name);
}

void config_dump() 
{
#ifdef DEBUG
	list_head_t *p;
	struct config_pair *mc;
	int i;

	TRACE_LOG ("config file dump:");
	for (i = 0; i < (1<<INIKEY_HASHBITS); i++) {
		list_for_each (p, &ini_key_head[i]) {
			mc = list_entry (p, struct config_pair, list);
			TRACE_LOG ("%16s = %s", mc->key, mc->val);
		}
	}
#endif
}

void config_exit ()
{
	list_head_t *p;
	struct config_pair *mc;
	int i;

	for (i = 0; i < (1<<INIKEY_HASHBITS); i++) {
		p = ini_key_head[i].next;
		while (p != &ini_key_head[i]) {
			mc = list_entry(p, struct config_pair, list);
			p = p->next;

			if(mc->val) 
				free(mc->val);
			free(mc);
		}
	}

	TRACE_LOG ("free config file data structure");
}
