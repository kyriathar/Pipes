#include <stdio.h>
#include <stdlib.h>
# include <fcntl.h>

#include "commander_named_pipe.h"

void createNamedPipe (char * name)
{
    int errorCode ;
    
    errorCode = mkfifo ( name , 0666);
    if ( errorCode == -1 )
    {
        perror (" Server : mkfifo " ) ;
        //exit (1) ;   dimiourgei provlima
    }
    return ;
}

void openNamedPipe( char * name , int flags ,int * fd)
{
    if(fd == NULL)
    {
        printf("Commander : fd = NULL\n");
        exit(1);
    }
    *fd = open ( name ,flags );
    if ( (*fd) < 0 )
    { 
        perror ( " Commander : fifo open error " ); 
        //exit (1) ; 
    }
}

int readFromNamedPipe(int fd ,char * buffer ,int buffer_size )
{
    int returnValue ;
    
    returnValue = read ( fd , buffer , buffer_size);
    if ( returnValue == -1)
    { 
        perror ( "Client Error in Reading ") ; 
        //exit (1) ; 
    }
    return returnValue;
}

void writeToNamedPipe(int fd , char * buffer ,int buffer_size )
{
    int errorCode ;
    
    errorCode = write ( fd , buffer , buffer_size);
    if ( errorCode == -1)
    { 
        perror ( "Client Error in Writing ") ; 
        exit (1) ; 
    }
}
