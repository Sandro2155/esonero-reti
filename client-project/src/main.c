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
		return -1;
	}
#endif

	int my_socket;

	// TODO: Create socket
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (my_socket < 0){
		printf("socket creation failed.\n");
		clearwinsock();
		return -1;
	} //just for good measure

	// TODO: Configure server address
	struct sockaddr_in server_addr;
	memset (&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);

	// TODO: Connect to server
	if (connect(my_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		printf("failed to connect.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	// TODO: Implement communication logic

	conversion_request_t input_request;
	float c = 0;

	if (argc == 4){
		strcpy(input_request.from, argv[1]);
		strcpy(input_request.to, argv[2]);
		c = strtof(argv[3], NULL);
		memcpy (&input_request.value, &c, sizeof(c));
	}

	else{
		scanf("%s %s %f", input_request.from, input_request.to, &c);
		if(strlen(input_request.from) <= 0 || strlen(input_request.from) > UNIT_LEN || strlen(input_request.to) <= 0 || strlen(input_request.to) > UNIT_LEN){
			printf("invalid measure unit.\n");
			closesocket(my_socket);
			clearwinsock();
			return 0;
		}
		memcpy(&input_request.value, &c, sizeof(c)); //copying the same 32 bytes of the float 'c'
	}

	input_request.value = htonl(input_request.value); //converts uint32_t in network byte order

	if(send(my_socket, (char *) &input_request, sizeof(input_request), 0) != sizeof(input_request)) {
		printf("send failed or sent fewer bytes.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	int bytes_received;
	conversion_response_t output_response;

	if ((bytes_received = recv(my_socket, (char *) &output_response, sizeof(output_response), 0)) <= 0){
		printf("recv failed or connection closed prematurely.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//converts back both values
	output_response.status = ntohl(output_response.status);
	output_response.value = ntohl(output_response.value);
	float o;
	memcpy(&o, &output_response.value, sizeof(o));
	if (output_response.status == STATUS_OK){
		printf("Ricevuto risultato dal server ip %s. %.2f %s = %.2f %s\n", inet_ntoa(server_addr.sin_addr), c, input_request.from, o, input_request.to);
	}
	else{
		if (output_response.status == STATUS_INVALID_UNIT){
				printf("Ricevuto risultato dal server ip %s. Status: %d, valore: %.2f\n", inet_ntoa(server_addr.sin_addr), output_response.status, o);
			}
			else{
				if (output_response.status == STATUS_INCOMPATIBLE_UNITS){
						printf("Ricevuto risultato dal server ip %s. Status: %d, valore: %.2f", inet_ntoa(server_addr.sin_addr), output_response.status, o);
					}
					else{
						printf("Ricevuto risultato dal server ip %s. Status: %d, valore: %.2f", inet_ntoa(server_addr.sin_addr), output_response.status, o);
					}
			}
	}

	// TODO: Close socket
	closesocket(my_socket);

	clearwinsock();
	return 0;
} // main end
