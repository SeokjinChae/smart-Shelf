#ifndef SERVER_H
#define SERVER_H

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "queue.h"

#define MAX_CLIENT		1

struct my_args {
	int *serv_sock;
	int *clnt_sock;
	circular_queue **queue;
};

void *get_book_id(void *args);
void *get_book_loc(void *args);
int id_server_open(int *serv_sock, int *clnt_sock, int port, circular_queue **queue);
int loc_server_open(int *serv_sock, int *clnt_sock, int port, circular_queue **queue);
int server_close(int serv_sock, int clnt_sock);

#endif
