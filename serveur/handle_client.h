#ifndef _HANDLE_CLIENT_H_
#define _HANDLE_CLIENT_H_

typedef struct {
    unsigned int taille;
    char *raw_image;
} Image;

void * handle_client(void* socket);


void handle_image(int serveur_socket);

void handle_message(int serveur_socket, char* buffer);

void handle_server_response(int serveur_socket);

void accept_connections(int server_socket);

#endif

