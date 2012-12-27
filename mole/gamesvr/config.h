#ifndef _CONFIG_H_
#define _CONFIG_H_
 extern int mmap_config_file (const char *file_name, char **buf);

extern int config_get_intval (const char *key, int def);
extern char *config_get_strval (const char *key);
extern int config_init (const char *file_name);
extern void config_exit ();
extern void config_dump ();

#endif	/* _CONFIG_H_ */

