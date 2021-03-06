#include "syscall.h"
#include "sched.h"
#include "util.h"

#define NB_PROCESS 5

void user_process()
{
    int v=0;
    for(;;)
    {
        v++;
        sys_yield();
    }
    sys_exit(0);
}

void kmain( void )
{
    sched_init(SP_SIMPLE);

    int i;
    for(i=0;i<NB_PROCESS;i++)
    {
        create_process((func_t*)&user_process, PP_MEDIUM);
    }

    __asm("cps 0x10"); // switch CPU to USER mode
    // ******************************************

    while(1)
    {
        sys_yield();
    }
}
