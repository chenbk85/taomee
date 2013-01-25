#include "version.h"
#include "version.h"
#include "libtaomee++/utils/strings.hpp"
char g_version_str [255]="ONLINE-0805-";

const char *  get_version_str(const char * version_str ) {
    std::vector<std::string> file_name_list;
   	return get_version_send_buf(version_str,file_name_list,false );
}

