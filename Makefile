OBJ_COMMANDER	= commander_main.o commander_named_pipe.o commander_rest_functions.o commander_signal_handler.o
OBJ_SERVER	= server_file_manipulation.o server_job_list.o server_main.o server_named_pipe.o server_rest_functions.o server_signal_handler.o 
CC		= gcc
FLAGS		= -c

all: program1 program2

program1: $(OBJ_COMMANDER)
	$(CC) $(OBJ_COMMANDER) -o jobcommander -lpthread

program2: $(OBJ_SERVER)
	$(CC) $(OBJ_SERVER) -o jobexecutorserver -lpthread

commander_main.o: commander_main.c
	$(CC) $(FLAGS) commander_main.c

commander_named_pipe.o: commander_named_pipe.c
	$(CC) $(FLAGS) commander_named_pipe.c

commander_rest_functions.o: commander_rest_functions.c
	$(CC) $(FLAGS) commander_rest_functions.c

commander_signal_handler.o: commander_signal_handler.c
	$(CC) $(FLAGS) commander_signal_handler.c

server_file_manipulation.o: server_file_manipulation.c
	$(CC) $(FLAGS) server_file_manipulation.c

server_job_list.o: server_job_list.c
	$(CC) $(FLAGS) server_job_list.c

server_main.o: server_main.c
	$(CC) $(FLAGS) server_main.c

server_named_pipe.o: server_named_pipe.c
	$(CC) $(FLAGS) server_named_pipe.c

server_rest_functions.o: server_rest_functions.c
	$(CC) $(FLAGS) server_rest_functions.c

server_signal_handler.o: server_signal_handler.c
	$(CC) $(FLAGS) server_signal_handler.c


clean:
	rm -f $(OBJ_COMMANDER) $(OBJ_SERVER) jobcommander jobexecutorserver
