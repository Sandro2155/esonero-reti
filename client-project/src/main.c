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


/*
* int recv_all(int socket, conversion_request_t* buffer, int len){
* 	int total = 0;
* 	while (total < len){
* 		int n = recv(socket, buffer + total, len - total, 0);
* 		if (n <= 0){
* 			return n;
* 		}
* 		total += n;
* 	}
* 	return total;
* } //returns the total length of the output
*/

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
	if (connect(my_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0){
		printf("failed to connect.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	// TODO: Implement communication logic

	conversion_request_t input_request;

	printf("\tFROM\t");
	scanf("%s", input_request.from);
	if(strlen(input_request.from) <= 0 || strlen(input_request.from) > UNIT_LEN){
		printf("invalid measure unit.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	printf("\tTO\t");
	scanf("%s", input_request.to);
	if(strlen(input_request.to) <= 0 || strlen(input_request.to) > UNIT_LEN){
		printf("invalid measure unit.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	printf("\tvalore:\t");
	scanf("%d", input_request.value);

	htonl(input_request.value); //converts uint32_t in network byte order

	if(send(my_socket, &input_request, sizeof(input_request), 0) != sizeof(input_request)) {
		printf("send failed or sent fewer bytes.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	int bytes_received;
	conversion_response_t output_response;
	/*
	* int total_bytes_received = 0;
	* conversion_response_t buffer[BUFFERSIZE];
	* memset(buffer, 0, BUFFERSIZE);
	*/
	if ((bytes_received = recv(my_socket, &output_response, sizeof(output_response), 0)) <= 0){
		printf("recv failed or connection closed prematurely.\n");
		closesocket(my_socket);
		clearwinsock();
		return 0;
	}

	ntohl(output_response.status);
	ntohl(output_response.value);
	printf("%d, %d", output_response.status, output_response.value); //converts back the output and prints it

	// TODO: Close socket
	closesocket(my_socket);

	clearwinsock();
	return 0;
} // main end
