#ifndef THREADPORC_H_INCLUDED
#define THREADPORC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>

#include <string.h>
#include <sys/time.h>

#include <unistd.h>

#define MAXDATA 1024

struct vars{
	char msg[MAXDATA];
	struct sockaddr_in addrcnt;
};

void *procRecv(void *p);
void *procCmd(void *p);
void *procNAT(void *p);
void *procList(void *p);
void *procListen(void *p);

void setvars(struct vars *p, const struct sockaddr_in* addrcnt, const char * id);

#endif // THREADPORC_H_INCLUDED
