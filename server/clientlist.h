#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef CREATE_LISTNODE
#define CREATE_LISTNODE

#define NODE_IP_LENGTH 16
#define NODE_ID_LENGTH 20

typedef struct listnode{
	char ip[NODE_IP_LENGTH];
	unsigned int port;
	char clientID[NODE_ID_LENGTH];
	int nattype;
	struct listnode* next;
}NODE;

typedef struct listnode* pNODE;


#endif

void addclient(NODE* plist, char *ip, unsigned int port, char *ID, int nattype);
NODE* addclient_n(NODE* plist, NODE* n_add);
NODE* findclient(NODE* plist, const char *ID);
int findclient_n(NODE* plist, NODE* n_find);
NODE* delclient(NODE* plist, char *ID);
NODE* delclient_n(NODE* plist, NODE* n_del);

void setlistempty(NODE* plist);
int islistempty(NODE* plist);
void printlist(NODE* plist);

//此函数用于添加测试用的客户端列表
//void addsometestiteam(pNODE p);