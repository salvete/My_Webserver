#ifndef HEADERS_H
#define HEADERS_H

/* This are all stadard headers needed for this project */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<time.h>

#define oops(msg,x) {perror(msg);exit(x);} 

#endif
