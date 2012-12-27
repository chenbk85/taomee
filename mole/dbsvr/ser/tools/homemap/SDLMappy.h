/***************************************************************************
                          SDLMappy.h  -  description
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

//////////////////////////////////////////////////////////////////////////////////
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

#ifndef SDLMAPPY_H
#define SDLMAPPY_H

#include <malloc.h>
#include <SDL/SDL.h>

#define MER_NONE 0			/* All the horrible things that can go wrong */
#define MER_OUTOFMEM 1
#define MER_MAPLOADERROR 2
#define MER_NOOPEN 3

#define AN_END -1					/* Animation types, AN_END = end of anims */
#define AN_NONE 0					/* No anim defined */
#define AN_LOOPF 1					/* Loops from start to end, then jumps to start etc */
#define AN_LOOPR 2					/* As above, but from end to start */
#define AN_ONCE 3					/* Only plays once */
#define AN_ONCEH 4					/* Only plays once, but holds end frame */
#define AN_PPFF 5					/* Ping Pong start-end-start-end-start etc */
#define AN_PPRR 6					/* Ping Pong end-start-end-start-end etc */
#define AN_PPRF 7					/* Used internally by playback */
#define AN_PPFR 8					/* Used internally by playback */
#define AN_ONCES 9					/* Used internally by playback */

typedef struct {					/* Structure for data blocks */
 int bgoff , fgoff;	/* offsets from start of graphic blocks */
 int fgoff2, fgoff3; /* more overlay blocks */
 unsigned long int  user1, user2;	/* user long data */
 unsigned short int user3, user4;	/* user short data */
 unsigned char user5, user6, user7;	/* user byte data */
 unsigned char tl : 1;				/* bits for collision detection */
 unsigned char tr : 1;
 unsigned char bl : 1;
 unsigned char br : 1;
 unsigned char trigger : 1;			/* bit to trigger an event */
 unsigned char unused1 : 1;
 unsigned char unused2 : 1;
 unsigned char unused3 : 1;
} BLKSTR;

typedef struct {					/* Animation control structure */
    signed char antype;				/* Type of anim, AN_? */
    signed char andelay;			/* Frames to go before next frame */
    signed char ancount;			/* Counter, decs each frame, till 0, then resets to andelay */
    signed char anuser;				/* User info */
    long int    ancuroff;			/* Points to current offset in list */
    long int    anstartoff;			/* Points to start of blkstr offsets list, AFTER ref. blkstr offset */
    long int    anendoff;			/* Points to end of blkstr offsets list */
} ANISTR;

typedef struct {					/* Generic structure for chunk headers */
    char id1, id2, id3, id4;		/* 4 byte header id. */
    long int headsize;				/* size of header chunk. */
} GENHEAD;

typedef struct {					/* Map header structure */
    char mapverhigh;				/* map version number to left of . (ie X.0). */
	char mapverlow;					/* map version number to right of . (ie 0.X). */
	char lsb;						/* if 1, data stored LSB first, otherwise MSB first. */
	char reserved;
	short int mapwidth;				/* width in blocks. */
	short int mapheight;			/* height in blocks. */
	short int reserved1;
	short int reserved2;
	short int blockwidth;			/* width of a block (tile) in pixels. */
	short int blockheight;			/* height of a block (tile) in pixels. */
	short int blockdepth;			/* depth of a block (tile) in planes (ie. 256 colours is 8) */
	short int blockstrsize;			/* size of a block data structure */
	short int numblockstr;			/* Number of block structures in BKDT */
	short int numblockgfx;			/* Number of 'blocks' in graphics (BGFX) */
} MPHD;

class SDLMappy
{
private:
	// These define how far from the left and top of the surface the drawn area is (en pixels)
	int			MMOX;
	int			MMOY;

	// These define the width and height of the drawn area... (en pixels)
	int			MSCRW;
	int			MSCRH;

	/* These define the block size */
	int			MBSW;				/* Width of block, in pixels */
	int			MBSH;				/* Height of block, in pixels */
	
    // These are for the parallax surface
    int         PARAW;              // width of the parallax surface - blockwidth
    int         PARAH;              // height of the parallax surface - blockheight

	short int	mapwidth,           // taille carte en blocks
				mapheight,          // taille carte en blocks
				mapblockwidth,      // taille block en pixels
				mapblockheight,     // taille block en pixels
				mapdepth;           // map BPP
	short int	mapblockstrsize,
				mapnumblockstr,     // Nombre total de blocs (?)
				mapnumblockgfx;     // (????)
				
	GENHEAD		mapgenheader;

    int         XPosition , YPosition;          // X/Y position of the map en pixels

	short int	* mappt;             // contient les numeros des tiles pour la layer selectionnee
	short int 	* mapmappt[8];       // contient les numeros des tiles des 8 layers
	
	unsigned char * mapcmappt;   // Colour palette for 8bit maps
	SDL_Color	mappept[256];    // les valeurs RGB des 256 couleurs pour les cartes 8 bits
	char		* mapblockgfxpt; // The raw graphics in whatever format the map is in
	char		* mapblockstrpt; // Block data. Contains BLKSTR structures...
								 // ...for however many block structures were made
	char		* mapanimstrpt;    // Animation data. Contains ANISTR structures for however many...
	char		* mapanimstrendpt; // ...animation structures were made, and also animation data

	int			maperror;				//	Set to a MER_ error if something wrong happens
	
	FILE		* mapfilept;            // fichier contenant la map
	
	SDL_Surface	* maplpDDSTiles[1024];  // surfaces contenant chaques tiles
	
	unsigned long ANDTSize;		// taille du bloc ANDT (utilise par l'affichage des anims
								// dans DrawBG
	Uint32		colorkey;
	
    SDL_Surface *ParallaxSurface;                // parallax surface object
    char        *ParallaxFilename;               // filename of the parallax .bmp
		
public:
	~SDLMappy();
	SDLMappy( void );

	void		MapFreeMem (void);
	
	int 		MapRelocate(void);
	int 		LoadMap(/*CDXScreen * Screen ,*/ char * Filename ,
						int left , int top , int width , int height );
	int			MapLoad (char * mapname/*, RBDIRECTDRAW lpDD*/);
	unsigned long int Mapbyteswapl (unsigned long int i);
	int 		MapDecodeNULL (void);
	int 		MapDecodeMPHD (void);
	int		 	MapDecodeBODY (void);
	int			MapDecodeCMAP (void);
	int			MapDecodeBKDT (void);
	int			MapDecodeANDT (void);
	int			MapDecodeBGFX (void);
	int			MapDecodeLYR1 (void);
	int			MapDecodeLYR2 (void);
	int			MapDecodeLYR3 (void);
	int			MapDecodeLYR4 (void);
	int			MapDecodeLYR5 (void);
	int			MapDecodeLYR6 (void);
	int			MapDecodeLYR7 (void);

	void 		MapCMAPtoPE (unsigned char * mycmappt, SDL_Color * mypept);
	
	BLKSTR * 	MapGetBlock (int x, int y);
	void 		MapSetBlock (int x, int y, int strvalue);
	short int	MapGetTile( int x , int y );
	
    int			MapGetXPosition( void ) { return XPosition; };
    int			MapGetYPosition( void ) { return YPosition; };
	short int	GetMapWidth( void );
	short int	GetMapHeight( void );
	short int	GetMapWidthInBlocks( void );
	short int	GetMapHeightInBlocks( void );
	short int	GetMapBlockWidth( void );
	short int	GetMapBlockHeight( void );
	short int	GetMapBPP( void );
		
	void 		MapMoveTo( int x , int y );
	int			MapChangeLayer (int newlyr);
	int 		MapDrawBG (SDL_Surface * lpDDS );
	int			MapDrawBGT (SDL_Surface * lpDDS );
	void 		MapInitAnims (void);
	void 		MapUpdateAnims (void);
	
    SDL_bool    CreateParallax( char * Filename );
    int         DrawParallax (SDL_Surface * lpDDS );
    SDL_bool    RestoreParallax( void );

};

#endif
