#ifndef _ERR_
#define _ERR_

namespace Err {

    void fatal(const char *fmt, ...);

    void logerr(const char *fmt, ...);

    void syserr(const char *fmt, ...);
};


#endif
