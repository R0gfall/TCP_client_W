#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	int size = strlen(buffer);
	int sent = 0;

	/*if (result_conn = send(connectSocket, buffer, (int)strlen(buffer), 0) == SOCKET_ERROR) {
		printf("ERROR FUNC TO FIRST CONNECT");
		return -1;
	}*/
	while (sent < size)
	{
		//printf(">>>>>>");
		// Отправка очередного блока данных
		int res = send(connectSocket, buffer + sent, size - sent, 0);
		if (res < 0)
			return -1;
		sent += res;
		printf(" %d bytes sent.\n", sent);
	}

	return 1;

}


int send_one_line_msg(SOCKET connectSocket, FILE* file, int count_line) {

	char buffer[2];
	int result_conn, result_recv;
	int int_buffer[3];
	unsigned int summ_date;
	char* message_buf[8];
	
	// dd.mm.yyyy hh:mm:ss hh:mm:ss Message

	/*Сначала отправляется 4 байта - номер сообщения в исходном файле, целое 32 - битное число в СЕТЕВОМ порядке байтов, индексация от 0.
	затем 4 байта - значение даты, unsigned int, рассчитанный как yyyy * 10000 + mm * 100 + dd, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение первого времени, unsigned int, рассчитано как hh * 10000 + mm * 100 + ss, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение часов, минут и секунд второго времени(в том же формате),
	затем 4 байта - длина поля Message в символах, unsigned int, в СЕТЕВОМ порядке байтов,
	затем N байт - символы самого поля Message.
	*/



	//printf("%d %d %d\n", int_buffer[0], int_buffer[1], int_buffer[2]);

	unsigned int count_line_INET = htonl(count_line);

	char* ab = (char*)(malloc(4));
	memcpy(ab, &count_line_INET, 4);

	//printf("<<<%d\n", ab);
	//printf("%d\n", count_line);
	
	result_conn = send(connectSocket, ab, 4, 0);

	printf(">>%d\n",result_conn);
	
	printf("%d\n", count_line_INET);
	

	//char buffer_recv[2];
	int res;


	// date send

	fscanf(file, "%d.%d.%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]);
	
	summ_date = int_buffer[0] + int_buffer[1] * 100 + int_buffer[2] * 10000;

	unsigned int summ_date_INET = htonl(summ_date);

	
	memcpy(ab, &summ_date_INET, 4);

	result_conn = send(connectSocket, ab, 4, 0);

	printf("%d\n", summ_date);
	printf(">>%d\n", result_conn);

	// time send

	fscanf(file, "%d:%d:%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]);

	summ_date = int_buffer[0] * 10000 + int_buffer[1] * 100 + int_buffer[2];
		

	summ_date_INET = htonl(summ_date);

	memcpy(ab, &summ_date_INET, 4);

	result_conn = send(connectSocket, ab, 4, 0);

	printf("%d\n", summ_date);

	// time send

	fscanf(file, "%d:%d:%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]);

	summ_date = int_buffer[0] * 10000 + int_buffer[1] * 100 + int_buffer[2];


	summ_date_INET = htonl(summ_date);

	memcpy(ab, &summ_date_INET, 4);

	result_conn = send(connectSocket, ab, 4, 0);

	printf("%d\n", summ_date);

	// len msg send
	fscanf(file, "%s", &message_buf);
	//strcat((char*)message_buf, "\n");
	


	unsigned int len_message = strlen((char*)message_buf);
	//unsigned int a = 7;
	unsigned int len_msg_INET = htonl(len_message);
	printf("%u, %u\n", len_message, len_msg_INET);

	memcpy(ab, &len_msg_INET, 4);

	result_conn = send(connectSocket, ab, 4, 0);






	// msg send

	/*fscanf(file, "%s", &message_buf);
	printf("%s\n", message_buf);*/

	//printf("%d\n", strlen((char*)message_buf));


	//int sent = 0;
	//int size = strlen((char*)message_buf);

	//while (sent < size)
	//{
	//	//printf(">>>>>>");
	//	// Отправка очередного блока данных
	//	int res = send(connectSocket, (char*)message_buf + sent, 2, 0);
	//	if (res < 0)
	//		return -1;
	//	sent += res;
	//	printf(" %d bytes sent.\n", sent);
	//}

	res = send(connectSocket, (char*)message_buf, len_message, 0);

	printf("%s", message_buf);

	//result_conn = send(connectSocket, (char*)&message_buf, strlen((char*)message_buf), 0);

	//printf(">>>%d\n", result_conn);



	//printf("%d %d %d\n", int_buffer[0], int_buffer[1], int_buffer[2]);


	//result_conn = send(connectSocket, "stop", strlen("stop"), 0);


	//while ((res = recv(connectSocket, buffer, sizeof(buffer), 0)) > 0)
	//{
	//	//fwrite(buffer, 1, res, f);
	//	printf(" %d bytes received\n", res);
	//}
	//if (res < 0)
	//	return -1;
	//return 0;

	char* buffer_recv = (char*)malloc(1);
	//char buffer_recv[2];

	//printf("\n%d\n", strlen(buffer_recv));

	result_recv = recv(connectSocket, buffer_recv, 2, 0);
	printf("\n____________%s\n", buffer_recv);

	printf("%d\n", result_recv);
	


	/*fscanf(file, "%s", buffer);
	printf(buffer);*/
	printf("2121212121");
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

	//printf("%d\n", count_line);
	count_line = send_one_line_msg(connectSocket, file, count_line); //first line message



}