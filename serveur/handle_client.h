#ifndef _HANDLE_CLIENT_H_
#define _HANDLE_CLIENT_H_

#include <stdbool.h>
typedef struct {
    unsigned int taille;
    char *raw_image;
} Image;

typedef struct {
    int socket;
    bool is_connected;
    pthread_t thread_id;
} Client;
void * handle_client(void* socket);


void handle_image(int serveur_socket);

void handle_message(int serveur_socket, char* buffer);

void handle_server_response(Client client);

void accept_connections(int server_socket);

void main_signal_handler(int signal);

void client_thread_signal_handler(int signal);

#endif

