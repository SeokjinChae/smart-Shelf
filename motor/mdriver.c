#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "move.h"
#include "server.h"

#define DEBUG			1

#define MAX_BOOK_NUM	3
#define MAX_USR_INPT	20
#define ID_LENGTH		5

char my_books[MAX_BOOK_NUM][ID_LENGTH];
char user_input[MAX_USR_INPT];
circular_queue *id_queue, *loc_queue;

int find_move() {
	for(int i = 0; i < MAX_BOOK_NUM; i++) {
		if(!strcmp(my_books[i], user_input)) {
			move_motor(i);
			fprintf(stderr, "i: %d\n", i);
			return i;
		}
	}

	fprintf(stderr, "There is no book \"%s\"\n", user_input);
	return -1;
}

void *get_user_input() {
	while(1) {
		fgets(user_input, ID_LENGTH * sizeof(char), stdin);
		find_move();
	}
}

void *mapping() {
	char *loc, *id;
	int _loc, __loc;
	
	while(1) {

		// fetch the location that wait to be handled.
		loc = dequeue(loc_queue);
		_loc = atoi(loc);

		if(_loc > 0) {
			id = dequeue(id_queue);
			strcpy(my_books[_loc - 1], id);

			#if DEBUG
				fprintf(stderr, "[insert]\tlocation: %d, id: %s\n", _loc, id);
			#endif

		} else {
			__loc = abs(_loc);
			strcpy(my_books[__loc - 1], "");

			#if DEBUG
				fprintf(stderr, "[delete]\tlocation: %d, id: %s\n", __loc, id);
			#endif
		}
	}
}

int main(int argc, char **argv) {
	pthread_t user_thr, bookid_thr, bookloc_thr, map_thr;
	int id_serv_sock, id_clnt_sock = -1, loc_serv_sock, loc_clnt_sock = -1, status;

	struct my_args id_args = {
		.serv_sock = &id_serv_sock, 
		.clnt_sock = &id_clnt_sock, 
		.queue = &id_queue
		};

	struct my_args loc_args = {
		.serv_sock = &loc_serv_sock, 
		.clnt_sock = &loc_clnt_sock, 
		.queue = &loc_queue
		};

	create_queue(&id_queue, MAX_QUEUE_SIZE);
	create_queue(&loc_queue, MAX_QUEUE_SIZE);
	 
	if(pthread_create(&user_thr, NULL, get_user_input, NULL) < 0) {
		perror("user thread create error");
		exit(1);
	}
	
	if(pthread_create(&bookid_thr, NULL, get_book_id, (void *)&id_args) < 0) {
		perror("id thread create error: ");
		exit(1);
	}

	if(pthread_create(&bookloc_thr, NULL, get_book_loc, (void *)&loc_args) < 0) {
		perror("loc thread create error: ");
		exit(1);
	}

	if(pthread_create(&map_thr, NULL, mapping, NULL) < 0) {
		perror("map thread create error: ");
		exit(1);
	}

	while(1);
}
