#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commander_rest_functions.h"
#include "commander_named_pipe.h"
            
int getNumberOfArguments(int fd)
{
    int returnValue =-1;
    /*number : argc-1*/
    int arg_number ;
    /*how much pipe will read*/
    int str_size;
    /*for strings comming from pipe*/
    char * str;
    
    /*get number of arguments*/
    if(readFromNamedPipe(fd ,(char *)&str_size , sizeof(int) ) == -1)
    {
        perror("Client :getNumberOfArguments: readFromNamedPipe");
    }
    str =(char *)malloc(str_size*sizeof(char));
    memset(str,'\0',str_size*sizeof(char));
    if(readFromNamedPipe(fd , str , str_size )==-1)
    {
        perror("Client :getNumberOfArguments: readFromNamedPipe2");
    }
    arg_number = atoi(str);
    free(str);
    
    returnValue = arg_number ;
    return returnValue ;
}

int getListSize(int fd)
{
    int returnValue =-1;
    /*list size*/
    int list_size ;
    /*how much pipe will read*/
    int str_size;
    /*for strings comming from pipe*/
    char * str;
    
    /*get number of arguments*/
    if(readFromNamedPipe(fd ,(char *)&str_size , sizeof(int) ) == -1)
    {
        perror("Client :getListSize: readFromNamedPipe");
    }
    str =(char *)malloc(str_size*sizeof(char));
    memset(str,'\0',str_size*sizeof(char));
    if(readFromNamedPipe(fd , str , str_size )==-1)
    {
        perror("Client :getListSize: readFromNamedPipe2");
    }
    list_size = atoi(str);
    free(str);
    
    returnValue = list_size ;
    return returnValue ;

}


int fillResultTable(int fd , char ** result_table , int table_size )
{
    int i ;
    int returnValue = -1 ;
    /*how much pipe will read*/
    int str_size;
    /*for strings comming from pipe*/
    char * str;
    
    if(result_table == NULL)
    {
        returnValue = 1;
    }
    else
    {
        for(i=0;i<table_size;i++)
        {
            readFromNamedPipe(fd ,(char *)&str_size , sizeof(int) ) ;
                
            str =(char *)malloc(str_size*sizeof(char)); 
            memset(str,'\0',str_size*sizeof(char));
                
            readFromNamedPipe(fd , str , str_size );
                
            result_table[i]= str ;            
        }
        returnValue = 0 ;
    }
    return returnValue ;
}


int freeResultTable( char ** result_table , int table_size )
{
    int i ;
    int returnValue =-1 ;

    if(result_table == NULL )
    {
        returnValue =1 ;
    }
    else
    {
        for(i=0;i<table_size;i++)
        {
            free(result_table[i]);
        }
        returnValue = 0 ;
        //free result_table out of this function !
    }
    return returnValue ;
}