#include "threadporc.h"
#include "defvars.h"
#include "stringfuncs.h"
#include "clientlist.h"
#include "addrhandle.h"

int ispnatlive = 0;
int isplstlive = 0;

void *procRecv(void *p){

    int sMain;                                  //socket to send and receive data
    SOCKADDR_IN addrser;                        //local address information 
    SOCKADDR_IN addrcnt;                        //address information of the client sending message
    SOCKADDR_IN addrTo;                         //address information of the translate client
	int addrlen, datalen;						//to store the data length and the length of the address
	char data[MAXDATA];							//data buffer to save the data sended by client
	char cmd[21];								//the buffer to save the command separated from the data
	char msg[MAXDATA];							//the buffer to save the message separated from the data
    struct timeval waittime = {30, 0};			//waiting time of the socket

    struct vars pin;							//the structure to pass parameter to the new thread
    pthread_t pt_nat, pt_lst;

	list_client = (pNODE)malloc(sizeof(NODE));	//初始化链表
	setlistempty(list_client);					//清空链表

	/*------------------注意,以下代码仅供测试---------------------------*/
	//	addsometestiteam(list_client);
	/*------------------注意,以上代码仅供测试---------------------------*/

    printf("1>creating socket...\n");
    sMain = socket(AF_INET, SOCK_DGRAM, 0);		//create the socket main
	if(sMain == -1){
        printf("1>failed to create socket main:%d\n", errno);
        return (void*)-1;
	}
	printf("1>Success.\n");

    addrser.sin_addr.s_addr = INADDR_ANY;		//setting local address information
    addrser.sin_family = AF_INET;
    addrser.sin_port = htons(PORT_SER);

	printf("1>binding socket and address...\n");
	if(bind(sMain, (struct sockaddr*)&addrser, sizeof(addrser))){	//binding the main socket and the address
		printf("1>failed to binding main socket and local address:%d\n", errno);
        return (void*)-1;
	}
	printf("1>Success.\n");

	//为了防止第一次接受到信息是,判断客户端地址为0.0.0.0:0的情况,需要设置原本的大小为struct sockaddr
	addrlen = sizeof(struct sockaddr);

	//get into the message loop
	printf("1>Begin to receive data from clients.\n");
    while(1){
        memset(data, 0, MAXDATA);
        memset(cmd, 0, 21);
        datalen = recvfrom(sMain, data, MAXDATA, 0, (struct sockaddr*)&addrcnt, (socklen_t*)&addrlen);
        if(datalen<0) continue;
		//因为直接利用inet_ntoa()输出ip地址会出错,所以现在利用变量 msg 来暂时储存地址信息
		memset(msg, 0, MAXDATA);
		strcpy(msg, inet_ntoa(addrcnt.sin_addr));
		//打印地址信息已经从该地址发送过来的信息
		printf("[%s:%d]::%s\n", msg, ntohs(addrcnt.sin_port), data);
		//清空msg缓冲区
		memset(msg, 0, MAXDATA);

		//从接收到的数据中取得命令
		if(getcmd(cmd, data)==0) printf("CMD:>[%s]\n", cmd);	//get command in the data
		//判断命令是什么
        if(strcmp(cmd, COMMAND_LOGIN) == 0){					//handle the command
			
			//如果进程不在运行
			if(!ispnatlive){
				//从登录消息中取得ID
				if(getdat(msg, data))continue;
				//判断列表中是否有存在的用户
				if(findclient(list_client, msg)){
					printf("Login: this ID has been took up:%s\n", msg);
					//给客户端发送重复登录的消息
					sendto(sMain, COMMAND_REPEAT_ID, strlen(COMMAND_REPEAT_ID), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
					//跳过此次登录
					continue;
				}
				//设置传入线程的变量
				setvars(&pin, &addrcnt,msg);
				//创建一个线程,并传入当前登录的用户的地址
				pthread_create(&pt_nat, NULL, procNAT, (void*)&pin);
				//给用户返回确认NAT类型的消息,让用户与判断NAT类型的线程通信
				sendto(sMain, COMMAND_CONFIRM_TYPE, strlen(COMMAND_CONFIRM_TYPE), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
			}else{			//如果在队列之外
				sendto(sMain, COMMAND_SERVER_BUSY, strlen(COMMAND_SERVER_BUSY), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
				//给客户端发送服务器繁忙的消息
			}

        }else if(strcmp(cmd, COMMAND_GET_CLIENTS) == 0){
			
			//判断进程是否运行
			if(isplstlive){
				//如果在运行,则返回服务器繁忙
				sendto(sMain, COMMAND_SERVER_BUSY, strlen(COMMAND_SERVER_BUSY), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
			}else{
				//如果不在运行,则开启线程
				//取得用户id, 存放在msg中
				if(getdat(msg, data))continue;
				//取得发送命令的客户的地址信息
				setvars(&pin, &addrcnt, msg);
				//向用户发送开始返回列表的消息,让客户端发送信息给获取列表的端口
				sendto(sMain, COMMAND_LIST_BEGIN, strlen(COMMAND_LIST_BEGIN), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
				pthread_create(&pt_lst, NULL, procList, (void*)&pin);
			}
			
        }else if(strcmp(cmd, COMMAND_HELPSEND_TO) == 0){
			//接收到的消息格式 COMMAND=HELPSEND=TO=toID|fromID|message
			
			//清空id缓冲区
			memset(msg, 0, MAXDATA);
			//取得转发放的id
			if(gettranslateid(msg, data)){
				printf("Translate: get id error.\n");
				continue;
			}
			//取得要发送的客户端,msg为id
            if(getRecvCnt(&addrTo, msg) == (pNODE)1){
				//取得真正要发送的数据
				gettranslatemsg(msg, data, &addrcnt);
				//给接收方转发信息,消息格式为: COMMAND=TRANSLATE===fromID|message
				sendto(sMain, msg, strlen(msg), 0, (struct sockaddr*)&addrTo, sizeof(addrTo));
            }else{
				//如果列表中不存在,则返回失败
				sendto(sMain, COMMAND_BACK_FAILED, strlen(COMMAND_BACK_FAILED), 0, (struct sockaddr*)&addrTo, sizeof(addrTo));
			}
        }else if(strcmp(cmd, COMMAND_LOGOUT) == 0){
			//清空缓冲区
			memset(msg, 0, MAXDATA);
			//取得注销id
			if(getdat(msg, data))continue;
			printf("LOGOUT: got msg:%s\n", msg);
			//判断用户ID是否存在
			if(!findclient(list_client, msg)){
				printf("LOGOUT: ID not founded:%s\n", msg);
				continue;
			}
			//删除节点
			delclient(list_client, msg);
		}

    }

	close(sMain);

	return NULL;
}

void *procListen(void *p){
	//这个线程是用来判断客户端是否掉线,如果掉线则将其从客户端中踢出
	/*
	定义变量:
		用于接受信息的套接口:scntltn;
		本地地址信息结构体:addrlocal
		用于接收消息的地质结构体:addrcnt;
		用于存放信息的数组变量:data[MAXCMD]
		用于存放地址长度的变量:addrlen
		用于存放数据长度的变量:datalen
		用于遍历客户列表的迭代器:pt
	*/

	int scntltn;
	struct sockaddr_in addrlocal;
	struct sockaddr_in addrcnt;
	char data[MAXCMD];
	int addrlen, datalen;
	pNODE list_listen;
	pNODE pt;

	//创建套接口
	scntltn = socket(AF_INET, SOCK_DGRAM, 0);
	//设置地址信息
	addrlocal.sin_family = AF_INET;
	addrlocal.sin_addr.s_addr = INADDR_ANY;
	addrlocal.sin_port = htons(PORT_CNT);
	//绑定套接口和地址信息
	if(bind(scntltn, (struct sockaddr*)&addrlocal, sizeof(addrlocal))){
		printf("CNTLISTEN: Error in binding socket:%d\n", errno);
		exit(0);	//如果线程开启不成功,则退出整个应用.
	}

	//当收到一个

	//进入消息循环,每隔一段时间判断一次客户端在线情况
	while(1){
		if(islistempty(list_client)){//如果列表为空则跳过此次循环
			continue;
		}
		//将迭代器设置到列标头
		pt = list_client;
		//进入循环列表,当到达列表底部时跳出循环
		while(pt != NULL){
			//获取当前节点的客户端地址
			
			//给列表中的当前客户端发送监听命令
			//清空缓冲区
			//等待客户端返回
			//判断是否超时
				//如果超时则将该节点删除
			//迭代器迭代,继续下一个循环
		}

	}


	return NULL;
}

void *procCmd(void *p){

    pthread_t htData = *(pthread_t*)p;      //to save the handle of the data receive thread.

    int sRecv;                              //to save the socket to receive command from local control server.
    struct sockaddr_in addrCMD;             //to save address information that send command
    struct sockaddr_in addrSelf;            //to save local address information.
    char message[MAXCMD];                   //the buffer to save the command
	char data[MAXDATA];						//the buffer to store data
	pNODE pt;								//递归器
    int cmdlen, addrlen;                    //to save length of the command data and address length
	struct vars pin;
	struct timeval tlisttimeout = {30,0};
	int ttimes = 0;

    puts("2>Creating socket...");
    sRecv = socket(AF_INET, SOCK_DGRAM, 0); //create the socket to receive the command
    puts("2>Success!");

    puts("2>Setting local address...");
    addrSelf.sin_addr.s_addr = inet_addr("127.0.0.1");      //only the local can send the command
    addrSelf.sin_family = AF_INET;
    addrSelf.sin_port = htons(PORT_CMD);                    //set the command receive port
    puts("2>Success!");

    puts("2>Binding socket and local address...");
    if(bind(sRecv, (struct sockaddr *)&addrSelf, sizeof(addrSelf))){           //bind the socket and address information
        printf("2>Error in binding socket and address >>%d\n", errno);
        close(sRecv);
        pthread_cancel(htData);                             //close the socket and stop the thread that receive data
        return (void*)(-1);
    }
    puts("2>Success!");

	//设置超时时间：
	setsockopt(sRecv, SOL_SOCKET, SO_RCVTIMEO, (void*)&tlisttimeout, (socklen_t)sizeof(tlisttimeout));

    puts("2>Begin to receive the command");
    while(1){
        memset(message, 0, MAXCMD);                            //clear the buffer to store the command
        cmdlen = recvfrom(sRecv, message, MAXCMD-1, 0, (struct sockaddr *)&addrCMD, (socklen_t*)&addrlen);
        if(cmdlen<0)continue;
        message[cmdlen] = 0x00;
        printf("2>%s\n", message);

        if(strcmp(message, "list")==0){
			//本地输出将要列出客户列表的提示
            printf("2>begin to list the clients\n");
			//本地输出客户列表
			printlist(list_client);
			//给控制端发送开始传输的命令
			sendto(sRecv, COMMAND_LIST_BEGIN, strlen(COMMAND_LIST_BEGIN), 0, (struct sockaddr *)&addrCMD, sizeof(addrCMD));
			//清空缓冲区data
			memset(message, 0, MAXCMD);
			//等待控制端返回准备好的命令
			cmdlen = recvfrom(sRecv, message, MAXCMD, 0, (struct sockaddr*)&addrCMD, (socklen_t*)&addrlen);
			//判断是否超时
			if(cmdlen <0){
				printf("List: timeout.\n");
				continue;
			}
			//判断返回命令是否错误
			if(strcmp(message, COMMAND_READY) != 0){
				printf("List: Return Error.\n");
				continue;
			}
			//判断列表是否为空
			if(islistempty(list_client)){
				//如果为空则给控制端返回列表空指令
				sendto(sRecv, COMMAND_LIST_EMPTY, strlen(COMMAND_LIST_EMPTY), 0, (struct sockaddr*)&addrCMD, sizeof(addrCMD));
				//并退出此次循环
				continue;
			}
			//将递归器pt指向链表头
			pt = list_client;
			//进入发送循环,直到到达尾部停止
			do{
				//清空缓冲区
				memset(data, 0, MAXDATA);
				//将要发送的数据按照格式放入缓冲区
				sprintf(data, "%s%s:%d[%s]", COMMAND_LIST_DATA, pt->ip, pt->port, pt->clientID);
				//此处是测试将要发送的数据格式正确与否
				puts(data);
				//发送数据
				sendto(sRecv, data, strlen(data), 0, (struct sockaddr*)&addrCMD, sizeof(addrCMD));
				//等待客户端反馈,时间为两分钟
				while((cmdlen = recvfrom(sRecv, message, MAXCMD, 0, (struct sockaddr*)&addrCMD, (socklen_t*)&addrlen)) < 0){
					if(ttimes++ == 4){
						printf("List: lost connect.\n");
						ttimes = 0;
						break;
					}
				}
				//判断控制端反馈是否正确
				if(strcmp(message, COMMAND_READY) != 0){
					printf("List: Response Error.\n");
					break;
				}
				if(cmdlen<0)break;
				//链表向前行进
				pt = pt->next;
			}while(pt != NULL);
			//循环结束,给控制端发送传输结束命令
			if(cmdlen>=0){//如果控制端没有失去连接
				sendto(sRecv, COMMAND_LIST_END, strlen(COMMAND_LIST_END), 0, (struct sockaddr*)&addrCMD, sizeof(addrCMD));
			}

        }else if(strcmp(message, "exitserver")==0){
            close(sRecv);
            pthread_cancel(htData);
            return (void*)(-1);
        }
    }

    return NULL;
}

void *procNAT(void *p){
	/*
	定义变量:
		套接口, snat
		存放本机地址信息的结构
		用于从主进程传入参数中取出并存放的地址结构
		用于存放接受信息时的客户地址
		数据缓冲区,数据长度
		地址结构长度
		超时时间结构体
	*/
	int snat; 
	struct sockaddr_in addrnat;
	struct sockaddr_in addrcnt;
	struct sockaddr_in addrtmp;
	char data[MAXCMD];
	char idnew[21];
	char idin[21];
	int datlen, addrlen;
	int nattpye;
	struct timeval ttimeout = {10, 0};

	//线程活动标志为1
	ispnatlive = 1;
	//创建套接口
	snat = socket(AF_INET, SOCK_DGRAM, 0);
	//从传入参数中取得信息
	memset(idin, 0, 21);
	strncpy(idin, ((struct vars*)p)->msg, strlen(((struct vars*)p)->msg));
	cpyaddr(&addrcnt, &((struct vars*)p)->addrcnt);
	//设置地址信息,ip为传入结构的ip
	addrnat.sin_family = AF_INET;
	addrnat.sin_addr.s_addr = INADDR_ANY;
	addrnat.sin_port = htons(PORT_NAT);
	//绑定套接口和本地地址
	if(bind(snat, (struct sockaddr*)&addrnat, sizeof(addrnat))){
		printf("Error in bind socket and address:%d\n", errno);
		ispnatlive = 0;
		return NULL;
	}

	//设置套接口超时时间
	setsockopt(snat, SOL_SOCKET, SO_RCVTIMEO, (void*)&ttimeout, sizeof(ttimeout));

	//设置原本的addrlen,防止第一次接受信息是地址为空
	addrlen = sizeof(struct sockaddr);

//重新接收信息标签
rerecv:
	//清空缓冲区
	memset(data, 0, MAXCMD);

	//等待客户端发送信息
	datlen = recvfrom(snat, data, MAXCMD, 0, (struct sockaddr*)&addrtmp, (socklen_t*)&addrlen);
	//判断是否超时
	if(datlen<0){
		printf("NAT:Time out.\n");
		close(snat);
		ispnatlive = 0;
		return NULL;
	}
	//取得再次传进来的用户id
	memset(idnew, 0, 21);
	getdat(idnew, data);
	//判断是否为同一个客户
	if(strcmp(idin, idnew) != 0){
		printf("NAT: ID error[in:%s;get:%s]\n", idin, idnew);
		goto rerecv;
	}
	//取得命令
	getcmd(data, data);
	//判断返回命令是否正确 COMMAND_READY
	if(strcmp(data, COMMAND_READY) != 0){
		printf("NAT: Responsed error.\n");
		close(snat);
		ispnatlive = 0;
		return NULL;
	}
	//如果接收正确,进行输出
	printf("NAT: Responsed success.\n");

	/*以下是判断nat类型的,现在还没有进行实现*/

	//TODO: 判断nat类型的过程
	
	/*以上是判断nat类型的,现在还没有进行实现*/

	//个体客户端返回登录成功消息,返回值为命令加上nat类型,目前因为没有实现nat类型的判断,所以先统一返回4(NATTYPE_SYMMETRIC)
	nattpye = NATTYPE_SYMMETRIC;
	//设置要返回的信息
	memset(data, 0, MAXCMD);
	sprintf(data, "%s%d", COMMAND_BACK_SUCCESS, nattpye);
	//将信息发送回去
	sendto(snat, data, strlen(data), 0, (struct sockaddr*)&addrtmp, (socklen_t)sizeof(addrtmp));
	//讲客户端信息添加到用户列表中
	addclient(list_client, inet_ntoa(addrcnt.sin_addr), ntohs(addrcnt.sin_port), idin, nattpye);
	//关闭套接口
	close(snat);
	//线程活动标志为0
	ispnatlive = 0;
	return NULL;
}

void *procList(void *p){
	//设置进程运行信号为1(运行)
	isplstlive = 1;
	/*建立变量:
		套接口:slst
		本地地址变量:addrlst
		传入地址变量:addrcnt
		接受缓存地址变量:addrtmp
		数据缓冲区:data[MAXDATA]
		传入id:idin[21]
		接受到的id:idnew[21]
		接收数据大小:datlen
		接受地址大小:addrlen
		超时时间结构体:trecvout
		客户列表递归器:pt
	*/
	int slst;
	struct sockaddr_in addrlst;
	struct sockaddr_in addrcnt;
	struct sockaddr_in addrtmp;
	char data[MAXDATA];
	char idnew[21];
	char idin[21];
	int datlen, addrlen;
	pNODE pt = list_client;
	struct timeval trecvout = {10 ,0};
	
	//取出传入的客户端地址
	cpyaddr(&addrcnt, &((struct vars*)p)->addrcnt);
	//取出传入的id
	strcpy(idin, ((struct vars*)p)->msg);
	//建立套接口
	slst = socket(AF_INET, SOCK_DGRAM, 0);
	//设置本地地址
	addrlst.sin_addr.s_addr = INADDR_ANY;
	addrlst.sin_family = AF_INET;
	addrlst.sin_port = htons(PORT_LST);
	//绑定套接口和本地地址
	if(bind(slst, (struct sockaddr*)&addrlst, sizeof(addrlst))){
		printf("LST: error in bind socket:%d\n", errno);
		isplstlive = 0;
		return NULL;
	}
	//设置套接口超时时间
	setsockopt(slst, SOL_SOCKET, SO_RCVTIMEO, (void*)&trecvout, (socklen_t)sizeof(trecvout));

	//设置addrlen初始长度,防止第一次接受时,客户端地址为空
	addrlen = sizeof(struct sockaddr);
	
//重新接受标签
rerecvlabel:
	//清空缓冲区
	memset(data, 0, MAXDATA);
	
	//等待用户返回准备好的消息
	datlen = recvfrom(slst, data, MAXDATA, 0, (struct sockaddr*)&addrtmp, (socklen_t*)&addrlen);
	//判断用户返回是否超时
	if(datlen<0){
		printf("LST: Time out.\n");
		close(slst);	//关闭套接口
		isplstlive = 0;
		return NULL;
	}
	//取出传入的id
	getdat(idnew, data);
	//判断用户是否为传入用户,不是则返回等待接受处
	if(strcmp(idin, idnew) != 0){
		printf("LST: Error client[in:%s;get:%s]\n", idin, idnew);
		goto rerecvlabel;
	}
	//获取命令
	getcmd(data, data);
	//判断用户返回是否正确
	if(strcmp(data, COMMAND_READY) != 0){
		printf("LST: response error:%s\n", data);
		close(slst);
		isplstlive = 0;
		return NULL;
	}
	/*接下来进行列表信息的传输*/
	//判断列表是为空
	if(islistempty(pt)){
		//为空,则给客户端返回空列表的指令
		sendto(slst, COMMAND_LIST_EMPTY, strlen(COMMAND_LIST_EMPTY), 0, (struct sockaddr*)&addrtmp, sizeof(addrtmp));
	}else{
		//不为空,则进入发送循环,从头开始,将在线客户端的信息发送给请求客户
		do{
			//清空缓冲区
			memset(data, 0, MAXDATA);
			//按照一定格式COMMAND_LIST_DATA + IP + : + PORT + [ + ID + ] + nattpye 将客户端列表放入缓冲区
			sprintf(data, "%s%s:%d:%s:%d", COMMAND_LIST_DATA, pt->ip, pt->port, pt->clientID, pt->nattype);
			//本地输出发送的消息
			puts(data);
			//给请求端发送数据
			sendto(slst, data, strlen(data), 0, (struct sockaddr*)&addrtmp, sizeof(addrtmp));

		recvlabel2:
			//清空缓冲区
			memset(data, 0, MAXDATA);
			//设置套接口超时时间
			setsockopt(slst, SOL_SOCKET, SO_RCVTIMEO, (void*)&trecvout, (socklen_t)sizeof(trecvout));
			//等待请求端应答 COMMAND_READY
			datlen = recvfrom(slst, data, MAXDATA, 0, (struct sockaddr*)&addrtmp, (socklen_t*)&addrlen);
			//判断是否超时
			if(datlen<0){
				printf("LST: Time out: receiving:%d\n", errno);
				close(slst);
				isplstlive = 0;
				return NULL;
			}
			//获取ID和命令
			getdat(idnew, data);
			getcmd(data, data);
			//判断用户是否正确
			if(strcmp(idin, idnew) != 0){
				printf("LST: Error client[in:%s;get:%s]\n", idin, idnew);
				goto recvlabel2;
			}
			//判断返回值是否正确
			if(strcmp(data, COMMAND_READY) != 0){
				printf("LST: response error:%s\n", data);
				close(slst);
				isplstlive = 0;
				return NULL;
			}
			//迭代器迭代
			pt = pt->next;
		}while(pt != NULL);//循环结束条件为到达列表底部
		//输出列表结束
		puts(COMMAND_LIST_END);
		//返回传输结束进程
		sendto(slst, COMMAND_LIST_END, strlen(COMMAND_LIST_END), 0, (struct sockaddr*)&addrcnt, sizeof(addrcnt));
	}
	
	//关闭套接口
	close(slst);
	//设置进程运行信号为0(不允许)
	isplstlive = 0;
	return NULL;
}

void setvars(struct vars *p, const SOCKADDR_IN* addrcnt, const char * id){
	strcpy(p->msg, id);
    p->addrcnt.sin_addr = addrcnt->sin_addr;
    p->addrcnt.sin_family = addrcnt->sin_family;
    p->addrcnt.sin_port = addrcnt->sin_port;
}

