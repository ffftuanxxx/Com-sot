//#include "stdafx.h"
//#include <mysql.h>
#include <winsock2.h> // 包含Winsock2库，用于网络编程
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <windows.h>

#define DEFAULT_PORT 5019 // 默认端口号
#define NUM 1024 // 最大连接数

SOCKET msg_sock[NUM]; // 保存客户端的套接字
SOCKET sock; // 服务器套接字

int msg_len; // 接收到的消息长度
int addr_len; // 地址长度
int count = 0; // 客户端连接数
int times = 1; // 连接次数

char check_IP[30]; // IP地址
char check_pot[30]; // 端口号
char szBuff[1024]; // 缓冲区
char temp[1024]; // 临时缓冲区

FILE* history; // 历史记录文件

struct sockaddr_in local, client_addr; // 本地地址和客户端地址结构体

void acceptThread(int idx);

struct User {
	char group[30]; // 用户组
	char chatname[30]; // 用户昵称
};
struct User user[NUM]; // 保存用户信息的结构体数组


int main(int argc, char** argv) {
	int i;
	WSADATA wsaData;

	/*database = fopen ("database.txt", "w+");
	if (database == NULL){
		printf("can not load the datat base");
		return;
	}
	else{
	char* username[]={"Eternity","a","b"};
	char* password[]={"123456","1","2"};
	for(i = 0; i < 3; i++)
	fprintf(database, "%s %s\n",username[i],password[i]);
	}
	fclose(database);
	*/

	// 初始化Winsock库
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}// end if

	// 填充服务器地址结构体
	// Fill in the address structure
	local.sin_family = AF_INET;  // 设置地址族为IPv4。
	local.sin_addr.s_addr = INADDR_ANY; //将本地地址设置为任意可用的IP地址。
	local.sin_port = htons(DEFAULT_PORT); // 将端口号设置为默认端口号（在代码中定义为5019），并使用htons()函数将端口号转换为网络字节顺序。


	// 创建套接字
	sock = socket(AF_INET, SOCK_STREAM, 0);	//TCp socket

	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if

	 // 绑定套接字到本地地址
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if

	// 监听连接请求
	// waiting for connections
	if (listen(sock, 5) == SOCKET_ERROR)
	{
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if


	printf("Waiting for connections ........\n");

	// 接受客户端连接
	for (i = 0; i < NUM; i++)
	{
		addr_len = sizeof(client_addr);
		msg_sock[i] = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		if (msg_sock[i] == INVALID_SOCKET)
		{
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}// end if
		printf("accepted connection from %s, port %d\n",
			inet_ntoa(client_addr.sin_addr),
			htons(client_addr.sin_port));
		count++;

		// 接收客户端发送的消息
		msg_len = recv(msg_sock[i], szBuff, sizeof(szBuff), 0);

		if (msg_len == SOCKET_ERROR)
		{
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}// end if

		if (msg_len == 0)
		{
			printf("Client closed connection\n");
			closesocket(msg_sock[i]);
			return -1;
		}// end if

		// 将接收到的消息作为用户组保存
		strcpy(user[i].group, szBuff);
		//printf("%s group selected\n",user[i].group);


		msg_len = recv(msg_sock[i], szBuff, sizeof(szBuff), 0);

		if (msg_len == SOCKET_ERROR)
		{
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}// end if

		if (msg_len == 0)
		{
			printf("Client closed connection\n");
			closesocket(msg_sock[i]);
			return -1;
		}// end if

		// 将接收到的消息作为用户昵称保存
		strcpy(user[i].chatname, szBuff);
		printf("%s group is selected by %s\n", user[i].group, user[i].chatname);
		
		// 将连接信息写入历史记录文件
		history = fopen("history.txt", "a+");
		if (history == NULL) {
			printf("can not load the datatbase");
			return;
		}
		else {
			fprintf(history, "IP: %s  Port: %d  Connection Times: %d   chatname: %s   group: %s\n", inet_ntoa(client_addr.sin_addr),
				htons(client_addr.sin_port), times, user[i].chatname, user[i].group);
			fclose(history);
		}

		// 创建线程处理客户端连接
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)acceptThread, (LPVOID)i, NULL, NULL);
	}

	// 关闭套接字和清理Winsock库
	closesocket(sock);
	WSACleanup();
}

//连接线程函数
void acceptThread(int idx) {

	while (1) {
		int i;
		msg_len = recv(msg_sock[idx], szBuff, sizeof(szBuff), 0);

		// 接收客户端发送的消息
		if (msg_len == SOCKET_ERROR)
		{
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}// end if

		if (msg_len == 0)
		{
			printf("Client closed connection\n");
			closesocket(msg_sock[idx]);
			return -1;
		}// end if

		printf("Received message %s from %s, port %d\n", szBuff, inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
		
		// 构造发送给其他客户端的消息
		memset(temp, 0, 120);
		sprintf(temp, "%s from group %s:%s", user[idx].chatname, user[idx].group, szBuff);
		
		// 发送消息给其他客户端
		for (i = 0; i < count; i++) {
			send(msg_sock[i], temp, sizeof(temp), 0);
		}

	}
}//end while loop

