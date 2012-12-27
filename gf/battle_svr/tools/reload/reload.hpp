#ifndef __RELOAD_HPP__
#define __RELOAD_HPP__

#define MJR_VER 0
#define MIN_VER 75
#define PROG_DESC "A tool for remote reloading Paipai's online config"

#define DIM(a) (sizeof(a) / sizeof(a[0]))

#define print_ver() \
	do { \
		printf("Version: %d.%d\n", MJR_VER, MIN_VER); \
	} while(0) 

#define print_usage(argv, param) \
	do {\
		printf("Usage: %s %s\n", argv[0], param);\
		printf("-h: Help(this info list)\n"); \
		printf("-v: Version\n"); \
		printf("-0...: Figures stand for various config\n"); \
		printf("\tYou can also use long_param:\n"); \
		for (uint32_t i = 0; i < DIM(long_options); i++) {\
			if (long_options[i].val == 'h' || long_options[i].val == 'v') \
				continue; \
			if (long_options[i].val == '4') { \
				printf("\033[1;30m" "\t-%c = --%s (hold-space, but no use)\n" "\033[0m", \
						long_options[i].val, long_options[i].name); \
				continue; \
			} \
			printf("\t-%c = --%s\n", long_options[i].val, long_options[i].name); \
		} \
		printf("\tFollow 'mcast'/'server_id' for mcast/single reloading config\n"); \
	} while(0)

#define print_desc() \
	do { \
		printf("\033[1;37m" "%s\n\n" "\033[0m", PROG_DESC); \
	} while(0)

#define print_func_name() do { printf("[%s]\n", __FUNCTION__); } while(0)

#endif /* __RELOAD_HPP__ */
