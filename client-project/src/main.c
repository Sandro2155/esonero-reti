/*
 * main.c
 *
 * TCP Client - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP client
 * portable across Windows, Linux and macOS.
 */

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0

#define BUFFERSIZE 512

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

	// TODO: Implement client logic

#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	int my_socket;

	// TODO: Create socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (my_socket < 0){
		printf("socket creation failed.\n");
		clearwinsock();
		return 0;
	} //just for good measure

	// TODO: Configure server address
	struct sockaddr_in client_addr;
	memset (&client_addr, 0, sizeof(client_addr));

	// TODO: Connect to server
	if (connect(my_socket, (struct sockaddr *)&client_addr) < 0){
		printf("failed to connect.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	// TODO: Implement communication logic

	conversion_request_t input_request;
	scanf("%s %s %d", input_request.from, input_request.to, input_request.value);

	if(send(my_socket, input_request, sizeof(input_request), 0) != sizeof(input_request)) {
		printf("send failed or sent fewer bytes.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	recv(my_socket, );

	// TODO: Close socket
	closesocket(my_socket);

	clearwinsock();
	return 0;
} // main end
