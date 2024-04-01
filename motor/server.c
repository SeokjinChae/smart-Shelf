#include "server.h"

void *get_book_id(void *args) {
    struct my_args *_args = (struct my_args *)args;
    id_server_open(_args->serv_sock, _args->clnt_sock, 8888, _args->queue);
}

void *get_book_loc(void *args) {
    struct my_args *_args = (struct my_args *)args;
    loc_server_open(_args->serv_sock, _args->clnt_sock, 8889, _args->queue);	
}

int id_server_open(int *serv_sock, int *clnt_sock, int port, circular_queue **queue) {
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUFSIZ];
    int read_bytes, option = 1;

    *serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(*serv_sock == -1) {
        perror("socket() error");
	    exit(1);
    }

    setsockopt(*serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(*serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1) {
        perror("bind() error");
	    exit(1);
    }

    fprintf(stderr, "[id]\tserver is listening...\n");
    if(listen(*serv_sock, MAX_CLIENT) == -1) {
            perror("listen() error");
	        exit(1);
	}

    if(*clnt_sock < 0){           
            clnt_addr_size = sizeof(clnt_addr);
            *clnt_sock = accept(*serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

            if(*clnt_sock == -1) {
                perror("accept() error");
                exit(1);
	        } 

        fprintf(stderr, "[id]\tclient is accepted\n");
    }
    
    while(read_bytes = read(*clnt_sock, buf, sizeof(buf))) {
        if(read_bytes > 0) {
            enqueue(*queue, buf);

        } else {
            perror("read() error");
            exit(1);
        }
    }
}

int loc_server_open(int *serv_sock, int *clnt_sock, int port, circular_queue **queue) {
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUFSIZ];
    int read_bytes, option;

    *serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(*serv_sock == -1) {
        perror("socket() error");
	    exit(1);
    }

    setsockopt(*serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(*serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1) {
        perror("bind() error");
	    exit(1);
    }

    fprintf(stderr, "[loc]\tserver is listening...\n");
    if(listen(*serv_sock, MAX_CLIENT) == -1) {
            perror("listen() error");
	        exit(1);
	}
	    
    if(*clnt_sock < 0) {           
            clnt_addr_size = sizeof(clnt_addr);
            *clnt_sock = accept(*serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

            if(*clnt_sock == -1) {
                perror("accept() error");  
		    exit(1);
	        } 

	    fprintf(stderr, "[loc]\tclient is accepted\n");
    }
	
    while(read_bytes = read(*clnt_sock, buf, 2 * sizeof(char))) {
        if(read_bytes > 0) {
            if(buf[0] == '2') {
                sprintf(buf, "2");
            } 
            enqueue(*queue, buf);
        } else {
            perror("read() error");
            exit(1);
        }
    }
}

int server_close(int serv_sock, int clnt_sock) {
    close(clnt_sock);
    close(serv_sock);
}

