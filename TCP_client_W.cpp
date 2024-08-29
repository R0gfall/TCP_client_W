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

int FLAG = 0;
int COUNT_OK = 0;


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

	while (sent < size)
	{
		// Отправка очередного блока данных
		int res = send(connectSocket, buffer + sent, size - sent, 0);
		if (res < 0)
			return -1;
		sent += res;
		printf("PUT bytes send %d\n", sent);
	}

	return 1;

}


int send_one_line_msg(SOCKET connectSocket, FILE* file, int count_line) {

	char buffer[2];
	int result_conn, result_recv;
	int int_buffer[3];
	unsigned int summ_date, count_line_INET, summ_date_INET, len_message, len_msg_INET;
	char message_buf[256];
	char* ui_msg_buf;
	char* buffer_recv;
	char string_msg[20];
	
	// dd.mm.yyyy hh:mm:ss hh:mm:ss Message

	/*Сначала отправляется 4 байта - номер сообщения в исходном файле, целое 32 - битное число в СЕТЕВОМ порядке байтов, индексация от 0.
	затем 4 байта - значение даты, unsigned int, рассчитанный как yyyy * 10000 + mm * 100 + dd, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение первого времени, unsigned int, рассчитано как hh * 10000 + mm * 100 + ss, передается в СЕТЕВОМ порядке байтов;
	затем 4 байта - значение часов, минут и секунд второго времени(в том же формате),
	затем 4 байта - длина поля Message в символах, unsigned int, в СЕТЕВОМ порядке байтов,
	затем N байт - символы самого поля Message.
	*/

	// >>> cnt msg


	// date send

	if (fscanf(file, "%d.%d.%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]) != 3) {
		FLAG = 1;
		printf("END FILE\n");
		return count_line;
	}

	count_line_INET = htonl(count_line);
	printf(">>>>>>>>>>>>%d\n", count_line_INET);

	//char string_msg[20];

	/*_itoa(count_line_INET, string_msg, 10);
	printf("<<<<<%s\n", string_msg);*/


	ui_msg_buf = (char*)(malloc(4));
	memcpy(ui_msg_buf, &count_line_INET, 4);

	result_conn = send(connectSocket, ui_msg_buf, 4, 0);

	printf(">>>>>>>>>>>>%s\n", ui_msg_buf);
	printf("COUNT bytes send %d\n",result_conn);


	// date send

	summ_date = int_buffer[0] + int_buffer[1] * 100 + int_buffer[2] * 10000;

	summ_date_INET = htonl(summ_date);
	memcpy(ui_msg_buf, &summ_date_INET, 4);

	result_conn = send(connectSocket, ui_msg_buf, 4, 0);

	printf("DATE bytes send %d\n", result_conn);


	// time send

	fscanf(file, "%d:%d:%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]);
	summ_date = int_buffer[0] * 10000 + int_buffer[1] * 100 + int_buffer[2];

	summ_date_INET = htonl(summ_date);
	memcpy(ui_msg_buf, &summ_date_INET, 4);

	result_conn = send(connectSocket, ui_msg_buf, 4, 0);

	printf("TIME 1 bytes send %d\n", result_conn);
	

	// time send

	fscanf(file, "%d:%d:%d ", &int_buffer[0], &int_buffer[1], &int_buffer[2]);
	summ_date = int_buffer[0] * 10000 + int_buffer[1] * 100 + int_buffer[2];

	summ_date_INET = htonl(summ_date);
	memcpy(ui_msg_buf, &summ_date_INET, 4);

	result_conn = send(connectSocket, ui_msg_buf, 4, 0);

	printf("TIME 2 bytes send %d\n", result_conn);


	// len msg send

	fgets(message_buf, sizeof(message_buf), file);
	size_t len = strcspn(message_buf, "\n");
	message_buf[len] = '\0';

	len_message = strlen(message_buf);

	len_msg_INET = htonl(len_message);
	memcpy(ui_msg_buf, &len_msg_INET, 4);
	
	result_conn = send(connectSocket, ui_msg_buf, 4, 0);


	// msg send

	int sent = 0;
	while (sent < len_message)
	{
		// Отправка очередного блока данных
		int result_conn = send(connectSocket, message_buf + sent, 1, 0);
		if (result_conn < 0)
			return -1;
		sent += result_conn;
		printf("MSG SEND: %d bytes sent.\n", sent);
	}

	printf("ALL MESSAGE bytes send: %s", message_buf);
	

	// recv get

	buffer_recv = (char*)malloc(2 * sizeof(char));
	result_recv = recv(connectSocket, buffer_recv, 2, 0);
	buffer_recv[result_recv] = '\0';

	printf("\nRECV FORM %s\n", buffer_recv);
	printf("RECV bytes send %d\n\n", result_conn);

	if (strcmp(buffer_recv, "ok") == 0) {
		
		COUNT_OK += 1;
		printf("OK GET IT!\n");
	}



	count_line++;
	return count_line;
}




int main(int argc, char* argv[])
{
	int result_conn, result_info, result_put;
	SOCKET connectSocket = INVALID_SOCKET;
	ADDRINFO* addr_result = NULL;
	int count_line = 0;

	char ip[16]; // Буфер для хранения IP-адреса
	char port[6]; // Буфер для хранения порта

	// Копируем входную строку, так как strtok изменяет исходную строку
	char buffer[256];
	strncpy(buffer, argv[1], sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0'; // Обеспечиваем нуль-терминатор

	// Разделяем строку по символу ':'
	char* token = strtok(buffer, ":");
	if (token != NULL) {
		// Считываем IP-адрес
		strncpy(ip, token, sizeof(ip));
		ip[sizeof(ip) - 1] = '\0'; // Обеспечиваем нуль-терминатор
	}

	token = strtok(NULL, ":");
	if (token != NULL) {
		// Считываем порт
		strncpy(port, token, sizeof(port));
		port[sizeof(port) - 1] = '\0'; // Обеспечиваем нуль-терминатор
	}

	printf("IP Address: %s\n", ip);
	printf("Port: %s\n", port);

	init();

	ADDRINFO hints = install_properties();
	result_info = getaddrinfo(ip, port, &hints, &addr_result);

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

	FILE* file = fopen(argv[2], "r");
	if (file == NULL) {
		printf("ERROR OPEN FILE\n");
	}


	// translate msg

	result_put = first_message_to_conn(connectSocket);  //first message "put"
	if (result_put == -1) {
		closesocket(connectSocket);
		freeaddrinfo(addr_result);
		deinit();
		return 1;
	}

	while (1) {

		count_line = send_one_line_msg(connectSocket, file, count_line); //first line message
		if (FLAG == 1) {
			break;
		}
		printf(">>>%d\n", count_line);
	}

	char* buffer_recv;
	int result_recv;
	

	printf("%d, %d\n", count_line, COUNT_OK);
	
	while (count_line != COUNT_OK) {
		buffer_recv = (char*)malloc(2 * sizeof(char));
		result_recv = recv(connectSocket, buffer_recv, 2, 0);
		buffer_recv[result_recv] = '\0';

		printf("\nRECV FORM %s\n", buffer_recv);
		printf("RECV bytes send %d\n\n", result_conn);

		if (strcmp(buffer_recv, "ok") == 0) {
			COUNT_OK++;
			printf("OK GET IT!\n");
		}
	}

}