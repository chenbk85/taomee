/*
 * =====================================================================================
 * 
 *       Filename:  pub.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  02/25/2008 05:23:26 PM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  spark (spark), zhezhide@163.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  PUB_INC
#define  PUB_INC


#ifdef __cplusplus
extern "C" {
#endif

int Trim(char *s);
extern int cpEncodeBase64(const unsigned char *sSrc, unsigned char *sTarget, const int nSize);
extern int cpDecodeBase64(const unsigned char *sSrc, unsigned char *sTarget, const int nSize);

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef PUB_INC  ----- */

