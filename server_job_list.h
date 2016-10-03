#ifndef JOB_LIST_H
#define	JOB_LIST_H

#include <sys/types.h>

#define SUCCESS 0

typedef struct Job Job ;
typedef struct Node Node ;
typedef struct MotherNode MotherNode;

struct Job {
    int id ;
    char ** exec_matrix ;
    pid_t pid ;
    pid_t client_pid ;
};

struct Node {
    Job * job ;
    Node * next ;
};

struct MotherNode {
    Node * first ;
    Node * last ;
    int size ;
    /*extra info might be needed here*/
};

/*Job*/
int initJob(Job * job);
int setId(Job * job ,int id);
int setExecMatrix(Job * job ,char ** exec_matrix);
int setPid(Job * job , pid_t pid);

int deleteJob(Job * job);       

/*Node*/
int initNode(Node * node);
int setJob(Node * node , Job * job);
int deleteNode(Node * node);    

/*MotherNode*/
int initMotherNode(MotherNode * motherNode);

/*List*/
int addAtTheEnd(MotherNode * motherNode,Node * node);
Node * removeFromStart(MotherNode * motherNode);
Node * findJobAndRemove(MotherNode * motherNode ,int id);
Node * findJobAndRemovePid(MotherNode * motherNode ,pid_t pid);
int ListIsEmpty(MotherNode * motherNode);

void printNode(Node * node);
void printList(MotherNode * motherNode);

/*empty-removes*/
int emptyList(MotherNode * motherNode);
int stopAllJobsFromRunningList(MotherNode * motherNode);

/*FREEs*/
void freeString(char * str);
void freeExecMatrix(char ** exec_matrix);
void freeJob(Job * job);
void freeNode(Node * node);

void freeList(MotherNode * motherNode);

#endif	/* JOB_LIST_H */

