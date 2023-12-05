#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "connect_server.h";
#include "../commun/commun.h";


void connect_to_server(int sock){
    printf("---------------------- Connecting to server ----------------------\n");
    struct sockaddr_in address;

    address.sin_family = AF_INET;

    address.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    checked(connect(sock, (struct sockaddr *)&address, sizeof(address)));

    printf("---------------------- Connected to server ----------------------\n");
}