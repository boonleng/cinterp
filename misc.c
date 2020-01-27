/*
 *  misc.c
 *
 *  Miscellaneous functions to ease coding.
 *
 *  Created by Boon Leng Cheong. All rights reserved.
 *
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

/*
 * Here is a nice reference: http://www.cplusplus.com/ref/ctime/time.html
 */
char *now()
{
    static char timestr[64];
    time_t utc;
    time(&utc);
    strftime(timestr, 63, "%H:%M:%S", localtime(&utc));
    return timestr;
}

/*
 * Comma separated thousands
 */
char *commaint(long long num)  /* Input number */
{
    static int i = 7;
    static char buf[8][64];
    
    /* Might need a semaphore to protect the following line */
    i = i == 7 ? 0 : i + 1;
    
    int b = i;
    snprintf(buf[b], 48, "%lld", num);
    if (num >= 1000) {
        int c = (int)(strlen(buf[b]) - 1) / 3; /* Number of commans */
        int p = (int)(strlen(buf[b])) + c;     /* End position      */
        int d = 1;                             /* Count of digits   */
        buf[b][p] = '\0';
        while (p > 0) {
            p--;
            buf[b][p] = buf[b][p - c];
            if (d > 3) {
                d = 0;
                buf[b][p] = ',';
                c--;
            }
            d++;
        }
    }
    return buf[b];
}

