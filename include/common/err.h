#ifndef _ERR_
#define _ERR_

struct LogErr {

    static void fatal(const char *fmt, ...);

    static void logerr(const char *fmt, ...);

    static void syserr(const char *fmt, ...);
};


#endif
