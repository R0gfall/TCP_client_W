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


int first_message_to_conn(SOCKET connectSocket)
{
	const char buffer[] = "put";
	int result_conn = SOCKET_ERROR;

	if (result_conn = send(connectSocket, buffer, (int)strlen(buffer), 0) == SOCKET_ERROR) {
		printf("ERROR FUNC TO FIRST CONNECT");
		return -1;
	}
	return 1;

}


int send_one_line_msg(SOCKET connectSocket, FILE* file, int count_line) {

	char buffer[2];
	int result_conn, result_recv;
	
	// dd.mm.yyyy hh:mm:ss hh:mm:ss Message

	/*Сначала отправляется 4 байта - номер сообщения в исходном файле, целое 32 - битное число в СЕТЕВОМ порядке байтов, индексация от 0.
	затем 4 байта - значение даты, unsigned int, рассчитанный как yyyy * 10000 + mm * 100 + dd, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение первого времени, unsigned int, рассчитано как hh * 10000 + mm * 100 + ss, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение часов, минут и секунд второго времени(в том же формате),
	затем 4 байта - длина поля Message в символах, unsigned int, в СЕТЕВОМ порядке байтов,
	затем N байт - символы самого поля Message.
	*/

	fscanf(file, "%s", buffer);
	printf("%s\n", buffer);


	unsigned int count_line_INET = htonl(count_line);

	//printf("%d\n", count_line);
	
	//result_conn = send(connectSocket, (char*)&count_line_INET, sizeof(count_line_INET), 0);

	result_conn = send(connectSocket, (char*)&count_line_INET, sizeof(count_line_INET), 0);
	printf("%d\n", count_line_INET);
	// printf("%d, %d\n", result_conn, sizeof(count_line_INET));


	//result_conn = send(connectSocket, (char*)&count_line_INET, sizeof(count_line_INET), 0);


	result_recv = recv(connectSocket, buffer, 2, 0);

	printf("%d\n", result_recv);
	


	/*fscanf(file, "%s", buffer);
	printf(buffer);*/

	return count_line++;
}




int main()
{
	int result_conn, result_info, result_put;
	SOCKET connectSocket = INVALID_SOCKET;
	ADDRINFO* addr_result = NULL;
	const char* sendBuffer = "put";
	int count_line = 0;

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

	

	/*result_conn = send(connectSocket, sendBuffer, (int)strlen(sendBuffer), 0);

	if (result_conn == SOCKET_ERROR) {

		printf("ERROR SEND MESSAGE");

		closesocket(connectSocket);
		freeaddrinfo(addr_result);
		deinit();
		return 1;
	}*/
		
	FILE* file = fopen("test.txt", "r");
	if (file == NULL) {
		printf(">>>>>\n");
	}


	// translate msg

	result_put = first_message_to_conn(connectSocket);  //first message "put"
	if (result_put == -1) {
		closesocket(connectSocket);
		freeaddrinfo(addr_result);
		deinit();
		return 1;
	}

	printf("%d\n", count_line);
	count_line = send_one_line_msg(connectSocket, file, count_line); //first line message


	


}