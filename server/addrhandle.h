#include <netinet/in.h>

void cpyaddr(struct sockaddr_in *newaddr, const struct sockaddr_in* oldaddr);
int isaddrequal(const struct sockaddr_in *addr1,const struct sockaddr_in* addr2);