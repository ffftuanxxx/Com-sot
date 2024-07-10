//#include "stdafx.h"
//#include <mysql.h>
#include <winsock2.h> // ����Winsock2�⣬����������
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <windows.h>

#define DEFAULT_PORT 5019 // Ĭ�϶˿ں�
#define NUM 1024 // ���������

SOCKET msg_sock[NUM]; // ����ͻ��˵��׽���
SOCKET sock; // �������׽���

int msg_len; // ���յ�����Ϣ����
int addr_len; // ��ַ����
int count = 0; // �ͻ���������
int times = 1; // ���Ӵ���

char check_IP[30]; // IP��ַ
char check_pot[30]; // �˿ں�
char szBuff[1024]; // ������
char temp[1024]; // ��ʱ������

FILE* history; // ��ʷ��¼�ļ�

struct sockaddr_in local, client_addr; // ���ص�ַ�Ϳͻ��˵�ַ�ṹ��

void acceptThread(int idx);

struct User {
	char group[30]; // �û���
	char chatname[30]; // �û��ǳ�
};
struct User user[NUM]; // �����û���Ϣ�Ľṹ������


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

	// ��ʼ��Winsock��
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}// end if

	// ����������ַ�ṹ��
	// Fill in the address structure
	local.sin_family = AF_INET;  // ���õ�ַ��ΪIPv4��
	local.sin_addr.s_addr = INADDR_ANY; //�����ص�ַ����Ϊ������õ�IP��ַ��
	local.sin_port = htons(DEFAULT_PORT); // ���˿ں�����ΪĬ�϶˿ںţ��ڴ����ж���Ϊ5019������ʹ��htons()�������˿ں�ת��Ϊ�����ֽ�˳��


	// �����׽���
	sock = socket(AF_INET, SOCK_STREAM, 0);	//TCp socket

	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if

	 // ���׽��ֵ����ص�ַ
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if

	// ������������
	// waiting for connections
	if (listen(sock, 5) == SOCKET_ERROR)
	{
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if


	printf("Waiting for connections ........\n");

	// ���ܿͻ�������
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

		// ���տͻ��˷��͵���Ϣ
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

		// �����յ�����Ϣ��Ϊ�û��鱣��
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

		// �����յ�����Ϣ��Ϊ�û��ǳƱ���
		strcpy(user[i].chatname, szBuff);
		printf("%s group is selected by %s\n", user[i].group, user[i].chatname);
		
		// ��������Ϣд����ʷ��¼�ļ�
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

		// �����̴߳���ͻ�������
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)acceptThread, (LPVOID)i, NULL, NULL);
	}

	// �ر��׽��ֺ�����Winsock��
	closesocket(sock);
	WSACleanup();
}

//�����̺߳���
void acceptThread(int idx) {

	while (1) {
		int i;
		msg_len = recv(msg_sock[idx], szBuff, sizeof(szBuff), 0);

		// ���տͻ��˷��͵���Ϣ
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
		
		// ���췢�͸������ͻ��˵���Ϣ
		memset(temp, 0, 120);
		sprintf(temp, "%s from group %s:%s", user[idx].chatname, user[idx].group, szBuff);
		
		// ������Ϣ�������ͻ���
		for (i = 0; i < count; i++) {
			send(msg_sock[i], temp, sizeof(temp), 0);
		}

	}
}//end while loop

