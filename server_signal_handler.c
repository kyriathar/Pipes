#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "server_job_list.h"
#include "server_signal_handler.h"

/*for running processes*/
extern int currRunning ;
/*flag if sigalarm occured*/
extern int flag_sigalarm ;
/*lists */
extern MotherNode waiting_list ; 
extern MotherNode running_list ;

void hdl (int sig)
{	
    int status ;
    pid_t child_pid;
    /*node list to be removed*/
    Node * node_removed ;
    
    if(sig==SIGRTMIN)
    {
        //printf("alarm :Server will continue reading from pipe !\n");
        flag_sigalarm ++ ;
    }
    if(sig == SIGCHLD)
    {
        node_removed = NULL;
        //printf("CHILD HAS FINISHED !\n");
        //child_pid =wait(&status);
        //
        while ( (child_pid = waitpid(-1, &status, WNOHANG)) > 0) 
        {
            //printf("child %d terminated\n", pid);
            node_removed = findJobAndRemovePid(&running_list,child_pid);
            currRunning --;
            freeNode(node_removed);
        }
        //printf("removing child with pid : %d\n",(int)child_pid);
    }
}


