/*
 * =====================================================================================
 *
 *       Filename:  t.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年02月19日 12时03分53秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */
#include "homemap_def.h"
int
main ( int argc, char *argv[] )
{
//	int i;
//	for (i=0;i<8;i++ )
//		printf("%X %X %X\n", init_map_data[0][i*3 + 0], 
//				init_map_data[0][i*3+ 1], init_map_data[0][i*3+2]);
	
	printf ("%d\n",  (uint8_t) (0x08)<<4   );
	return 0;
}				/* ----------  end of function main  ---------- */
