/* example.c*/
#include <stdio.h>
#include <pthread.h>

void* thread(void* arg)
{
	printf("========1\n");
	return NULL;
}

int main(void)
{
	pthread_t id,id1;
	int ret;
	printf("========\n");
	ret=pthread_create(&id,NULL,thread,NULL);
	ret=pthread_create(&id1,NULL,thread,NULL);
	pthread_join(id,NULL);
	printf("1========\n");
	pthread_join(id1,NULL);
	printf("2========\n");
	return (0);
}
