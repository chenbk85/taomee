#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "show_libevent_info.h"

void print_libevent_version(void)
{
	fprintf(stderr, "\t\tStarting Libevent: %s\n", event_get_version());
}

void print_supported_backend(void)
{
	int i;
	const char **methods = event_get_supported_methods();
	for (i=0; methods[i] != NULL; ++i) {
		fprintf(stderr, "\t\t%s\n", methods[i]);
	}
}

#if 0 /* NEED add libevent `src-dir' to include  */
void print_basic_struct_sizes(void)
{
#define PRINT_STRUCT_SIZE(stru) \
	do { \
		fprintf(stderr, "\t\tsizeof(struct "#stru") = %zd B\n", sizeof(struct stru)); \
	} while (0)

	PRINT_STRUCT_SIZE(event_base);
	PRINT_STRUCT_SIZE(event);
	PRINT_STRUCT_SIZE(eventop);
	PRINT_STRUCT_SIZE(event_config);
	PRINT_STRUCT_SIZE(bufferevent);
	PRINT_STRUCT_SIZE(bufferevent_ops);
	PRINT_STRUCT_SIZE(evbuffer);
	PRINT_STRUCT_SIZE(evbuffer_chain);

#undef PRINT_STRUCT_SIZE
}
#endif

void print_sys_info(void)
{
	fprintf(stderr, "\033[1;4mSystem basic info:\033[0m\n\n");

	fprintf(stderr, "\t\033[1mLibevent version:\033[0m\n");
	print_libevent_version();

	fprintf(stderr, "\t\033[1mSupported backends:\033[0m\n");
	print_supported_backend();

#if 0 /* NEED add libevent `src-dir' to include  */
	fprintf(stderr, "\t\033[1mStructer sizes:\033[0m\n");
	print_basic_struct_sizes();
#endif

	puts("------------------------------------------------------------");
}

void print_event_base_info(struct event_base *base)
{
	printf("Libevent backend method: [%s]\n",
			event_base_get_method(base));
	enum event_method_feature f;
	f = event_base_get_features(base);
	if (f & EV_FEATURE_ET) {
		printf("  Edge-triggered events are supported.\n");
	}
	if (f & EV_FEATURE_O1) {
		printf("  O(1) event notification is supported.\n");
	}
	if (f & EV_FEATURE_FDS) {
		printf("  All FD types are supported.\n");
	}

	puts("============================================================\n");
}

int main(int argc, char **argv)
{
	print_sys_info();
	print_event_base_info(event_base_new());

	return 0;
}
