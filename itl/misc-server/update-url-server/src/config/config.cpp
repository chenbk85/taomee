/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file config.c
 * @author richard <richard@taomee.com>
 * @date 2010-03-10
 */

#include <iostream>
#include <string.h>
#include "config.h"

using namespace std;

/**
 * @brief 创建i_config的实例
 */
int create_config_instance(i_config **pp_instance)
{
    if (pp_instance == NULL) {
        return -1;
    }

    c_config* p_instance = new(std::nothrow)c_config();
    if (p_instance == NULL) {
        return -1;
    } else {
        *pp_instance = dynamic_cast<i_config *>(p_instance);
        return 0;
    }
}

int c_config::init(const char (*config_file_list)[PATH_MAX], int config_file_count)
{
	for (int i = 0; i != config_file_count; ++i) {
		if (!m_ini.add_file(config_file_list[i])) {
			cerr << "m_ini.add_file " << config_file_list[i] << " error." << endl;
			return -1;
		}
	}

	m_inited = 1;

	return 0;
}

int c_config::get_config(const char *section, const char *name, char *buffer, int buffer_count) const
{
	if (!m_inited) {
		return -1;
	}

	if (!m_ini.section_exists(section) || !m_ini.variable_exists(section, name)) {
		//cerr << "ERROR: " << section << ": " << name << "." << endl;
		return -1;
	}

	if (buffer == NULL) {
		return m_ini.variable_value(section, name).size();	
	}

	memset(buffer, 0, buffer_count);
	strncpy(buffer, m_ini.variable_value(section, name).c_str(), buffer_count - 1);
	return 0;
}

int c_config::set_config(const char *section, const char *name, const char *value)
{
	if (!m_inited) {
		return -1;
	}
	return 0;
}

int c_config::uninit()
{
	m_inited = 0;
	return 0;
}

int c_config::release()
{
	if (m_inited) {
		uninit();
	}

	delete this;

	return 0;
}

