/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : jeu sep 21 20:44:54 CEST 2000
    copyright            : (C) 2000 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
	Name : SDL_Example
	Desc: 	- Draw a map with 2 transparent layers, animations and a parallax
			- mvt of a sprite with the arrows
			- the sprite's speed is the same for every frame-rate you have
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include "SDLMappy.h"
void deal_map (int index, char * filename, bool isend )  
{
	BLKSTR * blk;
	SDLMappy* Map = new SDLMappy();
	char fmtstr[]=	  "	BIN_8(%d,%d,%d,%d,%d,%d,%d,%d),\n";
	char fmtstr_end[]="	BIN_8(%d,%d,%d,%d,%d,%d,%d,%d)\n";
	char * fmtstr_cur;

	if (Map->LoadMap(filename , 0, 0,800 ,600 )==-1){
		printf("err open file [%s]\n",filename);
		return;
	}

	for (int c=0;c<8;c++) {
		Map->MapChangeLayer(c);
		printf("//layer %d data \n",index*8+c );
		printf("{ \n" );
		for (int i=0;i<17;i++) {
				
			if (i!=16){ fmtstr_cur=fmtstr; }
			else fmtstr_cur=fmtstr_end;

			printf(fmtstr_cur,  
				Map->MapGetBlock(i,0)->user1, 
				Map->MapGetBlock(i,1)->user1, 
				Map->MapGetBlock(i,2)->user1, 
				Map->MapGetBlock(i,3)->user1, 
				Map->MapGetBlock(i,4)->user1, 
				Map->MapGetBlock(i,5)->user1, 
				Map->MapGetBlock(i,6)->user1, 
				Map->MapGetBlock(i,7)->user1 
			);

				
		}
		if ( isend && c==16 ) printf("}\n"  );
		else	printf("},\n");
	}

	delete(Map);
}	

int main(int argc, char *argv[])
{
	int i;
	if  (argc<2){
		printf("can't input file'\n");
		return 1;
	}
	printf("uint8_t init_map_data[][17]={ \n" );
	for (i=1;i<argc;i++){
		if (i==argc-1) deal_map(i-1,argv[i],true);
		else deal_map(i-1,argv[i],false);
	}	
	printf("};\n" );
	return 0;
}

