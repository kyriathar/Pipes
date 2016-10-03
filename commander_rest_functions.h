#ifndef REST_FUNCTIONS_H
#define	REST_FUNCTIONS_H

int getNumberOfArguments(int fd);
int getListSize(int fd);
int fillResultTable(int fd , char ** result_table , int table_size );

int freeResultTable( char ** result_table , int table_size );

#endif	/* REST_FUNCTIONS_H */

