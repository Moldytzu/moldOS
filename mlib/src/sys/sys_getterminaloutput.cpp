#include <mlib.h>

const char* sys_getterminaloutput(uint64_t terminal) {
    return (const char*)sys_call(SYS_GETTERMINALOUTPUT,terminal,0,0);
}