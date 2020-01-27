/*
 *  misc.h
 *
 *  Miscellaneous functions to ease coding.
 *
 *  Created by Boon Leng Cheong. All rights reserved.
 *
 */

#ifndef _wfilter_misc_h
#define _wfilter_misc_h

#ifndef MAX
#define MAX(a,b)   (a > b ? a : b)
#endif

#ifndef MIN
#define MIN(a,b)   (a < b ? a : b)
#endif

#define DTIME(T1,T2)  ((double)((T2).tv_sec-(T1).tv_sec)+1e-6*(double)((T2).tv_usec-(T1).tv_usec))

/* now
 *
 * produces the current time in string
 *
 * Returns:
 *   A string with current time
 */
char *now();

/* commint
 *
 * produces the input number in string representation that has
 * the numbers grouped in thousands and deliminates them with
 * comma(s).
 *
 * Returns:
 *   A string of the number
 */
char *commaint(long long num   /* Input number */
               );

#endif
