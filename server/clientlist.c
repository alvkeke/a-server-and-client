#include "clientlist.h"

/*
//本函数用于添加测试用的客户端列表
void addsometestiteam(pNODE p){
	addclient(p, "192.168.0.156", 1235, "id1", 4);
	addclient(p, "192.168.1.156", 5234, "id2", 4);
	addclient(p, "192.222.4.567", 134, "id3", 4);
	addclient(p, "45.234.66.17", 7676, "id4", 4);
	addclient(p, "34.65.0.65", 4567, "id5", 4);
	printlist(p);
}
*/

void addclient(NODE* plist, char *ip, unsigned int port, char *ID, int nattype){
	pNODE pt = plist;
	//pointer to go through the list
	pNODE pin;
	//pointer to add the data in the list
	if(islistempty(plist)){
		plist->port = port;
		//set the port into the pointer
		strcpy(plist->ip, ip);
		//set the ip into the pointer
		strcpy(plist->clientID, ID);
		//set the nat type
		plist->nattype = nattype;
		//set the ID into the pointer
		plist->next = NULL;
	}else{
		pin = (pNODE)malloc(sizeof(NODE));
		pin->port = port;
		//set the port into the pointer
		strcpy(pin->ip, ip);
		//set the ip into the pointer
		strcpy(pin->clientID, ID);
		//set the ID into the pointer
		pin->nattype = nattype;
		//set the nat type
		while(pt->next != NULL)pt = pt->next;
		//goto the end of the list
		pt->next = pin;
		//add the pointer to the list
		pin->next = NULL;
		//set a flag for finding the end of the list		
	}

}

NODE* addclient_n(NODE* plist, NODE* n_add){
	pNODE pt = plist;
	if(islistempty(plist))
	{
		return n_add;
	}
	//pointer to go through the list
	while(pt->next != NULL)pt = pt->next;
	//goto the end of the list
	pt->next = n_add;
	//add the node to the list
	n_add->next = NULL;
	//set a flag for finding the end
	return plist;
}

NODE* findclient(NODE* plist, const char *ID){
//get the pnode of the client
	pNODE pt = plist;
	//pointer to go through the list
	if(islistempty(plist))return NULL;//如果列表为空,则返回-1
	while(strcmp(pt->clientID, ID) != 0){
	//finding
		if(pt->next == NULL)return NULL;//如果列表中没有,则返回NULL(0)
		//reached the end
		pt = pt->next;
		//++
	}
	return pt;
}

int findclient_n(NODE* plist, NODE* n_find){
//get the position of the client
	int t = 0;
	//position of the node
	pNODE pt = plist;

	if(islistempty(plist))return -1;

	while(pt != n_find){
		t++;
		if(pt->next == NULL)return -1;
		pt = pt->next;
	}
	return t;
}

NODE* delclient(NODE* plist, char *ID){
	pNODE pt = plist;
	//iterator
	pNODE ptmp;
	//temp
	if(strcmp(plist->clientID, ID)==0){
		if(plist->next == NULL)//列表中只有一个元素，清空列表
		{
			setlistempty(plist);	
		}else{
			//if the target was the head of the list
			ptmp = plist->next;
			//save the next node
			free(plist);
			//free the head
			return ptmp;
			//return the 2ed node			
		}

	}else{//else
		while(strcmp(pt->next->clientID, ID)!=0){
			//find the target
			if(pt->next == NULL)return (pNODE)-1;
			//not founded, return error
			pt = pt->next;//go on
		}
		if(pt->next->next!=NULL){
			//founded, and not the end of the list
			ptmp = pt->next;
			pt->next = ptmp->next;
			free(ptmp);
		}else{
			//target was the end of the list
			free(pt->next);
			//free the memory
			pt->next = NULL;
		}
	}
	
	return plist;//return the original head
}

NODE* delclient_n(NODE* plist, NODE* n_del){
	pNODE pt = plist;							//the pointer to go through the list
	pNODE ptmp;									//the pointer to save the target temply
	if(plist == n_del){							//if the head is the target
		if(plist->next == NULL){				//列表只有一个元素，则清空列表
			setlistempty(plist);				//清空列表
		}else{
			return plist->next;					//return the new head			
		}
	}else{										//the head is not the target
		while(pt->next != n_del){
			if(pt->next == NULL)return (pNODE)-1;//exit when reach the end of the list
			pt = pt->next;						//if not end, continue
		}										//get out of the loop when find the you want to delete
		if(pt->next->next!=NULL){				//if the node is not the end of the list
			ptmp = pt->next;					//save the target temply
			pt->next = ptmp->next;				//remove the node
			free(ptmp);							//free the memory
		}else{									//the node is the end of the list
			free(pt->next);						//free the memory and remove it from the list
			pt->next = NULL;
		}
	}
	return plist;
}

void setlistempty(NODE* plist){
	memset(plist->ip, 0, NODE_IP_LENGTH);
	memset(plist->clientID, 0, NODE_ID_LENGTH);
	plist->port = 0;
	plist->next = NULL;
}

int islistempty(NODE* plist){
	if(strcmp(plist->ip, "") == 0){
		if(strcmp(plist->clientID, "") == 0){
			if(plist->port == 0){
				if(plist->next == NULL){
					return 1;
				}
			}
		}
	}
	return 0;
}

void printlist(NODE* plist){
	pNODE pt = plist;
	if(islistempty(plist)){
		printf("List is empty!\n");
		//exit the list process
		return;
	}
	puts("\n==========begin to print list==========");
	while(pt != NULL){
		printf("Client ID:%s\n", pt->clientID);
		printf("Address:[%s:%d]\n", pt->ip, pt->port);
		printf("-------\n");
		pt = pt->next;
	}
	puts("================= end ==================\n");

}