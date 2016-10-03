#include <stdio.h>
#include <stdlib.h>
# include <fcntl.h>
#include "server_file_manipulation.h"


void openFile( char * filepath ,int flags ,mode_t perms,int * fd)
{
    *fd = open ( filepath , flags , perms );
    
    if((*fd)==-1)
    {
        printf("JobExecutorServer : checkfile was not created\n");
    }
}