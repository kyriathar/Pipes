#ifndef NAMED_PIPE_H
#define	NAMED_PIPE_H

void createNamedPipe (char * name);
void openNamedPipe( char * name , int flags ,int * fd);
int readFromNamedPipe(int fd ,char * buffer ,int buffer_size );
void writeToNamedPipe(int fd , char * buffer ,int buffer_size );

#endif	/* NAMED_PIPE_H */

