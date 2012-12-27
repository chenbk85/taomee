#include <stdio.h>
#include "player_attr.hpp"
int main()
{
	for (int i=1;i<4;i++)
	{
		printf("this no.%d \n",i);
		for (int level=1;level<=100;level++)
		{
				printf("%10d",calc_strength[i](level));
				printf("%10d",calc_agility[i](level));
				printf("%10d",calc_body_quality[i](level));
				printf("%10d",calc_stamina[i](level));
				printf("%10d",calc_mp[i](calc_stamina[i](level)));
				printf("%10d",calc_hp[i](calc_body_quality[i](level)));
				printf("%10d",calc_exp(level));
				printf("%10d",calc_atk(calc_strength[i](level)));
				printf("%10f",calc_def_rate(calc_body_quality[i](level)));
				printf("%10f",calc_def_rate(calc_body_quality[0](level)));
				printf("%10f",calc_def_rate(calc_body_quality[4](level)));
			printf("\n");
		}

	}

	printf("end test print last value:%d\n",calc_exp(100));
	return 0;
}

