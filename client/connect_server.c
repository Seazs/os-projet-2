#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "connect_server.h"
#include "../commun/commun.h"


void connect_to_server(int sock, char* server_ip){
    //printf("---------------------- Connecting to server ----------------------\n");
    
    struct sockaddr_in address;

    address.sin_family = AF_INET;

    address.sin_port = htons(PORT);

    inet_pton(AF_INET, server_ip, &address.sin_addr);

    checked(connect(sock, (struct sockaddr *)&address, sizeof(address)));

    //printf("---------------------- Connected to server ----------------------\n");
}