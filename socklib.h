#ifndef SOCKLIB_H
#define SOCKLIB_H


/* for the server */
int make_server_socket(int);
int make_server_socket_q(int,int);

/* for the client */
int connect_to_server(char *,int);


#endif

