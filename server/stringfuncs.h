#ifndef STRINGFUNCS_H_INCLUDED
#define STRINGFUNCS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int getcmd(char *cmd, const char *msgIn);
//function to get the command from the data sended from clients
int getdat(char *dat, const char *msgIn);
//function to get the data that not include the command
struct listnode* getRecvCnt(struct sockaddr_in *addr, const char *msgIn);
//function to get the address that need to send data to 
int gettranslatemsg(char *msg, const char *msgIn, const struct sockaddr_in *addrfrom);
//fuction to get the data that can be directly send
int gettranslateid(char *id, const char *msgIn);

#endif // STRINGFUNCS_H_INCLUDED
