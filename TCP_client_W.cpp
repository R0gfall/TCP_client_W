#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define p_ASCII 0x70
#define u_ASCII 0x75
#define t_ASCII 0x74


int init()
{
	WSADATA wsa_data;
	return (WSAStartup(MAKEWORD(2, 2), &wsa_data));
}


void deinit()
{
	WSACleanup();
}


ADDRINFO install_properties(void)
{
	ADDRINFO hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	return hints;
}


//int first_message_to_conn(SOCKET connectSocket)
//{
//	
//}


void send_one_line_msg(SOCKET connectSocket, FILE* file) {

	char buffer[6];

	fscanf(file, "%s", buffer);

	printf(buffer);


}




int main()
{
	int result_conn, result_info;
	SOCKET connectSocket = INVALID_SOCKET;
	ADDRINFO* addr_result = NULL;
	const char* sendBuffer = "put";
	
	init();

	ADDRINFO hints = install_properties();
	result_info = getaddrinfo("localhost", "9000", &hints, &addr_result);

	// printf("%d\n", result_info);
	if (result_info != 0) {
		printf("ERROR GET ADDRESS INFO");
		WSACleanup();
		return -1;
	}

	connectSocket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
	if (connectSocket == INVALID_SOCKET) {
		printf("ERROR CREATE SOCKET");
		freeaddrinfo(addr_result);
		WSACleanup();
		return -1;	
	}

	result_conn = connect(connectSocket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
	if (result_conn == SOCKET_ERROR) {
		for (int i = 1; i < 10; i++) {
			result_conn = connect(connectSocket, addr_result->ai_addr, (int)addr_result->ai_addrlen);

			if (result_conn == SOCKET_ERROR) {
				printf("ERROR CONNECT SOCKET\n");

				closesocket(connectSocket);
				connectSocket = INVALID_SOCKET;

			}

			int timer = 0;
			while (timer < 100) {
				timer++;
			}
		}
	}

	if (result_conn == INVALID_SOCKET) {
		printf("ERROR 10 TIMES CONNECT\n");
		freeaddrinfo(addr_result);
		deinit();
		return 1;
	}

	

	result_conn = send(connectSocket, sendBuffer, (int)strlen(sendBuffer), 0);

	if (result_conn == SOCKET_ERROR) {

		printf("ERROR SEND MESSAGE");

		closesocket(connectSocket);
		freeaddrinfo(addr_result);
		deinit();
		return 1;
	}
		
	
	FILE* file = fopen("test.txt", "r");
	if (file == NULL) {
		printf(">>>>>\n");
	}

	send_one_line_msg(connectSocket, file);





}