#ifndef _TSC_H_
#define _TSC_H_

extern int tscmsec;
extern long long tscsec; 
extern long long current_tsc;

void tsc_init ();
#endif
