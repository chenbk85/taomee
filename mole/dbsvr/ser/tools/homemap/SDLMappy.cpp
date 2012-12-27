/***************************************************************************
                          SDLMappy.cpp  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000 by Regis Quercioli
    email                : edorul@caramail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////
//
// SDLMappy
//
// (c) based on on sources of Robin Burrows / rburrows@bigfoot.com.
// Mappy homepage. The current Homepage for Mappy is:
// http://www.geocities.com/SiliconValley/Vista/7336/robmpy.htm
//
// Ported to SDL and modified by Regis Quercioli / edorul@caramail.com
//
//

#include "SDLMappy.h"

// ---------------------------------------------------------------------------------
// destructor of this class, inits all members
// ---------------------------------------------------------------------------------
SDLMappy::~SDLMappy( )
{
//    SAFEDELETE( ParallaxFilename );
//    SAFEDELETE( ParallaxSurface );
	MapFreeMem( );
}

// ---------------------------------------------------------------------------------
// constructor of this class, inits all members
// ---------------------------------------------------------------------------------
SDLMappy::SDLMappy( void )
{
	int		i;

	MMOX = 0;
	MMOY = 0;

	MSCRW = 320;
	MSCRH = 200;

	maperror		= 0;
	mapwidth		= 0;
	mapheight		= 0;
	mapblockwidth	= 0;
	mapblockheight	= 0;
	mapdepth		= 0;

	mapblockstrsize	= 0;
	mapnumblockstr	= 0;
	mapnumblockgfx	= 0;

	mapfilept		= NULL;
	mappt			= NULL;
//	maparraypt		= NULL;
	mapcmappt		= NULL;
	mapblockgfxpt	= NULL;
	mapblockstrpt	= NULL;
	mapanimstrpt	= NULL;
	mapanimstrendpt = NULL;

	for( i=0; i<1024; i++ )
		maplpDDSTiles[ i ] = NULL;

	for( i=0; i<8; i++ )
	{
		mapmappt[ i ]      = NULL;
//		mapmaparraypt[ i ] = NULL;
	}

	XPosition = YPosition = 0;

//	ParallaxSurface  = NULL;
//	ScreenObject     = NULL;
//	ParallaxFilename = NULL;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
void SDLMappy::MapFreeMem (void)
{
int i;
	for (i=0;i<8;i++) { if (mapmappt[i]!=NULL) { free (mapmappt[i]); mapmappt[i] = NULL; } }
	mappt = NULL;
//	for (i=0;i<8;i++) { if (mapmaparraypt[i]!=NULL) { free (mapmaparraypt[i]); mapmaparraypt[i] = NULL; } }
//	maparraypt = NULL;
	if (mapcmappt!=NULL) { free (mapcmappt); mapcmappt = NULL; }
	if (mapblockgfxpt!=NULL) { free (mapblockgfxpt); mapblockgfxpt = NULL; }
	if (mapblockstrpt!=NULL) { free (mapblockstrpt); mapblockstrpt = NULL; }
	if (mapanimstrpt!=NULL) { free (mapanimstrpt); mapanimstrpt = NULL; }

	for (i=0;(maplpDDSTiles[i]!=NULL);i++)
		SDL_FreeSurface(maplpDDSTiles[i]);
}

// ---------------------------------------------------------------------------------
// loads a map, left and top are by default 0
// bottom and right are by default -1 which means screen size
// ---------------------------------------------------------------------------------
int SDLMappy::LoadMap( /*SDL_Surface * Screen ,*/ char * Filename ,
					   int left , int top , int width , int height )
{
	int result;

	MMOX = left;
	MMOY = top;

	MSCRW = width;
	MSCRH = height;

	XPosition = YPosition = 0;

//	ScreenObject = Screen;

	// load the map
	result = MapLoad( Filename /*, Screen->GetDD()*/ );

    // if the map loaded ok, proceed, else return error code
	if( result != -1 )
	{
        // RB
        // Since the draw routines will end the application if you try to draw
        // outside the map area, the width and height must be =< the map
        if( ( mapwidth * mapblockwidth ) < MSCRW )
            MSCRW = ( mapwidth * mapblockwidth );

        if( ( mapheight * mapblockheight ) < MSCRH )
            MSCRH = ( mapheight * mapblockheight );

        // Also, the drawn area must be at least 3*3 tiles big
        if( MSCRW < ( mapblockwidth * 3 ) )
            MSCRW = mapblockwidth * 3;

        if( MSCRH < ( mapblockheight * 3 ) )
            MSCRH = mapblockheight * 3;


		//set the palette
//		Screen->SetPalette( 0, 256, mappept );

		MBSW = mapblockwidth;
		MBSH = mapblockheight;
	}

	return result;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapRelocate(void)
{
	int i, deplct;
	
	switch(mapdepth)
	{
		case 8:
			deplct=mapblockwidth*mapblockheight;
			for(i=0; i<mapnumblockstr; i++)
			{
				maplpDDSTiles[i] = SDL_CreateRGBSurfaceFrom (mapblockgfxpt+(deplct)*i,
						mapblockwidth, mapblockheight, mapdepth, mapblockwidth, 0, 0, 0, 0);			
				SDL_SetColors(maplpDDSTiles[i], mappept, 0, 256);
			}
			break;
		case 16:
			deplct=mapblockwidth*mapblockheight*2;
			for(i=0; i<mapnumblockstr; i++)
				maplpDDSTiles[i] = SDL_CreateRGBSurfaceFrom (mapblockgfxpt+(deplct)*i,
						mapblockwidth, mapblockheight, mapdepth, mapblockwidth*2, 0XF800, 0X7E0, 0x1F, 0);			
			break;
		case 24:
			deplct=mapblockwidth*mapblockheight*3;
			for(i=0; i<mapnumblockstr; i++)
				maplpDDSTiles[i] = SDL_CreateRGBSurfaceFrom (mapblockgfxpt+(deplct)*i,
					mapblockwidth, mapblockheight, mapdepth, mapblockwidth*3, 0XFF0000, 0XFF00, 0xFF, 0);			
			break;
		case 32:
			deplct=mapblockwidth*mapblockheight*4;
			for(i=0; i<mapnumblockstr; i++)
				maplpDDSTiles[i] = SDL_CreateRGBSurfaceFrom (mapblockgfxpt+(deplct)*i,
					mapblockwidth, mapblockheight, mapdepth, mapblockwidth*4, 0XFF0000, 0XFF00, 0xFF, 0);			
			break;
		default:
			return -1;
			break;
	}
		
	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapLoad (char * mapname/*, RBDIRECTDRAW lpDD*/)
{
	int			i;
	long int	mapfilesize;

	MapFreeMem ();
	maperror = 0;

	mapfilept = fopen (mapname, "rb");
	if (mapfilept==NULL) { maperror = MER_NOOPEN; return -1; }
	fseek (mapfilept, 0, SEEK_END);
	mapfilesize = ftell (mapfilept);
	fseek (mapfilept, 0, SEEK_SET);
	if (fread (&mapgenheader, 1, sizeof(GENHEAD), mapfilept)!=sizeof(GENHEAD))
	{ maperror = MER_MAPLOADERROR; fclose (mapfilept); return -1; }

	if (mapgenheader.id1!='F') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id2!='O') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id3!='R') maperror = MER_MAPLOADERROR;
	if (mapgenheader.id4!='M') maperror = MER_MAPLOADERROR;

	if (mapfilesize==-1) maperror = MER_MAPLOADERROR;
	if (mapfilesize!=(long int)((Mapbyteswapl(mapgenheader.headsize))+8)) maperror = MER_MAPLOADERROR;

	if (maperror) { fclose (mapfilept); return -1; }

	if (fread (&mapgenheader, 1, sizeof(GENHEAD)-4,mapfilept)!=sizeof(GENHEAD)-4)
	{ maperror = MER_MAPLOADERROR; fclose (mapfilept); return -1; }
	maperror=MER_MAPLOADERROR;
	if (mapgenheader.id1=='F') { if (mapgenheader.id2=='M') { if (mapgenheader.id3=='A')
	{ if (mapgenheader.id4=='P') maperror=MER_NONE; } } }
	if (maperror) { fclose (mapfilept); return -1; }

	while (mapfilesize!=(ftell(mapfilept)))
	{
		if (fread (&mapgenheader, 1, sizeof(GENHEAD),mapfilept)!=sizeof(GENHEAD))
		{ maperror = MER_MAPLOADERROR; fclose (mapfilept); return -1; }

		i = 0;
		
		if (mapgenheader.id1=='M')  if (mapgenheader.id2=='P')  if (mapgenheader.id3=='H')
			if (mapgenheader.id4=='D')
				{MapDecodeMPHD (); i = 1;}
		
		if (mapgenheader.id1=='C')  if (mapgenheader.id2=='M')  if (mapgenheader.id3=='A')
			if (mapgenheader.id4=='P')
				{ MapDecodeCMAP (); i = 1; }
		
		if (mapgenheader.id1=='B')  if (mapgenheader.id2=='K')  if (mapgenheader.id3=='D')
			if (mapgenheader.id4=='T')
				{ MapDecodeBKDT (); i = 1; }
		
		if (mapgenheader.id1=='A')  if (mapgenheader.id2=='N')  if (mapgenheader.id3=='D')
			if (mapgenheader.id4=='T')
				{ MapDecodeANDT (); i = 1; }
		
		if (mapgenheader.id1=='B')  if (mapgenheader.id2=='G')  if (mapgenheader.id3=='F')
			if (mapgenheader.id4=='X')
				{ MapDecodeBGFX (); i = 1; }
		
		if (mapgenheader.id1=='B')  if (mapgenheader.id2=='O')  if (mapgenheader.id3=='D')
			if (mapgenheader.id4=='Y')
				{ MapDecodeBODY (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='1')
				{ MapDecodeLYR1 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='2')
				{ MapDecodeLYR2 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='3')
				{ MapDecodeLYR3 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='4')
				{ MapDecodeLYR4 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='5')
				{ MapDecodeLYR5 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='6')
				{ MapDecodeLYR6 (); i = 1; }
		
		if (mapgenheader.id1=='L')  if (mapgenheader.id2=='Y')  if (mapgenheader.id3=='R')
			if (mapgenheader.id4=='7')
				{ MapDecodeLYR7 (); i = 1; }
		
		if (!i) MapDecodeNULL ();
		if (maperror) { fclose (mapfilept); return -1; }
	}
	fclose (mapfilept);

	return (MapRelocate ());
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
unsigned long int SDLMappy::Mapbyteswapl (unsigned long int i)
{
	unsigned long int j;

	j   = i&0xFF;
	j <<= 8;
	i >>= 8;
	j  |= i&0xFF;
	j <<= 8;
	i >>= 8;
	j  |= i&0xFF;
	j <<= 8;
	i >>= 8;
	j  |= i&0xFF;
	
	return j;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeNULL (void)
{
	char * mynllpt;

	mynllpt = (char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mynllpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mynllpt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);
	free (mynllpt);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeMPHD (void)
{
	MPHD	* hdrmempt;

	hdrmempt = (MPHD *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (hdrmempt==NULL) return 0;
	fread (hdrmempt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);
	mapwidth=hdrmempt->mapwidth;
	mapheight=hdrmempt->mapheight;
	mapblockwidth=hdrmempt->blockwidth;
	mapblockheight=hdrmempt->blockheight;
	mapdepth=hdrmempt->blockdepth;
	mapblockstrsize=hdrmempt->blockstrsize;
	mapnumblockstr=hdrmempt->numblockstr;
	mapnumblockgfx=hdrmempt->numblockgfx;
	free (hdrmempt);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeBODY (void)
{
	mappt = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mappt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);
	mapmappt[0] = mappt;

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeCMAP (void)
{
	mapcmappt = (unsigned char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapcmappt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapcmappt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);
	MapCMAPtoPE (mapcmappt, mappept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeBKDT (void)
{
	mapblockstrpt = (char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapblockstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapblockstrpt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeANDT (void)
{
	ANDTSize =  Mapbyteswapl(mapgenheader.headsize);
	mapanimstrpt = (char *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapanimstrpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	mapanimstrendpt = (char *) (mapanimstrpt+(Mapbyteswapl(mapgenheader.headsize)));
	fread (mapanimstrpt, Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeBGFX (void)
{
	unsigned long int i, size;
	unsigned long int temp;
	
	size=Mapbyteswapl(mapgenheader.headsize);
	mapblockgfxpt = (char *) malloc (size);
	if (mapblockgfxpt==NULL) { maperror = MER_OUTOFMEM; return -1; }
	switch(mapdepth)
	{
		case 8:
			printf("maps en 8 bits non supportees pour l'instant...\n");
			return -1;
			fread (mapblockgfxpt, size, 1, mapfilept);
			break;
		case 16:
			for (i=0; i<size; i+=2)
			{
				fread (&temp, 2, 1, mapfilept);
				mapblockgfxpt[i+1]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i]=temp&0XFF;
			}
			break;
		case 24:
			for (i=0; i<size; i+=3)
			{
				fread (&temp, 3, 1, mapfilept);
				mapblockgfxpt[i+2]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i+1]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i]=temp&0XFF;
			}
			break;
		case 32:
			for (i=0; i<size; i+=4)
			{
				fread (&temp, 4, 1, mapfilept);
				mapblockgfxpt[i+3]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i+2]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i+1]=temp&0XFF;
				temp>>=8;
				mapblockgfxpt[i]=temp&0XFF;
			}
			break;
		default:
			return -1;
			break;
	}

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR1 (void)
{
	mapmappt[1] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[1]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[1], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR2 (void)
{
	mapmappt[2] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[2]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[2], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR3 (void)
{
	mapmappt[3] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[3]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[3], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR4 (void)
{
	mapmappt[4] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[4]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[4], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR5 (void)
{
	mapmappt[5] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[5]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[5], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR6 (void)
{
	mapmappt[6] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[6]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[6], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
int SDLMappy::MapDecodeLYR7 (void)
{
	mapmappt[7] = (short int *) malloc (Mapbyteswapl(mapgenheader.headsize));
	if (mapmappt[7]==NULL) { maperror = MER_OUTOFMEM; return -1; }
	fread (mapmappt[7], Mapbyteswapl(mapgenheader.headsize), 1, mapfilept);

	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
void SDLMappy::MapCMAPtoPE (unsigned char * mycmappt, SDL_Color * mypept)
{
	int i;

	for (i=0;i<256;i++)
	{
		mypept[i].r = (char) *mycmappt; mycmappt++;
		mypept[i].g = (char) *mycmappt; mycmappt++;
		mypept[i].b = (char) *mycmappt; mycmappt++;
		mypept[i].unused = 0;
	}
}

// ---------------------------------------------------------------------------------
// Returns a BLKSTR pointer, useful for collision detection and examining a block structure.
// ---------------------------------------------------------------------------------
BLKSTR * SDLMappy::MapGetBlock (int x, int y)
{
	short int * mymappt;
	ANISTR * myanpt;

/*	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else */{
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	if (*mymappt>=0) return (BLKSTR*) (((char *)mapblockstrpt) + *mymappt);
	else { myanpt = (ANISTR *) (mapanimstrendpt + *mymappt);
		return (BLKSTR *) *((long int *)(myanpt->ancuroff)); }
}

// ---------------------------------------------------------------------------------
// The x and y paramaters are the offset from the left and top of the map in BLOCKS, NOT pixels.
// ---------------------------------------------------------------------------------
void SDLMappy::MapSetBlock (int x, int y, int strvalue)
{
	short int * mymappt;

/*	if (maparraypt!= NULL) {
		mymappt = maparraypt[y]+x;
	} else */{
		mymappt = mappt;
		mymappt += x;
		mymappt += y*mapwidth;
	}
	if (strvalue>=0) *mymappt = strvalue*sizeof(BLKSTR);
	else *mymappt = strvalue*sizeof(ANISTR);
}

// ------------------------------------------------------------
// renvoie le numero de la tile a l'emplacement (x,y) en blocks
//
// returns the content of a block
// if return is positive then the block is a normal tile
// if return is negative then the block (-X) is animation (+X)
// ------------------------------------------------------------
short int SDLMappy::MapGetTile( int x , int y )
{
	short int	Content;

	Content = mappt[x+(y*mapwidth)];

	if( Content >= 0 )
		Content /= sizeof(BLKSTR);
	else
	{
		Content = -Content / sizeof(ANISTR) ;
		Content = -Content + 1;
	}

	return Content;
}

// ---------------------------------------------------------------------------------
// returns the width of the map in pixels
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapWidth( void )
{
	return mapwidth * mapblockwidth;
}



// ---------------------------------------------------------------------------------
// returns the width of the map in pixels
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapHeight( void )
{
	return mapheight * mapblockheight;
}



// ---------------------------------------------------------------------------------
// returns the width of the map in blocks
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapWidthInBlocks( void )
{
	return mapwidth;
}



// ---------------------------------------------------------------------------------
// returns the width of the map in blocks
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapHeightInBlocks( void )
{
	return mapheight;
}



// ---------------------------------------------------------------------------------
// returns the width of one map block in pixels
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapBlockWidth( void )
{
	return mapblockwidth;
}



// ---------------------------------------------------------------------------------
// returns the height of one map block in pixels
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapBlockHeight( void )
{
	return mapblockheight;
}

// ---------------------------------------------------------------------------------
// returns the color depth of the map
// ---------------------------------------------------------------------------------
short int SDLMappy::GetMapBPP( void )
{
    return mapdepth;
}

// ---------------------------------------------------------------------------------
// loads a map, left and top are by default 0
// bottom and right are by default -1 which means screen size
// ---------------------------------------------------------------------------------
void SDLMappy::MapMoveTo( int x , int y )
{
    int Width , Height;

    XPosition = x;
    YPosition = y;

    Width = GetMapWidth( );

    if( XPosition > (Width - 1 - MSCRW))
        XPosition = Width-1-MSCRW;

    if( XPosition < 0 )
        XPosition = 0;

    Height = GetMapHeight( );

    if( YPosition > (Height - 1 - MSCRH))
        YPosition = Height-1-MSCRH;

    if( YPosition < 0 )
        YPosition = 0;
}

// ---------------------------------------------------------------------------------
// change le numero de la layer (0 à 7) devant etre affichee
// ---------------------------------------------------------------------------------
int SDLMappy::MapChangeLayer (int newlyr)
{
	if (newlyr<0 || newlyr>7 || mapmappt[newlyr] == NULL) return -1;
	mappt = mapmappt[newlyr]; //maparraypt = mapmaparraypt[newlyr];
	return newlyr;
}

// ---------------------------------------------------------------------------------
// draws the background layer without Transparency
// ---------------------------------------------------------------------------------
int SDLMappy::MapDrawBG (SDL_Surface * lpDDS )
{
	int			i, j, mapvclip, maphclip;
    int         mapxo, mapyo;
    int 		numtile, numanim;
    ANISTR		* anim;
	SDL_Rect TileDestRect;
	
	for(i=0; i<mapnumblockstr; i++)
		SDL_SetColorKey(maplpDDSTiles[i], 0, 0);
	
	mapxo=XPosition/mapblockwidth;
	mapyo=YPosition/mapblockheight;
	maphclip=XPosition%mapblockwidth;
	mapvclip=YPosition%mapblockheight;	
	
	for(i=0; i<lpDDS->h/mapblockheight+2;i++)
		for(j=0; j<lpDDS->w/mapblockwidth+2;j++)
		{	
			TileDestRect.y	= i*mapblockheight-mapvclip;
			TileDestRect.x	= j*mapblockwidth-maphclip;
			TileDestRect.h	= mapblockheight;
			TileDestRect.w	= mapblockwidth;
			
			numtile = mappt[mapxo+j+((mapyo+i)*mapwidth)];
			
			if (((mapxo+j)<mapwidth) && ((mapyo+i)<mapheight))
			    if (numtile>=0)
					SDL_BlitSurface(maplpDDSTiles[numtile>>5],
										NULL, lpDDS, &TileDestRect);
				else
				{
					anim = (ANISTR *) (mapanimstrendpt + numtile);
					numanim = mapanimstrpt[ANDTSize+anim->ancuroff+1]&0XFF;
					numanim <<= 8;
					numanim |= mapanimstrpt[ANDTSize+anim->ancuroff]&0XFF;
					numanim >>= 5;
					SDL_BlitSurface(maplpDDSTiles[numanim],
										NULL, lpDDS, &TileDestRect);
				}
		}
	
	return 0;
}

// ---------------------------------------------------------------------------------
// draws the background layer WITH Transparency (couleur de transparence = celle du
//			1er block qui doit etre uni)
// ---------------------------------------------------------------------------------
int SDLMappy::MapDrawBGT (SDL_Surface * lpDDS )
{
	int			i, j, mapvclip, maphclip;
    int         mapxo, mapyo;
    int 		numtile, numanim;
    ANISTR		* anim;
	SDL_Rect TileDestRect;
	
	switch(mapdepth)
	{
		case 8:
			for(i=0; i<mapnumblockstr; i++)
				SDL_SetColorKey(maplpDDSTiles[i], SDL_SRCCOLORKEY,
						*(Uint8 *)maplpDDSTiles[0]->pixels);
			break;
		case 16:
			for(i=0; i<mapnumblockstr; i++)
				SDL_SetColorKey(maplpDDSTiles[i], SDL_SRCCOLORKEY,
						*(Uint16 *)maplpDDSTiles[0]->pixels);
			break;
		case 24:
		case 32:
			for(i=0; i<mapnumblockstr; i++)
				SDL_SetColorKey(maplpDDSTiles[i], SDL_SRCCOLORKEY,
						*(Uint32 *)maplpDDSTiles[0]->pixels);
			break;
		default:
			break;
	}
	
	mapxo=XPosition/mapblockwidth;
	mapyo=YPosition/mapblockheight;
	maphclip=XPosition%mapblockwidth;
	mapvclip=YPosition%mapblockheight;	
	
	for(i=0; i<lpDDS->h/mapblockheight+2;i++)
		for(j=0; j<lpDDS->w/mapblockwidth+2;j++)
		{	
			TileDestRect.y	= i*mapblockheight-mapvclip;
			TileDestRect.x	= j*mapblockwidth-maphclip;
			TileDestRect.h	= mapblockheight;
			TileDestRect.w	= mapblockwidth;
			
			numtile = mappt[mapxo+j+((mapyo+i)*mapwidth)];
			
			if (((mapxo+j)<mapwidth) && ((mapyo+i)<mapheight))
			    if (numtile>=0)
					SDL_BlitSurface(maplpDDSTiles[numtile>>5],
										NULL, lpDDS, &TileDestRect);
				else
				{
					anim = (ANISTR *) (mapanimstrendpt + numtile);
					numanim = mapanimstrpt[ANDTSize+anim->ancuroff];
					numanim = mapanimstrpt[ANDTSize+anim->ancuroff+1]&0XFF;
					numanim <<= 8;
					numanim |= mapanimstrpt[ANDTSize+anim->ancuroff]&0XFF;
					numanim >>= 5;
					SDL_BlitSurface(maplpDDSTiles[numanim],
										NULL, lpDDS, &TileDestRect);
				}
		}
	
	return 0;
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
void SDLMappy::MapInitAnims (void)
{
	ANISTR	* myanpt;

	if (mapanimstrpt==NULL) return;
	myanpt = (ANISTR *) mapanimstrendpt; myanpt--;
	while (myanpt->antype!=-1)
	{
		if (myanpt->antype==AN_PPFR) myanpt->antype = AN_PPFF;
		if (myanpt->antype==AN_PPRF) myanpt->antype = AN_PPRR;
		if (myanpt->antype==AN_ONCES) myanpt->antype = AN_ONCE;
		if ((myanpt->antype==AN_LOOPR) || (myanpt->antype==AN_PPRR))
		{
		myanpt->ancuroff = myanpt->anstartoff;
		if ((myanpt->anstartoff)!=(myanpt->anendoff)) myanpt->ancuroff=(myanpt->anendoff)-4;
		} else {
		myanpt->ancuroff = myanpt->anstartoff;
		}
		myanpt->ancount = myanpt->andelay;
		myanpt--;
	}
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
void SDLMappy::MapUpdateAnims (void)
{
	ANISTR	* myanpt;

	if (mapanimstrpt==NULL) return;
	myanpt = (ANISTR *) mapanimstrendpt; myanpt--;
	while (myanpt->antype!=-1)
	{
		if (myanpt->antype!=AN_NONE) { myanpt->ancount--; if (myanpt->ancount<0) {
		myanpt->ancount = myanpt->andelay;
		if (myanpt->antype==AN_LOOPF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) myanpt->ancuroff = myanpt->anstartoff;
		} }
		if (myanpt->antype==AN_LOOPR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4))
				myanpt->ancuroff = (myanpt->anendoff)-4;
		} }
		if (myanpt->antype==AN_ONCE)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->antype = AN_ONCES;
				myanpt->ancuroff = myanpt->anstartoff; }
		} }
		if (myanpt->antype==AN_ONCEH)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) {
			if (myanpt->ancuroff!=((myanpt->anendoff)-4)) myanpt->ancuroff+=4;
		} }
		if (myanpt->antype==AN_PPFF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 8;
			myanpt->antype = AN_PPFR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff +=4; }
		} } else {
		if (myanpt->antype==AN_PPFR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 8;
			myanpt->antype = AN_PPFF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff -=4; }
		} } }
		if (myanpt->antype==AN_PPRR)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff-=4;
			if (myanpt->ancuroff==((myanpt->anstartoff)-4)) { myanpt->ancuroff += 8;
			myanpt->antype = AN_PPRF;
			if (myanpt->ancuroff>myanpt->anendoff) myanpt->ancuroff -=4; }
		} } else {
		if (myanpt->antype==AN_PPRF)
		{
			if (myanpt->anstartoff!=myanpt->anendoff) { myanpt->ancuroff+=4;
			if (myanpt->ancuroff==myanpt->anendoff) { myanpt->ancuroff -= 8;
			myanpt->antype = AN_PPRR;
			if (myanpt->ancuroff<myanpt->anstartoff) myanpt->ancuroff +=4; }
		} } }
	} } myanpt--; }
}

// ---------------------------------------------------------------------------------
// creates a parallax surface from a bitmap file
// returns true, if an error occurs, returns FALSE
// errors can be: bitmap not found
// caution: the parallax surface size must be a multiple of the map block size
// ---------------------------------------------------------------------------------
SDL_bool SDLMappy::CreateParallax( char * Filename )
{
    // set the global variable to the filename
    ParallaxFilename = new char[ strlen(Filename)+1 ];
    strcpy( ParallaxFilename , Filename );

    return RestoreParallax( );
}

// ---------------------------------------------------------------------------------
// creates a parallax surface from a bitmap file
// returns true, if an error occurs, returns FALSE
// errors can be: bitmap not found
// caution: the parallax surface size must be a multiple of the map block size
// ---------------------------------------------------------------------------------
SDL_bool SDLMappy::RestoreParallax( void )
{
    SDL_Surface *SourceSurfaceTmp;

    // when no map is loaded, return an error
    if( GetMapWidth() == 0 )
        return SDL_FALSE;

    // load the bitmap into a surface
	if(ParallaxFilename == NULL)
		return SDL_FALSE;
	
	/* Load a BMP sprite into a surface */
	SourceSurfaceTmp = SDL_LoadBMP(ParallaxFilename);
	if ( SourceSurfaceTmp == NULL )
		return SDL_FALSE;
		
	/* Convert the sprite to the video format (maps colors) */
	ParallaxSurface = SDL_DisplayFormat(SourceSurfaceTmp);
	SDL_FreeSurface(SourceSurfaceTmp);
	if ( ParallaxSurface == NULL )
		return SDL_FALSE;

    PARAW = ParallaxSurface->w;
    PARAH = ParallaxSurface->h;


    return SDL_TRUE;
}

int SDLMappy::DrawParallax (SDL_Surface * lpDDS )
{
	int			i, j, mapvclip, maphclip;
    int         mapxo, mapyo;
    int 		indentx, indenty;
	SDL_Rect TileDestRect;
	
	mapxo=XPosition;
	mapyo=YPosition;
	maphclip = mapxo%PARAW;	/* Number of pixels to clip from left */
	mapvclip = mapyo%PARAH;	/* Number of pixels to clip from top */
	indentx = ((mapxo/2)%PARAW);
	indenty = ((mapyo/2)%PARAH);
	
	for(i=0; i<lpDDS->h/PARAH+2;i++)
		for(j=0; j<lpDDS->w/PARAW+2;j++)
		{	
			TileDestRect.y	= i*PARAH-indenty;
			TileDestRect.x	= j*PARAW-indentx;
			TileDestRect.h	= PARAH;
			TileDestRect.w	= PARAW;
			
			SDL_BlitSurface(ParallaxSurface, NULL, lpDDS, &TileDestRect);
		}
	
	return 0;
}