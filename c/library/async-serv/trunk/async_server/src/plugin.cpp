#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "plugin.h"

static void *plugin_handle = NULL;
struct plugin_t g_plugin;

#define PLUGIN_LOADFUN(fptr, fname) \
    do { \
        fptr = (typeof(fptr))dlsym(plugin_handle, fname); \
        if (!fptr) { \
            dlerror(); \
            dlclose(plugin_handle); \
            BOOT_LOG(-1, "load plugin file:%s dlsym no find:%s", filename, fname); \
        } \
    } while (0) \

#define PLUGIN_LOADFUN_IGNORE(fptr, fname) \
    do { \
        fptr = (typeof(fptr))dlsym(plugin_handle, fname); \
        if (!fptr) { \
            DEBUG_LOG("dlsym no find:%s", fname); \
            dlerror(); \
        } \
    } while (0) \

int plugin_load(const char *filename)
{
    char *error_msg;
    plugin_handle = dlopen(filename, RTLD_NOW);
    if ((error_msg = dlerror()) != NULL)
        BOOT_LOG(-1, "load plugin file:%s %s", filename, error_msg);

    dlerror();

    PLUGIN_LOADFUN_IGNORE(g_plugin.plugin_init, "plugin_init");
    PLUGIN_LOADFUN_IGNORE(g_plugin.plugin_fini, "plugin_fini");
    PLUGIN_LOADFUN_IGNORE(g_plugin.time_event, "time_event");
    PLUGIN_LOADFUN(g_plugin.get_pkg_len_cli, "get_pkg_len_cli");
    PLUGIN_LOADFUN(g_plugin.get_pkg_len_ser, "get_pkg_len_ser");

    PLUGIN_LOADFUN_IGNORE(g_plugin.check_open_cli, "check_open_cli");
    PLUGIN_LOADFUN_IGNORE(g_plugin.select_channel, "select_channel");
    PLUGIN_LOADFUN_IGNORE(g_plugin.shmq_pushed, "shmq_pushed");

    PLUGIN_LOADFUN(g_plugin.proc_pkg_cli, "proc_pkg_cli");
    PLUGIN_LOADFUN(g_plugin.proc_pkg_ser, "proc_pkg_ser");

    PLUGIN_LOADFUN_IGNORE(g_plugin.link_up_cli, "link_up_cli");
    PLUGIN_LOADFUN_IGNORE(g_plugin.link_down_cli, "link_down_cli");
    PLUGIN_LOADFUN_IGNORE(g_plugin.link_down_ser, "link_down_ser");

    BOOT_LOG(0, "load plugin file:%s", filename);
}

void plugin_unload()
{
    dlclose(plugin_handle);
}
