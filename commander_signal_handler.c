#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "commander_signal_handler.h"

extern int flag ;

void hdl (int sig)
{
    if(sig==SIGRTMIN)
    {
        printf("alarm :Client will read from second pipe !\n");
        flag = 1 ;
    }
}
