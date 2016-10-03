#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
# include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
 #include <sys/wait.h>
#include <semaphore.h>

#include "server_signal_handler.h"
#include "server_named_pipe.h"
#include "server_file_manipulation.h"
#include "server_job_list.h"
#include "server_rest_functions.h"

#define PERMS 0644
#define FLAGS O_RDONLY | O_NONBLOCK
#define FLAGS_FOR_SECOND_PIPE O_WRONLY | O_NONBLOCK
#define FLAGS_FOR_FILE O_CREAT | O_RDWR


/*Global variables*/

/*for running processes*/
int currRunning ;
/*flag if sigalarm occured*/
int flag_sigalarm ;                         
/*lists */
MotherNode waiting_list ; 
MotherNode running_list ;

int main(int argc, char** argv) 
{
    int i,j;
    int status ;
    /*fds */
    int fd_check ;                      
    int fd_race ;                       
    int fd_pipe ;
    /*fd for 2nd pipe*/
    int fd_second_pipe ;
    /*how much pipe will read*/
    int str_size;
    /*number : argc-1*/
    int arg_number ;
    /*for IDs */
    int id ;                            
    /*for running processes*/
    int maxRunning ;                    
    /*ID of process to be removed*/
    int id_to_remove ;
    /*node list*/
    Node * node ;                       
    /*node list to be removed*/
    Node * node_removed ;
    /*for atm node*/
    Node * node_atm ;
    /*for jobs*/
    Job * job ;                         
    int returnValue ;                   
    char buffer[32];                    
    char * fifo = "ClientToServerFifo" ;          
    /*Server to Client pipe*/
    char * second_fifo = "ServerToClientFifo";
    /*for strings comming from pipe*/
    char * str;
    /*char * matrix for arguments*/
    char ** chr_matrix ;
    /*char * matrix for execvp*/
    char ** exec_matrix ;               
    char master_buffer[MASTER_BUFFER_SIZE]; 
    struct sigaction act;               
    int client_pid ;
    pid_t server_pid ;                  
    pid_t pid ;                         
    
    /*semaphore*/
    sem_t * m;
    
    printf("SERVER CREATED! \n");
    
    if(argc>=2)
    {
        printf("JobExecutorServer : Invalid Arguments\n");
        exit(1);
    }
    /*Initializing stuff*/
    memset(buffer,'\0',32);
    memset(master_buffer,'\0',MASTER_BUFFER_SIZE);
    memset (&act, '\0', sizeof(act));
    
    act.sa_handler = hdl;
    act.sa_flags = SA_SIGINFO;
    sigfillset(&(act.sa_mask));
    
    maxRunning = 1 ;        //default Concurrency  
    currRunning = 0 ;
    id =0;
    
    initMotherNode(&waiting_list);
    initMotherNode(&running_list);
    
    flag_sigalarm = 0;
    
    id_to_remove = -1 ;
    
    node_removed = NULL ;
    
    m = sem_open("common_sem" , O_CREAT , S_IRUSR|S_IWUSR , 0);
    if(m== SEM_FAILED)
    {
        perror("SERVER : sem_open");
    }
    
    /*creating check file*/
    openFile("checkfile.txt",FLAGS_FOR_FILE,PERMS, &fd_check);
    
    /*write server's pid in check file*/
    server_pid =getpid();
    sprintf(buffer,"%d",server_pid);
    write(fd_check,buffer,32);
    
    /*Create named pipe*/
    createNamedPipe(fifo);
    /*open pipe*/
    openNamedPipe(fifo,FLAGS,&fd_pipe);
    
    /*Server's work*/
    if (sigaction(SIGRTMIN, &act, NULL) < 0) 
    {
        perror ("sigaction");
	return 1;
    }
    if (sigaction(SIGCHLD, &act, NULL) < 0) 
    {
        perror ("sigaction");
	return 1;
    }
    /*reading from pipe once so no error occurs*/
    readFromNamedPipe(fd_pipe , master_buffer , MASTER_BUFFER_SIZE ); 
    /*creating race file*/
    openFile("race.txt",FLAGS_FOR_FILE,PERMS, &fd_race);
    
    
    while(1)
    {
        printf("Server :about to pause\n");
        pause();  
        //sleep(1);
        
        if( flag_sigalarm >= 1 )            
        {
            j=0;
            /*get number of arguments*/
            if(readFromNamedPipe(fd_pipe ,(char *)&str_size , sizeof(int) ) == -1)
            {
                perror("Server : readFromNamedPipe");
            }
            str =(char *)malloc(str_size*sizeof(char));
            memset(str,'\0',str_size*sizeof(char));
            if(readFromNamedPipe(fd_pipe , str , str_size )==-1)
            {
                perror("Server : readFromNamedPipe2");
            }
            arg_number = atoi(str);
            arg_number -- ;
            free(str);
            
            //printf("arg_number = %d\n",arg_number);
            
            chr_matrix = (char **)malloc(arg_number*sizeof(char*));             
            for(i=0;i<arg_number;i++)
                chr_matrix[i] = NULL ;
            
            for(i=0;i<arg_number;i++)
            {
                readFromNamedPipe(fd_pipe ,(char *)&str_size , sizeof(int) ) ;
                
                str =(char *)malloc(str_size*sizeof(char)); 
                memset(str,'\0',str_size*sizeof(char));
                
                readFromNamedPipe(fd_pipe , str , str_size );
                
                chr_matrix[i]= str ;            
            }
            
            /*get client PID*/
            readFromNamedPipe(fd_pipe ,(char *)&str_size , sizeof(int) ) ;
            readFromNamedPipe(fd_pipe ,(char *)&client_pid , sizeof(int) ) ;
            
            printf("client PID = %d\n",client_pid);
            
            /*chr_matrix is ready here*/
            /*printf("CHR_MATRIX:\n");
            for(i=0;i<arg_number;i++)
                printf("%s\n",chr_matrix[i]);
            */
             
            /*picking command*/
            if( strcmp(chr_matrix[0],"issuejob") == 0 )
            {
                /*create exec_matrix  (DEEPCOPY)*/
                exec_matrix = (char **)malloc(arg_number*sizeof(char*));
                for(i=0;i<arg_number;i++)
                        exec_matrix[i] = NULL ;

                for(i=0;i<arg_number-1;i++)
                {
                    exec_matrix[i] =(char *) malloc( (strlen(chr_matrix[i+1])+1)*sizeof(char) );
                    strcpy(exec_matrix[i],chr_matrix[i+1]);
                }
                exec_matrix[arg_number-1] = NULL ;

                /*printf("EXEC_MATRIX:\n");
                for(i=0;i<arg_number-1;i++)
                    printf("%s\n",exec_matrix[i]);
                 */
                
                /*creating Job*/
                job =(Job *) malloc(sizeof(Job));
                initJob(job);
                setId(job,id);
                id++;           //preparing id for next job
                setExecMatrix(job,exec_matrix);
                setPid(job,(pid_t)0);       //process is not yet running so its pid will be 0 

                /*create Node*/
                node = (Node *)malloc(sizeof(Node));
                initNode(node);
                setJob(node,job);
                
                
               
                /*open second pipe for writing...*/
                openNamedPipe(second_fifo,FLAGS_FOR_SECOND_PIPE,&fd_second_pipe);
                /*for at the moment process*/
                node_atm = node ;
                /*check if it will run or not*/
                if(currRunning <maxRunning)
                {
                    //printNode(node_atm);
                    //printf("RUNNING>\n");
                    passToPipeIssueJob(fd_second_pipe,node_atm,"RUNNING");
                }
                else
                {
                    //printNode(node_atm);
                    //printf("QUEUED>\n");
                    passToPipeIssueJob(fd_second_pipe,node_atm,"QUEUED");
                }
                
                 sem_post(m);       //up
                
                close(fd_second_pipe);
                
                /*push Job at the end of the waiting list...*/
                addAtTheEnd(&waiting_list,node);

                //printf("issuejob matched !\n");
            }
            else if( strcmp(chr_matrix[0],"setConcurrency") == 0 )
            {
                //printf("setConcurrency matched !\n");
                maxRunning = atoi(chr_matrix[1]);
            }
            else if( strcmp(chr_matrix[0],"stop") == 0 )
            {
                //printf("stop matched !\n");
                if(strcmp(chr_matrix[1],"all") == 0 )
                {
                    /*first free waiting list*/
                    freeList(&waiting_list);
                    /*empty waiting list*/
                    emptyList(&waiting_list);
                    /*Stop all running Processes*/
                    stopAllJobsFromRunningList(&running_list);
                    /*free running list*/
                    freeList(&running_list);
                    /*empty running list*/
                    emptyList(&running_list);
                }
                else
                {
                    id_to_remove = atoi(chr_matrix[1]);

                    node_removed = findJobAndRemove(&running_list,id_to_remove);

                    if(node_removed != NULL)
                    {
                        /*process is running so we need to stop it */
                        kill(node_removed->job->pid,SIGINT);
                    }
                    else
                    {
                        /*process is not running so check in the waiting_list*/
                        node_removed = findJobAndRemove(&waiting_list,id_to_remove);
                    }

                    if(node_removed == NULL)
                    {
                        printf("job with id : %d doesnt exist in either list\n",id_to_remove);
                    }
                    /*---->free node_removed<------*/
                    freeNode(node_removed);
                    node_removed = NULL ;
                }
            }
            else if( strcmp(chr_matrix[0],"poll") == 0 )        
            {
                //printf("poll matched !\n");
                
                /*open second pipe for writing...*/
                openNamedPipe(second_fifo,FLAGS_FOR_SECOND_PIPE,&fd_second_pipe);
                
                if(strcmp(chr_matrix[1],"running") == 0)
                {
                    //printf("running processes are :\n");
                    //printList(&running_list);
                    
                    passToPipeList(fd_second_pipe,&running_list);
                    
                }
                else if(strcmp(chr_matrix[1],"queued") == 0)
                {
                    //printf("queued processes are :\n");
                    //printList(&waiting_list);
                    passToPipeList(fd_second_pipe,&waiting_list);
                }
                else
                {}
                
                sem_post(m);       //up
                
                close(fd_second_pipe);
                
            }
            else if( strcmp(chr_matrix[0],"exit") == 0 )
            {
                //printf("exit matched !\n");
                
                /*wait until all processes are done running*/
                while( ListIsEmpty(&running_list)==0 )                              
                {
                    printf("Server waiting : there are still running processes\n");
                    pause();
                }
                /*freeing lists*/
                freeList(&waiting_list);
                freeList(&running_list);
                
                /*delete check file*/
                close(fd_check);
                unlink("checkfile.txt");
                /*delete race file*/
                close(fd_race);
                unlink("race.txt");
                /*delete named pipes*/
                close(fd_pipe);
                unlink(fifo);
                close(fd_second_pipe);
                unlink(second_fifo);
		/*delete named semaphore...*/
                sem_close(m);
                sem_unlink("common_sem");
                printf("Server exiting ...\n");
                exit(0);
            }
            else
            {
                printf("Wrong arguments !\n");
            }
            
            /*free chr_matrix*/
            for(i=0;i<arg_number;i++)
            {
                free(chr_matrix[i]);
                chr_matrix[i] = NULL ;
            }
            free(chr_matrix);
            
            flag_sigalarm --;
        }
        /*check if a process can start running*/
        while(currRunning <maxRunning && (ListIsEmpty(&waiting_list)==0))
        {
            /*take from the beginning of the waiting list*/
            node = removeFromStart(&waiting_list);
            /*put her at the end of running list*/
            addAtTheEnd(&running_list,node);
            
            /*run...*/
            /*fork*/
            if((pid = fork())==-1)
            {
                perror("fork");
                exit(1);
            }
            if(pid == 0)    //child -> job
            {
                //printf("child with pid %d will do work now \n",getpid());
                /*execvp*/
                returnValue = execvp(node->job->exec_matrix[0],node->job->exec_matrix);
                if(returnValue ==-1)
                {
                    perror("JobExecutorServer : execvp\n");
                    exit(1);
                }
            }
            else        //Server...
            {
                /*'pid' has chiild's pid*/
                /*need to fill pid at running_list*/
                node->job->pid = pid ;
            }
            currRunning ++;             
        }
        //kill((pid_t)client_pid,SIGRTMIN);
    }
    
    return (EXIT_SUCCESS);
}

