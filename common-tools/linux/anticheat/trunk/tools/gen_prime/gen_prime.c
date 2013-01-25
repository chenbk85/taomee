#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{  
	int i,j;
	int count = 0, gen_count = 0, max = 0, line_count = 8;

	if (argc != 3 && argc != 4) {
		printf("Usage: %s <max> <gen_count> [line_count]\n", argv[0]);
		return 0;
	}
	max = atoi(argv[1]);
	gen_count = atoi(argv[2]);
	if (argc == 4) {
		line_count = atoi(argv[3]);
	}

	for (i = max; i > max/3; i--) {
		if (i % 2 == 0) continue;

		j=3;
		while (j <= i/2 && i%j != 0) j += 2;

		if(j > i/2) {
			if(count % line_count == 0) printf("\n");
			printf("%d, ", i);
			count++;
		}   

		if(count >= gen_count) break;
	}   
	printf("\n");
	return 0;				    
}   
