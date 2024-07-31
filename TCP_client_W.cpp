#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")


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



int main()
{
	int result_conn, result_info;
	SOCKET connectSocket = INVALID_SOCKET;
	ADDRINFO* addr_result = NULL;
	
	ADDRINFO hints = install_properties();

	result_info = getaddrinfo("localhost", "9000", &hints, &addr_result);

	if (result_info != 0) {
		printf("ERROR GET ADDRESS INFO");
		WSACleanup();
		return -1;
	}


	connectSocket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
	if (connectSocket == INVALID_SOCKET) {
		printf("ERROR CREATE SOCKET");
		WSACleanup();
		return -1;	
	}

}