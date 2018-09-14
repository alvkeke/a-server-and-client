#include "addrhandle.h"

void cpyaddr(struct sockaddr_in *newaddr, const struct sockaddr_in* oldaddr){
	newaddr->sin_addr = oldaddr->sin_addr;
	newaddr->sin_port = oldaddr->sin_port;
	newaddr->sin_family = oldaddr->sin_family;
}
int isaddrequal(const struct sockaddr_in *addr1,const struct sockaddr_in* addr2){
	//判断地址是否相同
	if(addr1->sin_addr.s_addr == addr2->sin_addr.s_addr){
		//判断端口是否相同
		if(addr1->sin_port == addr2->sin_port){
			//判断协议族是否相同
			if(addr1->sin_family == addr1->sin_family){
				return 1;
			}
		}
	}
	return 0;
}