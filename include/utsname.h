#pragma once

struct utsname {
    char sysname[10];
    char nodename[10];
    char release[10];
    char version[10];
    char machine[10];
};
