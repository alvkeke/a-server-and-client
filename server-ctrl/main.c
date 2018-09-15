#include "main.h"
#include "defvars.h"


int main()
{

    int sSend;
    struct sockaddr_in addrSer;
    struct sockaddr_in addrSelf;
    struct sockaddr_in addrtmp;
    int addrtmp_len, datlen;
    char message[MAXDATA];
    char scmd[MAXSTR];
    char sanswer[3];

    struct timeval timeout_recv = {30,0};

    sSend = socket(AF_INET, SOCK_DGRAM, 0);

    addrSelf.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrSelf.sin_family = AF_INET;
    addrSelf.sin_port = htons(PORT);

    if(bind(sSend, &addrSelf, sizeof(addrSelf))){
        printf("Error in binding socket and address:%d\n", errno);
        return -1;
    }

    setsockopt(sSend, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_recv, sizeof(timeout_recv));

    addrSer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrSer.sin_port = htons(PORT_SERCMD);
    addrSer.sin_family = AF_INET;

    while(1){
        printf(">");
        memset(scmd, '\0', MAXSTR);
        fgets(scmd, MAXSTR-1, stdin);
        while(scmd[strlen(scmd)- 1] == '\n')
            scmd[strlen(scmd)-1] = 0;
        if(strcmp(scmd, "exit")==0)break;               //exit control server but not stop the main server

        if(strcmp(scmd, "exitserver") == 0){
            printf("Are you sure to exit the main server?[y/N]:");
            fgets(sanswer, 3, stdin);
            if(strcmp(sanswer, "y\n")==0 || strcmp(sanswer, "Y\n")==0){
                sendto(sSend, scmd, sizeof(scmd), 0, &addrSer, sizeof(addrSer));
                break;
            }
        }else if(strcmp(scmd, "restart") == 0){
            printf("Are you sure to restart the main server?[y/N]:");
            fgets(sanswer, 3, stdin);
            if(strcmp(sanswer, "y\n")==0 || strcmp(sanswer, "Y\n")==0){
                sendto(sSend, scmd, sizeof(scmd), 0, &addrSer, sizeof(addrSer));

                if(recvfrom(sSend, message, MAXDATA-1, 0, &addrtmp, &addrtmp_len)>0 &&
                   strcmp(sSend, COMMAND_BACK_SUCCESS)==0)
                        printf("Success to restart the main server.\n");
                else
                    printf("Failed to restart the main server.\n");
            }
        }else if(strcmp(scmd, "list")){
			sendto(sSend, scmd, sizeof(scmd), 0, &addrSer, sizeof(addrSer));
			datlen = recvfrom(sSend, message, MAXDATA, 0, (struct sockaddr*)&addrtmp, &addrtmp_len);
			if(datlen < 0){
				printf("List: time out.\n");
				continue;
			}
			if(strcmp(message, COMMAND_LIST_BEGIN) == 0){
                do{
					sendto(sSend, COMMAND_READY, strlen(COMMAND_READY), 0, (struct sockaddr*)&addrSer, sizeof(addrSer));
					datlen = recvfrom(sSend, message, MAXDATA, 0, (struct sockaddr*)&addrtmp, (socklen_t*)&addrtmp_len);
					if(datlen < 0){
						printf("List: lost connect.\n");
					}
					puts(datlen);
                }while(strcmp(message, COMMAND_LIST_END) != 0);

			}
		}else{
            sendto(sSend, scmd, sizeof(scmd), 0, &addrSer, sizeof(addrSer));
        }


    }

    close(sSend);

    return 0;
}
