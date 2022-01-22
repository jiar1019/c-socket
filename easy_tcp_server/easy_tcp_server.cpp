

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN //避免冲突#include<windows.h> #include<winsock2.h>
#include<windows.h>
#include<winsock2.h>
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#define SOCKET unsigned int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif
#include<stdio.h>
#include<vector>
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RES,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RES,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//消息头
struct DataHeader
{
	short datalength; //数据长度
	short cmd; //命令
};
struct Login : public DataHeader
{
	Login()
	{
		datalength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char useName[32];
	char passWord[32];
};
struct LoginRes : public DataHeader
{
	LoginRes()
	{
		datalength = sizeof(LoginRes);
		cmd = CMD_LOGIN_RES;
		res = 0;
	}
	int res;
};

struct Loginout : public DataHeader
{
	Loginout()
	{
		datalength = sizeof(Loginout);
		cmd = CMD_LOGINOUT;
	}
	char useName[32];
};
struct newUserJoin :public DataHeader
{
	newUserJoin()
	{
		datalength = sizeof(newUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
struct LoginoutRes : public DataHeader
{
	LoginoutRes()
	{
		datalength = sizeof(LoginoutRes);
		cmd = CMD_LOGINOUT_RES;
		res = 0;
	}
	int res;
};

std::vector<SOCKET> g_clients;
int processor(SOCKET _c_sock)
{
	//字节缓冲区
	char szRecv[1024] = {};
	// 4.1接收客户端发送的数据
	int nlen = (int)recv(_c_sock, szRecv, sizeof(DataHeader), 0);
	DataHeader *header = (DataHeader*)szRecv;
	if (nlen <= 0)
	{
		printf("客户端已经退出<socket=%d>，结束\n", _c_sock);
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_c_sock, szRecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		printf("收到客户端<socket=%d>收到命令CMD_LOGIN,数据长度%d,usename=%s password=%s \n",
			_c_sock, login->datalength, login->useName, login->passWord);
		// 忽略判断用户名密码是否正确
		LoginRes res;
		//
		send(_c_sock, (char *)&res, sizeof(LoginRes), 0);
	}
	break;
	case CMD_LOGINOUT:
	{
		recv(_c_sock, szRecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
		Loginout* loginout = (Loginout*)szRecv;
		printf("收到客户端<socket=%d>收到命令CMD_LOGINOUT,数据长度%d,usename=%s \n",
			_c_sock, loginout->datalength, loginout->useName);
		// 忽略判断用户名密码是否正确
		LoginoutRes res;
		//先发消息头，再发消息体
		send(_c_sock, (char *)&res, sizeof(LoginoutRes), 0);
	}
	break;
	default:
	{
		DataHeader header = { 0,CMD_ERROR };
		send(_c_sock, (char *)&header, sizeof(DataHeader), 0);
	}
	break;
	}
	return 0;
}
int main()
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat); //启动windos socket 环境 还需要依赖库ws2_32.lib
#endif
	//用socket API建立简易TCP服务端
	//1.建立一个scoket AF_INET：ipv4 SOCK_STREAM ： 流 IPPROTO_TCP
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind 绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); //host to net unsigned short
	//_sin.sin_addr.S_un.S_addr = inet_addr("192.168.31.200");
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; //本机ip地址都可以访问
#else
	_sin.sin_addr.s_addr = INADDR_ANY; //本机ip地址都可以访问
#endif
	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(_sin)))
	{
		printf("ERROR,绑定网络端口失败\n");
	}
	else
	{
		printf("绑定网络端口成功\n");
	}
	//3.listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR,监听网络端口失败\n");
	}
	else
	{
		printf("监听网络端口成功\n");
	}

	while (true)
	{
		//伯克利 socket
		fd_set fdRead; //（描述符）socket集合
		fd_set fdWrite;
		fd_set fdExp;
		//清空
		FD_ZERO(&fdRead); //清理
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		FD_SET(_sock, &fdRead); //将描述符加入集合
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);
		SOCKET max_sock = _sock;
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
			if (max_sock < g_clients[n])
			{
				max_sock = g_clients[n];
			}
		}
		//nfds整数值是指集合中fd_set集合中所有描述符的范围而不是数量
		//即是所有文件描述符最大值+1（windows这个参数可太大作用，可以写0）
		timeval t = { 3, 0 };
		int ret = select(max_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select，结束\n");
			break;
		}
		//判断描述符是否在集合中
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			//4.accept 等待客户端连接
			sockaddr_in client_addr = {};
			int n_addr_len = sizeof(sockaddr_in);
			SOCKET _c_sock = INVALID_SOCKET;
#ifdef _WIN32
			_c_sock = accept(_sock, (sockaddr*)&client_addr, &n_addr_len);
#else
			_c_sock = accept(_sock, (sockaddr*)&client_addr, (socklen_t *)&n_addr_len);
#endif
			if (SOCKET_ERROR == _c_sock)
			{
				printf("ERROR,接受到无效客户端SOCKET\n");
			}
			else
			{
				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					newUserJoin userJoin;
					send(g_clients[n], (const char *)&userJoin, sizeof(newUserJoin), 0);
				}
				g_clients.push_back(_c_sock);
				printf("新客户端加入：socket = %d,ip = %s \n", (int)_c_sock, inet_ntoa(client_addr.sin_addr));
			}
		}
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			if (FD_ISSET(g_clients[n], &fdRead))
			{
				if (-1 == processor(g_clients[n]))
				{
					auto iter = g_clients.begin() + n;//std::vector<SOCKET>::iterator
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);
					}
				}
			}
		}
		//printf("空闲时间处理其他业务\n");
	}


	//6.关闭socket
#ifdef _WIN32
	for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	closesocket(_sock);


	WSACleanup(); //关闭windos socket 环境
#else
	for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	{
		close(g_clients[n]);

	}
	close(_sock);
#endif

	printf("任务结束\n");
	getchar();
	return 0;
}
