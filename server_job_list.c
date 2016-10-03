#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server_job_list.h"

/*Job*/
int initJob(Job * job)
{
    int returnValue =-1 ;
    
    if(job == NULL)
    {
        printf("initJob : job == NULL\n");
        returnValue = 1;
    }
    else
    {
        job->id = -1 ;
        job->exec_matrix = NULL ;
        job->pid = 0 ;
        
        returnValue = SUCCESS ;
    }
    return returnValue ;
}

int setId(Job * job ,int id)
{
    int returnValue =-1 ;
    
    if(job == NULL)
    {
        printf("setId : job == NULL\n");
        returnValue = 1;
    }
    else
    {
        job->id = id ;
        returnValue = SUCCESS ;
    }
    return returnValue ;
}
int setExecMatrix(Job * job ,char ** exec_matrix)
{
    int returnValue =-1 ;
    
    if(job == NULL || exec_matrix == NULL)
    {
        printf("setExecMatrix : job == NULL or exec_matrix == NULL\n");
        returnValue = 1;
    }
    else
    {
        job->exec_matrix = exec_matrix ;
        returnValue = SUCCESS ;
    }
    return returnValue ;
}
int setPid(Job * job , pid_t pid)
{
    int returnValue =-1 ;
    
    if(job == NULL)
    {
        printf("setPid : job == NULL\n");
        returnValue = 1;
    }
    else
    {
        job->pid = pid ;
        returnValue = SUCCESS ;
    }
    return returnValue ;
}

/*Node*/
int initNode(Node * node)
{
    int returnValue =-1;
    
    if(node == NULL)
    {
        printf("initNode : node == NULL\n");
        returnValue = 1;
    }
    else
    {
        node->job = NULL ;
        node->next = NULL ;
        returnValue = SUCCESS ;
    }
    return returnValue ;
}

int setJob(Node * node , Job * job)
{
    int returnValue =-1;
    
    if(node == NULL || job == NULL)
    {
        printf("setJob : node == NULL or job == NULL\n");
        returnValue = 1;
    }
    else
    {
        node->job = job ;
       returnValue = SUCCESS ;
    }
    return returnValue ;
}

/*MotherNode*/
int initMotherNode(MotherNode * motherNode)
{
    int returnValue =-1;
    
    if(motherNode == NULL)
    {
        printf("initMotherNode : motherNode == NULL\n");
        returnValue = 1;
    }
    else
    {
        motherNode->first = NULL ;
        motherNode->last = NULL ;
        motherNode->size = 0;
        returnValue = SUCCESS ;
    }
    return returnValue ;
}

/*List*/
int addAtTheEnd(MotherNode * motherNode,Node * node)
{
    int returnValue = -1;
    
    if(motherNode == NULL || node == NULL )
    {
        printf("addToTheEnd : motherNode == NULL or node == NULL\n");
        returnValue = 1 ;
    }
    else
    {
        if(motherNode->first == NULL)
        {
            /*list is empty*/
            motherNode->first = node ;
            motherNode->last =node ;
            motherNode->last->next = NULL ;
            motherNode->size ++ ;
        }
        else
        {
            motherNode->last->next = node ;
            motherNode->last->next->next = NULL ;
            /*update last*/
            motherNode->last = motherNode->last->next ;
            motherNode->size ++ ;
        }
    }
    return returnValue ;
}

Node * removeFromStart(MotherNode * motherNode)
{
    Node * temp = NULL ;
    if(motherNode == NULL)
    {
        printf("removeFromStart : motherNode == NULL\n");
        return NULL ;
    }
    else
    {
        if(motherNode->first == NULL)
        {
            /*empty list*/
            return NULL ;
        }
        else
        {
            temp = motherNode->first ;
            /*update motherNodes's head*/
            motherNode->first = motherNode->first->next ;
            if(motherNode->first == NULL)
            {
                /*list has one element only so we need to fix last also*/
                motherNode->last = NULL ;
            }
            motherNode->size -- ;
            return temp ;
        }
    }
    return NULL ;  //perito... 
}


Node * findJobAndRemove(MotherNode * motherNode ,int id)
{
    Node * temp = NULL ;
    Node * previous = NULL ;
    if(motherNode == NULL)
    {
        printf("findJobAndRemove : motherNode == NULL\n");
        return NULL ;
    }
    else
    {
        temp = motherNode->first;
        if(temp == NULL)
        {
            printf("findJobAndRemove : list is empty\n");
            return NULL ;
        }
        else
        {
            previous = temp ;
            if(temp->job->id == id )
            {
                /*update head*/
                motherNode->first = temp->next ;
                if(motherNode->first == NULL)
                {
                    /*list has one element only so we need to fix last also*/
                    motherNode->last = NULL ;
                }
                motherNode->size -- ;
                return temp ;
            }
            
            while(temp->next != NULL)
            {
                temp = temp->next ;
                if(temp->job->id == id )
                {
                    previous->next = temp->next ;
                    if(temp->next == NULL)
                    {
                        /*this is the last element of the list so i need to update motherNode*/
                        motherNode->last = previous ;
                    }
                    motherNode->size -- ;
                    return temp ;
                }
                previous = temp ;
            }
        }
    }
    return NULL ;
}

Node * findJobAndRemovePid(MotherNode * motherNode ,pid_t pid)
{
    Node * temp = NULL ;
    Node * previous = NULL ;
    if(motherNode == NULL)
    {
        printf("findJobAndRemovePid : motherNode == NULL\n");
        return NULL ;
    }
    else
    {
        temp = motherNode->first;
        if(temp == NULL)
        {
            printf("findJobAndRemovePid : list is empty\n");
            return NULL ;
        }
        else
        {
            previous = temp ;
            if(temp->job->pid == pid )
            {
                /*update head*/
                motherNode->first = temp->next ;
                if(motherNode->first == NULL)
                {
                    /*list has one element only so we need to fix last also*/
                    motherNode->last = NULL ;
                }
                motherNode->size -- ;
                return temp ;
            }
            
            while(temp->next != NULL)
            {
                temp = temp->next ;
                if(temp->job->pid == pid )
                {
                    previous->next = temp->next ;
                    if(temp->next == NULL)
                    {
                        /*this is the last element of the list so i need to update motherNode*/
                        motherNode->last = previous ;
                    }
                    motherNode->size -- ;
                    return temp ;
                }
                previous = temp ;
            }
        }
    }
    return NULL ;    
}

int ListIsEmpty(MotherNode * motherNode)
{
    if(motherNode == NULL)
    {
        printf("ListIsEmpty : motherNode == NULL\n");
        return -1 ;
    }
    else
    {
        if(motherNode->first == NULL)
            return 1 ;
        else
            return 0 ;
    }
}

void printNode(Node * node)
{
    int i ;
    if(node == NULL)
    {
        printf("printNode : node == NULL\n");
        return ;
    }
    else
    {
        printf("<%d",node->job->id);
        i=0;
        while(node->job->exec_matrix[i]!= NULL)  
        {
            printf(",%s",node->job->exec_matrix[i]);
            i++;
        }
        printf(",");
    }
}

void printList(MotherNode * motherNode)
{
    int i ;
    Node * temp = NULL;
    if(motherNode == NULL)
    {
        printf("printList : motherNode == NULL\n");
        return ;
    }
    else
    {
        temp = motherNode->first ;
        
        if(temp == NULL)
        {
            printf("printList : this list is empty\n");
            return ;
        }
        while(temp!=NULL)
        {
            
            printf("<%d",temp->job->id);
            i=0;
            while(temp->job->exec_matrix[i]!= NULL)  
            {
                printf(",%s",temp->job->exec_matrix[i]);
                i++;
            }
            printf(">\n");
            temp = temp->next ;
        }
    }
}

/*empty-removes*/
int emptyList(MotherNode * motherNode)
{
    int returnValue =-1;
    
    if(motherNode == NULL)
    {
        returnValue =1 ;
    }
    else
    {
        motherNode->first = NULL;
        motherNode->last = NULL ;
        motherNode->size = 0;
        returnValue =0 ;
    }
    return returnValue;
}

int stopAllJobsFromRunningList(MotherNode * motherNode)
{
    int returnValue =-1;
    Node * temp = NULL;
    
    if(motherNode == NULL)
    {
        returnValue =1 ;
    }
    else
    {
        temp =motherNode->first;
        while(temp != NULL)
        {
            kill(temp->job->pid,SIGINT);
            temp = temp->next ;
        }
        returnValue =0;
    }
    return returnValue ;
}

/*FREEs*/
void freeString(char * str)
{
    if(str == NULL)
        printf("freeString : str == NULL\n");
    else
        free(str);
    return ;
}

void freeExecMatrix(char ** exec_matrix)
{
    int i;
    
    if(exec_matrix == NULL)
        printf("freeExecMatrix : exec_matrix == NULL\n");
    else
    {
        i=0;
        while(exec_matrix[i]!=NULL)
        {
            freeString(exec_matrix[i]);
            i++;
        }
        free(exec_matrix);
    }
    return ;
}

void freeJob(Job * job)
{
    if(job == NULL)
        printf("freeJob : job == NULL\n");
    else
    {
        freeExecMatrix(job->exec_matrix);
        free(job);
    }
    return;
}

void freeNode(Node * node)
{
    if(node == NULL)
        printf("freeNode : node == NULL\n");
    else
    {
        freeJob(node->job);
        free(node);
    }
    return ;
}

void freeList(MotherNode * motherNode)
{
    Node * temp = NULL;
    Node * to_be_deleted = NULL ;
    if(motherNode == NULL)
        printf("freeList : motherNode == NULL\n");
    else
    {
        temp = motherNode->first;
        if(temp == NULL)
            printf("freeList : list is empty \n");
        else
        {
            /*list is not empty*/
            while(temp != NULL)
            {
                to_be_deleted = temp ;
                temp = temp->next ;
                freeNode(to_be_deleted);
            }
            /*motherNode is at stack ==> no free*/
        }
    }
    return ;
}