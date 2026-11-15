/*
 * main.c
 *
 * TCP Server - Template for Computer Networks assignment
 *
 * This file contains the boilerplate code for a TCP server
 * portable across Windows, Linux and macOS.
 */

/*
 * all of my comments are going to be in English,
 * cause i hate that eclipse highlights every
 * Italian word
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

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

//table implementation
typedef enum {LENGTH, MASS} quantity_t;

typedef struct {
	const char *symbol;
	quantity_t quantity;
	double factor;
} unit_t;

static const unit_t UNITS[] = {
		{"m", LENGTH, 1.0}, {"km", LENGTH, 1000.0}, {"ft", LENGTH, 0.3048},
		{"yd", LENGTH, 0.9144}, {"mi", LENGTH, 1609.344},
		{"g", MASS, 0.001}, {"kg", MASS, 1.0}, {"oz", MASS, 0.028349523},
		{"lb", MASS, 0.45359237},
};

/*
 * Converts a value from one unit to another of the same quantity.
 * Example: 26.2 mi -> km is convert(26.2f, 1609.344, 1000.0) = 42.16 km
 */
float convert (float value, double factor_from, double factor_to){
	return (float)(value * factor_from / factor_to); // double math: same result on every machine
}

int main(int argc, char *argv[]) {

	// Disable stdout buffering so that log lines are printed immediately
	setvbuf(stdout, NULL, _IONBF, 0);

	// TODO: Implement server logic

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
		printf ("socket creation failed.\n");
		clearwinsock();
		return -1;
	} //just for good measure

	// TODO: Configure server address
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0: accept connections on any interface (127.0.0.1 included)

	// TODO: Bind socket
	if(bind(my_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
		printf("bind failed.\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	} //checking if bind was successful

	// TODO: Set socket to listen
	if (listen(my_socket, QUEUE_SIZE) < 0) {
		printf("listen failed.\n");
		closesocket (my_socket);
		clearwinsock();
		return -1;
	} //checking if listener was successful

	// TODO: Implement connection acceptance loop
	struct sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	 while (1) {
		 int client_socket = accept(my_socket, (struct sockaddr *) &client_addr, &addr_len);

		 if(client_socket < 0){
	    	 printf("accept failed.\n");
	    	 closesocket(client_socket);
	    	 clearwinsock();
	    	 return -1;
	     }

	     // Handle client communication

	     printf("handling client %s\n", inet_ntoa(client_addr.sin_addr));

	     conversion_request_t client_request;
	     conversion_response_t output_response;
	     int bytes_received = recv(client_socket, (char *) &client_request, sizeof(client_request), 0);
	     if (bytes_received > 0){
	    	 printf("received: %s, %s, %f\n", client_request.from, client_request.to, (float) client_request.value);
	    	 unit_t from; //saves the first unit measure
	    	 unit_t to; //saves the second unit measure
	    	 int f = 0; //flag
	    	 for(int i = 0; i < (sizeof(UNITS)/sizeof(UNITS[0])) && f == 0; i++){
	    		 if (strcmp(client_request.from, UNITS[i].symbol) == 0){
	    			 f = 1; //if it's a valid unit measure, i break the "for" cycle
	    			 from = UNITS[i];
	    		 }
	    	 } //checks if the unit measure is valid
	    	 if (!f){
	    		 output_response.status = STATUS_INVALID_UNIT;
	    		 output_response.value = 0;
	    	 } //if it isn't, it returns invalid status
	    	 else{
	    		 f = 0;
	    		 for(int i = 0; i < (sizeof(UNITS)/sizeof(UNITS[0])) && f == 0; i++){
	    		 	    		 if (strcmp(client_request.to, UNITS[i].symbol) == 0){
	    		 	    			 f = 1;
	    		 	    			 to = UNITS[i];
	    		 	    		 }
	    		 	    	 } //checks if the unit measure is valid
	    		 if (!f){
	    			 output_response.status = STATUS_INVALID_UNIT;
	    			 output_response.value = 0;
	    		 } //if it isn't, it returns invalid status
	    		 else{
	    			 if (from.quantity != to.quantity){
	    				 output_response.status = STATUS_INCOMPATIBLE_UNITS;
	    				 output_response.value = 0;
	    			 } //checks if the unit measures are compatible with each other
	    			 else {
	    				 if (client_request.value < 0 || client_request.value > MAX_VALUE){
	    					 output_response.status = STATUS_INVALID_VALUE;
	    					 output_response.value = 0;
	    				 } //checks if the value is valid
	    				 else{
	    					 output_response.status = STATUS_OK;
	    					 output_response.value = (uint32_t) convert(client_request.value, from.factor, to.factor);
	    				 } //status must be OK, so i make the conversion
	    			 } //ends third else
	    		 } //ends second else
	    	 } //ends first else
	    	 send (client_socket, (char *)&output_response, sizeof(output_response), 0);
	    	 printf("sent %f, %f\n", (float) output_response.status, (float) output_response.value);
	     }//ends if

	     closesocket(client_socket);
	 }

	printf("Server terminated.\n");

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end
