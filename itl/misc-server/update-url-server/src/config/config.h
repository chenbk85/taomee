/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file config.ch
 * @author richard <richard@taomee.com>
 * @date 2010-03-10
 */

#include "../i_config.h"
#include "ini.h"

struct c_config : public i_config
{
public:
	virtual int init(const char (*config_file_list)[PATH_MAX], int config_file_count);
	virtual int get_config(const char *section, const char *name, char *buffer, int buffer_count) const;
	virtual int set_config(const char *section, const char *name, const char *value);
	virtual int uninit();
	virtual int release();

private:
	int m_inited;
	c_ini m_ini;
};

