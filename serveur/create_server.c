#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>


#include "create_server.h"
#include "../commun/commun.h"
#define MAX_CONNEXION 1000


int create_socket(int server_fd){
    // creation d'un descripteur de fichier pour le socket

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    checked(bind(server_fd, (struct sockaddr*) &address, sizeof(address)));
    checked(listen(server_fd, 3));

    size_t addrlen = sizeof(address);
    int new_socket = checked(accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen));

    return new_socket;
}