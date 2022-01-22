//
//#define WIN32_LEAN_AND_MEAN //避免冲突#include<windows.h> #include<winsock2.h>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#ifdef _WIN32
//#include<windows.h>
//#include<winsock2.h>
//#else
//#include<unistd.h>
//#include<arpa/inet.h>
//#include<string.h>
//#define SOCKET unsigned int
//#define INVALID_SOCKET (SOCKET)(~0)
//#define SOCKET_ERROR (-1)
//#endif
//#include<stdio.h>
//#include<thread>
//
//enum CMD
//{
//	CMD_LOGIN,
//	CMD_LOGIN_RES,
//	CMD_LOGINOUT,
//	CMD_LOGINOUT_RES,
//	CMD_NEW_USER_JOIN,
//	CMD_ERROR
//};
////消息头
//struct DataHeader
//{
//	short datalength; //数据长度
//	short cmd; //命令
//};
//struct Login : public DataHeader
//{
//	Login()
//	{
//		datalength = sizeof(Login);
//		cmd = CMD_LOGIN;
//	}
//	char useName[32];
//	char passWord[32];
//};
//struct LoginRes : public DataHeader
//{
//	LoginRes()
//	{
//		datalength = sizeof(LoginRes);
//		cmd = CMD_LOGIN_RES;
//		res = 0;
//	}
//	int res;
//};
//struct Loginout : public DataHeader
//{
//	Loginout()
//	{
//		datalength = sizeof(Loginout);
//		cmd = CMD_LOGINOUT;
//	}
//	char useName[32];
//};
//struct newUserJoin :public DataHeader
//{
//	newUserJoin()
//	{
//		datalength = sizeof(newUserJoin);
//		cmd = CMD_NEW_USER_JOIN;
//		sock = 0;
//	}
//	int sock;
//};
//struct LoginoutRes : public DataHeader
//{
//	LoginoutRes()
//	{
//		datalength = sizeof(LoginoutRes);
//		cmd = CMD_LOGINOUT_RES;
//		res = 0;
//	}
//	int res;
//};
//int processor(SOCKET _c_sock)
//{
//	//字节缓冲区
//	char szRecv[1024] = {};
//	// 4.1接收客户端发送的数据
//	int nlen = (int)recv(_c_sock, szRecv, sizeof(DataHeader), 0);
//	DataHeader *header = (DataHeader*)szRecv;
//	if (nlen <= 0)
//	{
//		printf("与服务器断开连接，结束\n");
//		return -1;
//	}
//	switch (header->cmd)
//	{
//	case CMD_LOGIN_RES:
//	{
//
//		recv(_c_sock, szRecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
//		LoginRes* loginRes = (LoginRes*)szRecv;
//		printf("收到服务器消息：CMD_LOGIN_RES,数据长度%d \n",
//			loginRes->datalength);
//	}
//	break;
//	case CMD_LOGINOUT_RES:
//	{
//		recv(_c_sock, szRecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
//		LoginoutRes* loginoutRes = (LoginoutRes*)szRecv;
//		printf("收到服务端消息：CMD_LOGINOUT_RES,数据长度%d \n",
//			loginoutRes->datalength);
//	}
//	break;
//	case CMD_NEW_USER_JOIN:
//	{
//		recv(_c_sock, szRecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
//		newUserJoin* userJoin = (newUserJoin*)szRecv;
//		printf("收到服务端消息：CMD_NEW_USER_JOIN,数据长度%d \n",
//			userJoin->datalength);
//	}
//	break;
//	}
//	return 0;
//}
//bool g_bExit = true;
//void cmdThread(SOCKET _sock)
//{
//	while (g_bExit)
//	{
//		char cmdBuf[256] = {};
//		scanf("%s", cmdBuf);
//		if (0 == strcmp(cmdBuf, "exit"))
//		{
//			printf("退出cmdThread\n");
//			g_bExit = false;
//			break;
//		}
//		else if (0 == strcmp(cmdBuf, "login"))
//		{
//			Login login;
//			strcpy(login.useName, "jiar");
//			strcpy(login.passWord, "1234");
//			send(_sock, (const char *)&login, sizeof(Login), 0);
//		}
//		else if (0 == strcmp(cmdBuf, "loginout"))
//		{
//			Loginout loginout;
//			strcpy(loginout.useName, "jiar");
//			send(_sock, (const char *)&loginout, sizeof(Loginout), 0);
//		}
//		else
//		{
//			printf("不支持的命令\n");
//		}
//	}
//
//
//}
//
//int main()
//{
//#ifdef _WIN32
//	WORD ver = MAKEWORD(2, 2);
//	WSADATA dat;
//	WSAStartup(ver, &dat); //启动windos socket 环境 还需要依赖库ws2_32.lib
//#endif
//
//	//用socket API建立简易TCP客户端
//	//1.建立一个socket
//	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (INVALID_SOCKET == _sock)
//	{
//		printf("建立socket失败\n");
//	}
//	else
//	{
//		printf("建立socket成功\n");
//	}
//	//2.连接服务器
//	sockaddr_in _sin = {};
//	_sin.sin_family = AF_INET;
//	_sin.sin_port = htons(4567);
//#ifdef _WIN32
//	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
//#else
//	_sin.sin_addr.s_addr = inet_addr("192.168.83.1");
//#endif
//	int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
//	if (SOCKET_ERROR == ret)
//	{
//		printf("连接服务器失败\n");
//	}
//	else
//	{
//		printf("连接服务器成功\n");
//	}
//	//启动线程
//	std::thread t1(cmdThread, _sock);
//	t1.detach();
//	while (g_bExit)
//	{
//		fd_set fdReads;
//		FD_ZERO(&fdReads);
//		FD_SET(_sock, &fdReads);
//		timeval t = { 3, 0 };
//		int ret = select(_sock + 1, &fdReads, 0, 0, &t);
//		if (ret < 0)
//		{
//			printf("select任务结束1\n");
//			break;
//		}
//		if (FD_ISSET(_sock, &fdReads))
//		{
//			FD_CLR(_sock, &fdReads);
//			if (-1 == processor(_sock))
//			{
//				printf("select任务结束2\n");
//				break;
//			}
//		}
//
//		//printf("空闲时间处理其他业务\n");
//
//		//Sleep(1000);
//	}
//
//	//4.关闭socket
//#ifdef _WIN32
//	closesocket(_sock);
//	WSACleanup(); //关闭windos socket 环境
//#else
//	close(_sock);
//#endif
//	printf("任务结束\n");
//	getchar();
//	return 0;
//}