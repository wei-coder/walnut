<<<<<<< HEAD
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

//#include "types.h"

struct utsname {
    char sysname[9];
    char nodename[9];
    char release[9];
    char version[9];
    char machine[9];
};

extern int uname(struct utsname * utsbuf);

#endif

=======
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

//#include "types.h"

struct utsname {
    char sysname[9];
    char nodename[9];
    char release[9];
    char version[9];
    char machine[9];
};

extern int uname(struct utsname * utsbuf);

#endif

>>>>>>> 2747b789ab05c6901e2e08b9db7fbb72fd8f6f24
