#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define _CRT_SECURE_NO_WARNINGS
#define DEFAULT_PORT	5019
#define MaxBufSize 1024


SOCKET connect_sock;
int msg_len;
const int BUFFER_SIZE = 1024;


FILE* database;
FILE* temp;//temp txt to store the new time
char serach[60];//user info
char temp_buff[60];//temp to search file
int index_temp = 0;
char group[30];//group to join
char chatname[30];//when you chat you name is over there
DWORD WINAPI recvMsgThread(LPVOID IpParameter);


int main(int argc, char** argv) {

	struct sockaddr_in server_addr;
	struct hostent* hp;

	WSADATA wsaData;

	char* server_name = "localhost";
	unsigned short	port = DEFAULT_PORT;
	unsigned int	addr;

	char info[1024];//input to buffer
	char SendBuff[MaxBufSize];//send to buffer
	char RecvBuff[MaxBufSize];//recevice from buffer

	int choice;
	char username[30];
	char password[30];
	char check_username[30];
	char check_password[30];


	while (1) {


	start:printf("choose your mode:1 for login, 2 for register, 3 for update, 4 for delete, others for exit)\n");
		scanf("%d", &choice);



		// 登录
		if (choice == 1) {
			database = fopen("database.txt", "r");
			if (database == NULL) {
				printf("can not load the database");
				return 0;
			}
			else {
				printf("\nPlease input the username:");
				scanf("%s", username);
				printf("Please input the password:");
				scanf("%s", password);
				while (!feof(database)) {
					fscanf(database, "%s%s", check_username, check_password);
					if (strcmp(check_username, username) == 0 && strcmp(check_password, password) == 0) {
						printf("Login successfully!\n");
						memset(username, 0, sizeof(username));
						memset(password, 0, sizeof(password));
						fclose(database);
						goto chating;
					}
				}
				printf("\nWrong username or password!\n\n");
				fclose(database);
			}
		}



		// 注册
		else if (choice == 2) {
			database = fopen("database.txt", "a+");
			if (database == NULL) {
				printf("can not load the datatbase");
				return;
			}
			else {
				printf("\nPlease input the username:");
				scanf("%s", username);
				printf("Please input the password:");
				scanf("%s", password);
				fprintf(database, "%s %s\n", username, password);
				printf("\nregister successfully！\n\n");
				memset(username, 0, sizeof(username));
				memset(password, 0, sizeof(password));
				fclose(database);
			}
		}



		// 更新
		else if (choice == 3) {
			database = fopen("database.txt", "r");
			if (database == NULL) {
				printf("can not load the database");
				return 0;
			}
			else {
				printf("\nPlease input the username you want to update:");
				scanf("%s", username);
				printf("Please input the password you want to update:");
				scanf("%s", password);
				while (!feof(database)) {
					fscanf(database, "%s%s", check_username, check_password);
					if (strcmp(check_username, username) == 0 && strcmp(check_password, password) == 0) {
						temp = fopen("temp.txt", "w");
						strcpy(serach, username);
						strcat(serach, " ");
						strcat(serach, password);
						printf("\nYour previous infomation:%s\n", serach);
						if (temp == NULL) {
							printf("can not load the temp");
							return 0;
						}
						rewind(database);
						while (!feof(database)) {
							fscanf(database, "\n%[^\n]", temp_buff);
							if ((strcmp(serach, temp_buff) != 0) && (strcmp("\n", temp_buff) != 0))
								fprintf(temp, "%s\n", temp_buff);
							memset(temp_buff, 0, sizeof(temp_buff));
						}
						fclose(database);
						fclose(temp);
						Sleep(1000);
						printf("%d\n", remove("database.txt"));
						perror("remove");
						Sleep(1000);
						printf("%d\n", rename("temp.txt", "database.txt"));
						perror("rename");
						database = fopen("database.txt", "a+");
						if (database == NULL) {
							printf("can not load the datatbase");
							return;
						}
						else {
							printf("\nPlease input the new username you want to change:");
							scanf("%s", username);
							printf("Please input the new password you want to change:");
							scanf("%s", password);
							fprintf(database, "%s %s\n", username, password);
							printf("\nUpdate successfully\n\n");
							memset(username, 0, sizeof(username));
							memset(password, 0, sizeof(password));
							fclose(database);
							goto start;
						}
					}
				}
			}
			printf("\nWrong username or password!\n\n");
			fclose(database);
		}



		// 删除账户
		else if (choice == 4) {
			database = fopen("database.txt", "r");
			if (database == NULL) {
				printf("can not load the datat base");
				return 0;
			}
			else {
				printf("\nPlease input the username you want to delete:");
				scanf("%s", username);
				printf("Please input the password you want to delete:");
				scanf("%s", password);

				while (!feof(database)) {

					fscanf(database, "%s%s", check_username, check_password);

					if (strcmp(check_username, username) == 0 && strcmp(check_password, password) == 0) {
						temp = fopen("temp.txt", "w");
						strcpy(serach, username);
						strcat(serach, " ");
						strcat(serach, password);
						printf("\nYour previous infomation:%s\n", serach);
						if (temp == NULL) {
							printf("can not load the temp");
							return 0;
						}
						rewind(database);
						while (!feof(database)) {
							fscanf(database, "\n%[^\n]", temp_buff);
							if ((strcmp(serach, temp_buff) != 0) && (strcmp("\n", temp_buff) != 0))
								fprintf(temp, "%s\n", temp_buff);
							memset(temp_buff, 0, sizeof(temp_buff));
						}
						printf("\nDelete successfully!\n");
						fclose(database);
						fclose(temp);
						Sleep(1000);
						printf("%d\n", remove("database.txt"));
						perror("remove");
						Sleep(1000);
						printf("%d\n", rename("temp.txt", "database.txt"));
						perror("rename");
						printf("\n");
						goto start;
					}

				}

				printf("\nWrong username or password!\n\n");
				fclose(database);

			}

		}



		// 退出程序
		else {
			printf("\nExit the program successfully!\n");
			return 0;
		}



	}





chating: if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
	// stderr: standard error are printed to the screen.
	fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
	//WSACleanup function terminates use of the Windows Sockets DLL. 
	WSACleanup();
	return -1;
}// end if

if (isalpha(server_name[0]))
hp = gethostbyname(server_name);
else {
	addr = inet_addr(server_name);
	hp = gethostbyaddr((char*)&addr, 4, AF_INET);
}// end else

if (hp == NULL) {
	fprintf(stderr, "Cannot resolve address: %d\n", WSAGetLastError());
	WSACleanup();
	return -1;
}// end if

//copy the resolved information into the sockaddr_in structure
memset(&server_addr, 0, sizeof(server_addr));
memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
server_addr.sin_family = hp->h_addrtype;
server_addr.sin_port = htons(port);


connect_sock = socket(AF_INET, SOCK_STREAM, 0);	// TCP socket


if (connect_sock == INVALID_SOCKET) {
	fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
	WSACleanup();
	return -1;
}// end if

printf("\nClient connecting to: %s\n", hp->h_name);

if (connect(connect_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
	fprintf(stderr, "connect() failed with error %d\n", WSAGetLastError());
	WSACleanup();
	return -1;
}// end if

printf("\nPlease input a chating group which you want to join:");
scanf("%s", group);
strcpy(SendBuff, group);
msg_len = send(connect_sock, SendBuff, sizeof(SendBuff), 0);
memset(SendBuff, 0, sizeof(SendBuff));
if (msg_len == SOCKET_ERROR) {
	fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
	WSACleanup();
	return -1;
}// end if

if (msg_len == 0) {
	printf("server closed connection\n");
	closesocket(connect_sock);
	WSACleanup();
	return -1;
}// end if

printf("\nPlease input a chatname for this chating:");
scanf("%s", chatname);
strcpy(SendBuff, chatname);
msg_len = send(connect_sock, SendBuff, sizeof(SendBuff), 0);
memset(SendBuff, 0, sizeof(SendBuff));
if (msg_len == SOCKET_ERROR) {
	fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
	WSACleanup();
	return -1;
}// end if

if (msg_len == 0) {
	printf("server closed connection\n");
	closesocket(connect_sock);
	WSACleanup();
	return -1;
}// end if

CloseHandle(CreateThread(NULL, 0, recvMsgThread, (LPVOID)&connect_sock, 0, 0));
while (1) {
	printf("\nYour chatname %s from group %s->input character string:(quit for exit)\n", chatname, group);
	scanf_s("\n%[^\n]", &info, MaxBufSize);
	if (strcmp(info, "quit") == 0) {
		printf("\n");
		goto start;
	}
	strcpy(SendBuff, info);
	memset(info, 0, sizeof(info));
	msg_len = send(connect_sock, SendBuff, sizeof(SendBuff), 0);
	memset(SendBuff, 0, sizeof(SendBuff));
	if (msg_len == SOCKET_ERROR) {
		fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}// end if

	if (msg_len == 0) {
		printf("server closed connection\n");
		closesocket(connect_sock);
		WSACleanup();
		return -1;
	}// end if
}
closesocket(connect_sock);
WSACleanup();

return 0;

}







DWORD WINAPI recvMsgThread(LPVOID IpParameter) {

	SOCKET cliSock = *(SOCKET*)IpParameter;

	while (1) {

		char buffer[1024];

		int nrecv = recv(cliSock, buffer, sizeof(buffer), 0);

		if (nrecv > 0)
			printf("Receive message from client %s\n", buffer);

		else if (nrecv < 0) {
			printf("receive error\n");
			break;
		}

	}

	return 0;

}




