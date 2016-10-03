#ifndef REST_FUNCTIONS_H
#define	REST_FUNCTIONS_H

#include "server_job_list.h"

int getExecMatrixSize(Node * node);
int passToPipeIssueJob(int fd,Node * node ,char * STATUS);
int passToPipeList( int fd,MotherNode * motherNode );


#endif	/* REST_FUNCTIONS_H */

