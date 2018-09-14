#include "stringfuncs.h"
#include "defvars.h"
#include "addrhandle.h"


int getcmd(char *cmd, const char *msgIn){
	if(strlen(msgIn)<20){						//if the data is too short, return error -1 and exit the function
			printf("Error:Data is too short:%d\n", strlen(msgIn));
			return -1;					
	}

	strncpy(cmd, msgIn, 20);					//get the command in the data, and put it into the string 'cmd'
	*(cmd+20) = '\0';							//set the end of the string

	return 0;									//return no error .
}

int getdat(char *dat, const char *msgIn){		
	if(strlen(msgIn)<20){						//if the data is too short, return error -1 and exit the function
		printf("Error:Data is too short:%d\n", strlen(msgIn));
		return -1;								
	}
    strncpy(dat, msgIn+20, strlen(msgIn)-20);	//set the data into the char pointer witch was pass in
    dat[strlen(msgIn)-20] = '\0';				//set the end of the string
	return 0;	
}

pNODE getRecvCnt(struct sockaddr_in *addr, const char *msgIn){

    //TODO: 通过传入的数据取得相应的id, 再通过id寻找到相应的客户端地址,将其返回

	/*
	变量定义
		储存id的字符串
		分隔符位置变量
		列表节点指针
	*/
	pNODE pt;

	//通过id查找用户所在节点
	pt = findclient(list_client, msgIn);
	if(pt<=0)return pt;
	//通过节点将数据复制到传入的结构体中
	addr->sin_port = htons(pt->port);
	addr->sin_addr.s_addr = inet_addr(pt->ip);
	addr->sin_family = AF_INET;
	//返回值为0
	return 1;
}

int gettranslatemsg(char *msg, const char *msgIn, const struct sockaddr_in *addrfrom){
	//COMMAND_HELPSEND_TO11111|22222|message
	char *firdivider;
	char *secdivider;
	char msgreal[MAXDATA];
	char fromID[21];
	//清空缓冲区
    memset(msg, 0, MAXDATA);
	memset(fromID, 0, 21);
	//取得第一个分隔符
	firdivider = strchr(msgIn, '|');
	//取得第二个分隔符
	secdivider = strchr(firdivider+1, '|');
	//取得发送方id
	strncpy(fromID, firdivider + 1, secdivider - firdivider - 1);
	//取得真正要显示的消息
	strncpy(msgreal, secdivider +1, strlen(msgIn) - (size_t)secdivider + (size_t)msgIn);
	//打包数据
	sprintf(msg, "%s%s|%s", COMMAND_TRANSLATE, fromID, msgreal);
	return 0;
}


int gettranslateid(char *id, const char *msgIn){

	/*
	定义变量
		id分隔符:divider
	*/
	char *divider;
	//查找分隔符,判断是否出错
	if((divider = strchr(msgIn, '|')) < 0)return -1;
	strncpy(id, msgIn + 20, (int)(divider - msgIn - 20));
	return 0;
}