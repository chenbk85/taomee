#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ip_dict.hpp"
int main(int argc,char **argv)
{
	if(argc != 3) {
        fprintf(stderr, "\tUsage: %s ip(ulong or dot string) dict\n", argv[0]);
        return -1;
    }

	int a1,a2,a3,a4;
	a1=a2=a3=a4=-1;

	uint32_t ip = strtoul(argv[1], NULL, 10);
	if(ip<256)
	{
		sscanf(argv[1],"%d.%d.%d.%d",&a1,&a2,&a3,&a4);
		ip = a1*256*256*256+a2*256*256+a3*256+a4;
	}

	if (g_ipDict.init(argv[2]) == false) {
        printf("open dict failed\n");
        return -1;
    }

	code_t code;
	uint32_t key;
	if(g_ipDict.find(ip, code, &key))
		printf("%s,province_code=%u,city_code=%u,isp:%s\n",argv[1],code.province_code,code.city_code,code.isp);
	return 0;

}
