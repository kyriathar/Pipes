#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
# include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>

#include "commander_named_pipe.h"
#include "commander_signal_handler.h"
#include "commander_rest_functions.h"

#define PERMS 0644
#define MASTER_BUFFER_SIZE 128
#define FLAGS O_WRONLY | O_NONBLOCK
#define FLAGS_FOR_SECOND_PIPE O_RDONLY | O_NONBLOCK

int flag ;

int main(int argc, char** argv) 
{
    int i,j;
    /*for id print at result table*/
    int id;
    int status ;
    int size_of_buffer ;
    /*size of the list*/
    int list_size ;
    /*number : argc-1*/
    int arg_number ;
    int fd_pipe ;
    /*fd for 2nd pipe*/
    int fd_second_pipe ;
    int fd_check ;
    int fd_race ;
    int returnValue ;
    char buffer[32];
    char * str;
    /*Client to Server pipe*/
    char * fifo = "ClientToServerFifo" ;
    /*Server to Client pipe*/
    char * second_fifo = "ServerToClientFifo";
    /*result table*/
    char ** result_table ;
    int server_pid ;
    int client_pid ;
    pid_t pid ;
    //struct sigaction act;  
    
    /*semaphore*/
    sem_t * m;
    
    //printf("COMMANDER CREATED!\n");
    
    if(argc<2)
    {
        printf("JobCommander : Invalid Arguments\n");
        exit(1);
    }
    /*Initializing stuff*/
    memset(buffer,'\0',32);
    str = NULL ;
    
    /*act.sa_handler = hdl;
    act.sa_flags = SA_SIGINFO;
    sigfillset(&(act.sa_mask));
    */

    flag =0;
    
    m = sem_open("common_sem" , O_CREAT , S_IRUSR|S_IWUSR , 0);
    if(m== SEM_FAILED)
    {
        perror("SERVER : sem_open");
    }
    
   /* if (sigaction(SIGRTMIN, &act, NULL) < 0) 
    {
        perror ("sigaction");
        return 1;
    }*/
        
    /*attempt to open check file*/
    fd_check = open ( "checkfile.txt" , O_RDONLY , PERMS );
    
    /*close check file*/
    close(fd_check);
    
    if(fd_check == - 1)     //file does not exit => create Server
    {
        /*Create second named pipe*/
        createNamedPipe(second_fifo);
        
        if((pid = fork())==-1)
        {
            perror("fork");
            exit(1);
        }

        if(pid == 0)     //PAIDI -> Server
        {
           returnValue = execl ( "jobexecutorserver" ,"jobexecutorserver", NULL ) ;
           if(returnValue ==-1)
           {
               perror("JobCommander : execl\n");
               exit(1);
           }
        }
    }
    //Kwdikas Patera (JobCommander)
    
    /*fixing race problem for first Commander and server...*/
    /*...attempt to open check file*/
    while((fd_race = open ( "race.txt" , O_RDONLY , PERMS )) == -1)
    {
        //printf("---client waiting for server---\n");
       /* sleep(1);*/          //DANGER
    }
    close(fd_race);
    
    /*open pipe*/
    openNamedPipe(fifo,FLAGS,&fd_pipe);
    
    /*passing stuff to pipe...*/
    /*...first the number of arguments...*/
    if(argc != 1)
    {
        
        str =(char *) malloc(3*sizeof(char));           //argc can be up to 99...            
        memset(str,'\0',3*sizeof(char));
        sprintf (str, "%d",argc);                     //get arguments-count without executable + clientpid
        size_of_buffer = strlen(str)+1 ;
        writeToNamedPipe(fd_pipe,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd_pipe,str,size_of_buffer);
        free(str);
        /*...then the arguments*/
        for (i=1; i<argc; i++)
        {
            size_of_buffer = strlen(argv[i])+1;
            str =(char *) malloc(size_of_buffer*sizeof(char));
            memset(str,'\0',size_of_buffer*sizeof(char));
            strcpy(str, argv[i]);
            
            /*write to pipe*/
            writeToNamedPipe(fd_pipe,(char *)&size_of_buffer,sizeof(int));
            writeToNamedPipe(fd_pipe,str,size_of_buffer);
            free(str);
        }
        /*client pid at the end*/
        client_pid = getpid();
        size_of_buffer = 1;
        writeToNamedPipe(fd_pipe,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd_pipe,(char *)&client_pid,sizeof(int));
    }
        
    /*send signal for Server to gather pipe stuff...*/
    /*...that will be SIGALRM*/
    fd_check = open ( "checkfile.txt" , O_RDONLY , PERMS );
    if(fd_check == - 1)     //file does not exit 
    {
        perror("Commander : open2");
    }
    /*get server's pid*/
    returnValue = read ( fd_check , buffer , 32);
    
    server_pid = atoi(buffer);
    
    /*open second pipe and wait...*/
    openNamedPipe(second_fifo,FLAGS_FOR_SECOND_PIPE,&fd_second_pipe);
    
    kill((pid_t)server_pid,SIGRTMIN);
    
    close(fd_pipe);
    
    /*will wait for data from Server here*/
    
    if( strcmp(argv[1],"issuejob")==0 )
    {
        printf("COMMANDER PAUSED\n");
        sem_wait(m);                //down
        
        /*while(flag ==0 )
            pause();
        */
        printf("COMMANDER UNPAUSED\n");
        /*start reading*/
        arg_number = getNumberOfArguments(fd_second_pipe) ;
        
        result_table = (char **)malloc(arg_number*sizeof(char*));
        for(i=0;i<arg_number;i++)
                result_table[i] = NULL ;
        
        fillResultTable(fd_second_pipe,result_table,arg_number);
        
        /*print */
        id = atoi(result_table[0]);
        printf("<%d",id);
        /*reading from 1 because id is already printed above for better printing lookwise*/
        for(i=1;i<arg_number;i++)
        {
            printf(",%s",result_table[i]);
        }
        printf(">\n");
        
        /*FREE result table*/
        freeResultTable(result_table,arg_number);
        free(result_table);
        
    }
    if(strcmp(argv[1],"poll")==0 )
    {
        printf("COMMANDER PAUSED\n");
        sem_wait(m);                //down
        /*while(flag ==0 )
            pause();
        */
        
        printf("COMMANDER UNPAUSED\n");
        /*start reading*/
        
        list_size = getListSize(fd_second_pipe);    
        
        if(strcmp(argv[2],"running") == 0)
        {
            printf("running processes are :\n");
	    if(list_size == 0)            
		printf("<NONE>\n");

            for(j=0;j<list_size;j++)
            {
                arg_number = getNumberOfArguments(fd_second_pipe) ;

                result_table = (char **)malloc(arg_number*sizeof(char*));
                for(i=0;i<arg_number;i++)
                        result_table[i] = NULL ;

                fillResultTable(fd_second_pipe,result_table,arg_number);

                /*print */
                id = atoi(result_table[0]);
                printf("<%d",id);

                for(i=1;i<arg_number;i++)
                {
                    printf(",%s",result_table[i]);
                }
                printf(">\n");

                /*FREE result table*/
                freeResultTable(result_table,arg_number);
                free(result_table);
            
            }
        }
        else if(strcmp(argv[2],"queued") == 0)
        {
            printf("queued processes are :\n");
	    if(list_size == 0)            
		printf("<NONE>\n");
            
            for(j=0;j<list_size;j++)
            {
                arg_number = getNumberOfArguments(fd_second_pipe) ;

                result_table = (char **)malloc(arg_number*sizeof(char*));
                for(i=0;i<arg_number;i++)
                        result_table[i] = NULL ;

                fillResultTable(fd_second_pipe,result_table,arg_number);

                /*print */
                id = atoi(result_table[0]);
                printf("<%d",id);

                for(i=1;i<arg_number;i++)
                {
                    printf(",%s",result_table[i]);
                }
                printf(">\n");

                /*FREE result table*/
                freeResultTable(result_table,arg_number);
                free(result_table);
            }    
        }
        else
            printf("3rd argument is wrong \n");
    }
    
     close(fd_second_pipe);
     sem_close(m);
    
    /*the first client is going to wait for Server*/
   // wait();
        
    
    printf("COMMANDER  %d exiting\n",getpid());
    
    return (EXIT_SUCCESS);
}
