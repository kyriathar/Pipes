#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server_rest_functions.h"
#include "server_job_list.h"

int getExecMatrixSize(Node * node)
{
    int i;
    int returnValue = -1 ;
    
    if(node == NULL)
    {
        printf("getExecMatrixSize : node == NULL\n");
        returnValue =1 ;
    }
    else
    {
        i =0 ;
        while(node->job->exec_matrix[i]!= NULL)
            i++;
    }
    returnValue = i ;
    return returnValue ;
}

int passToPipeIssueJob(int fd,Node * node ,char * STATUS)
{
    int i ;
    int args ;
    int returnValue =-1;
    int size_of_buffer ;
    int execMatrixSize ;
    char * str;
    
    if(node == NULL)
    {
        printf("passToPipe : node == NULL\n");
        returnValue =1 ;
    }
    else
    {
        execMatrixSize = getExecMatrixSize(node) ;
        args = execMatrixSize + 2 ;    // +2 stands for id and RUNNING/QUEUED
        
        str =(char *) malloc(3*sizeof(char));           //args can be up to 99...            
        memset(str,'\0',3*sizeof(char));
        sprintf (str, "%d",args);                     
        size_of_buffer = strlen(str)+1 ;
        writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd,str,size_of_buffer);
        free(str);
        
        /*id*/
        str =(char *) malloc(10*sizeof(char));           
        memset(str,'\0',10*sizeof(char));
        sprintf (str, "%d",node->job->id);
        size_of_buffer = strlen(str)+1 ;
        writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd,str,size_of_buffer);
        free(str);
        /*ExecMatrix*/
        for (i=0; i<execMatrixSize; i++)
        {
            size_of_buffer = strlen(node->job->exec_matrix[i])+1;
            str =(char *) malloc(size_of_buffer*sizeof(char));
            memset(str,'\0',size_of_buffer*sizeof(char));
            strcpy(str, node->job->exec_matrix[i]);
            
            /*write to pipe*/
            writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
            writeToNamedPipe(fd,str,size_of_buffer);
            free(str);
        }
        /*STATUS*/
        size_of_buffer = strlen(STATUS)+1;
        str =(char *) malloc(size_of_buffer*sizeof(char));
        memset(str,'\0',size_of_buffer*sizeof(char));
        strcpy(str, STATUS);
        
        /*write to pipe*/
        writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd,str,size_of_buffer);
        free(str);
        
        returnValue = 0 ;
    }
    return returnValue ;    
}

int passToPipeList( int fd,MotherNode * motherNode )
{
    int i ;
    int args ;
    int returnValue =-1;
    int size_of_buffer ;
    int execMatrixSize ;
    char * str;
    Node * temp ;
    
    if(motherNode == NULL )
    {
        returnValue = 1;
    }
    else
    {
        /*1st passing list size*/
        str =(char *) malloc(10*sizeof(char));                   
        memset(str,'\0',10*sizeof(char));
        sprintf (str, "%d",motherNode->size);                     
        size_of_buffer = strlen(str)+1 ;
        writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
        writeToNamedPipe(fd,str,size_of_buffer);
        free(str);
        
        temp = motherNode->first;
        while(temp!=NULL)
        {
            execMatrixSize = getExecMatrixSize(temp) ;
            args = execMatrixSize + 1 ;    // +1 stands for id 

            str =(char *) malloc(3*sizeof(char));           //args can be up to 99...            
            memset(str,'\0',3*sizeof(char));
            sprintf (str, "%d",args);                     
            size_of_buffer = strlen(str)+1 ;
            writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
            writeToNamedPipe(fd,str,size_of_buffer);
            free(str);

            /*id*/
            str =(char *) malloc(10*sizeof(char));           
            memset(str,'\0',10*sizeof(char));
            sprintf (str, "%d",temp->job->id);
            size_of_buffer = strlen(str)+1 ;
            writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
            writeToNamedPipe(fd,str,size_of_buffer);
            free(str);
            /*ExecMatrix*/
            for (i=0; i<execMatrixSize; i++)
            {
                size_of_buffer = strlen(temp->job->exec_matrix[i])+1;
                str =(char *) malloc(size_of_buffer*sizeof(char));
                memset(str,'\0',size_of_buffer*sizeof(char));
                strcpy(str, temp->job->exec_matrix[i]);

                /*write to pipe*/
                writeToNamedPipe(fd,(char *)&size_of_buffer,sizeof(int));
                writeToNamedPipe(fd,str,size_of_buffer);
                free(str);
            }
            
            temp = temp->next;
        }
    }
    return returnValue ;
}