#ifndef _HANDLE_CLIENT_H_
#define _HANDLE_CLIENT_H_

#include <stdatomic.h>

#include <stdbool.h>
typedef struct {
    unsigned int taille;
    char *raw_image;
} Image;

typedef struct {
    int socket;
    bool is_connected;
    bool has_to_terminate;
    int client_number;
    pthread_t thread_id;
} Client;
void * handle_client(void* arg_client);


void handle_image(Client *client);

void handle_server_response(Client *client);

int accept_new_connection(int server_socket);

void accept_connections(int server_socket);

int set_main_signal_handler();

void main_signal_handler(int signal);

void client_thread_signal_handler(int signal);

#endif

