#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "handle_client.h"
#include "image_comparison.h"
#include "../commun/commun.h"
#include "message_transmission.h"

#define MAX_CONNECTED_CLIENTS 2345 // Arbitrary number

volatile sig_atomic_t signalRecu = 0; // Atomic flag to indicate if a signal has been received
atomic_int nb_clients = 0; // Atomic counter for the number of connected clients
pthread_t threads[MAX_CONNECTED_CLIENTS]; // Array to store thread IDs
Client clients[MAX_CONNECTED_CLIENTS]; // Array to store client information

/**
 * Function to handle a client connection in a separate thread.
 * @param arg_client Pointer to the Client struct containing client information.
 * @return NULL
 */
void *handle_client(void *arg_client) {
    // Ignore SIGINT signal
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return NULL;
    }

    // Handle SIGPIPE and SIGUSR1 signals (sent by the main thread when receiving SIGINT)
    struct sigaction action;
    action.sa_handler = client_thread_signal_handler;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGUSR1, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    if (sigaction(SIGPIPE, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }

    Client client = *(Client *)arg_client;
    pthread_t thread_id = pthread_self();

    handle_server_response(client); // Perform operations to handle the client connection
    
    
    atomic_fetch_sub(&nb_clients, 1); // Decrement the number of connected clients
    threads[client.client_number] = 0; // Clear the thread ID for the client
    close(client.socket); // Close the client socket
    free(arg_client); // Free the memory allocated for the client struct

    return NULL;
}

/**
 * Function to handle an image received from the server.
 * @param serveur_socket The socket connected to the server.
 */
void handle_image(int serveur_socket) {
    // Allocate memory for the image (max 20000 bytes)
    Image *image = (Image *)malloc(sizeof(Image));
    if (!receive_image(serveur_socket, image)) {
        handle_threads(image, serveur_socket); // Perform operations to handle the received image
        free(image); // Free the memory allocated for the image
    } else {
        printf("Erreur lors de la réception de l'image\n");
        free(image); // Free the memory allocated for the image in case of an error
    }
}

void handle_message(int serveur_socket, char* buffer) {
    if(!receive_message(serveur_socket, buffer)){
        printf("Recu du cote serveur : %s\n", buffer);
        send_message(serveur_socket, buffer);
    }
}

void handle_server_response(Client client) {
    int serveur_socket = client.socket;
    char buffer[10000];


    while(client.is_connected) {
        int ret = 0;
        if((ret = receive_message(serveur_socket, buffer))){
            if (ret == 1){
                perror("read()");
                continue;
            }
            else if(ret == 2){
                perror("read() signal received: ");
                client.is_connected = false;
                break;
            }
        }
        clean_str(buffer);
        //printf("Annonce Recu du cote serveur : %s\n", buffer);
        if(strcmp(buffer, "img") == 0){
            handle_image(serveur_socket);
        }
        else if(strcmp(buffer, "message") == 0){
            handle_message(serveur_socket, buffer);
        }
        else if(strcmp(buffer, "exit") == 0){
             printf("Client déconnecté\n");
             client.is_connected = false;
        }
        else{
            printf("Erreur, pas de signal d'annonce reçu\n");
            client.is_connected = false;
        }
    }
}

int accept_new_connection(int server_socket){
    int client_socket;
        if((client_socket = accept(server_socket, NULL, NULL)) < 0) {
            if(errno == EINTR) {
                perror("accept");
                return -1;
            }
            else {
                perror("accept");
                return -1;
            }
        }
}

void accept_connections(int server_socket) {

    if(set_main_signal_handler() != 0) {
        return;
    }

    while(!signalRecu) {
        if (nb_clients >= MAX_CONNECTED_CLIENTS) {
            printf("Nombre maximum de connexions atteint\n");
            sleep(2); // wait 2 seconds for clients to disconnect
            continue;
        }
        printf("Attente de connexion...\n");
        int client_socket;
        if((client_socket = accept_new_connection(server_socket)) < 0) {
            continue;
        }

        Client* client = (Client*)malloc(sizeof(Client));
        client->socket = client_socket;
        client->is_connected = true;


        pthread_t thread_id;
        
        if(pthread_create(&thread_id, NULL, handle_client, client) != 0) {
            perror("pthread_create");
            atomic_fetch_sub(&nb_clients, 1);
            free(client);
            continue;
        }

        client->thread_id = thread_id;
        client->client_number = nb_clients;
        client->has_to_terminate = false;
        clients[nb_clients] = *client;
        threads[nb_clients] = thread_id;
        atomic_fetch_add(&nb_clients, 1);

        printf("Nouveau Client connecté\n");
    }

    close(server_socket);
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (threads[i] != 0) {
            pthread_join(threads[i], NULL);
        }
    }

}

int set_main_signal_handler() {
    struct sigaction action;
    action.sa_handler = main_signal_handler;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGINT, &action, NULL) != 0) {
        perror("sigaction()");
        return 1;
    }
    // ignorer SIGPIPE
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return 1;
    }
    return 0;
}

void main_signal_handler(int signal){
   switch (signal) {
   case SIGINT:
      printf("main : SIGINT\n");
      printf("Fermeture du serveur\n");
      for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (threads[i] != 0) {
        clients[i].has_to_terminate = true;
        pthread_kill(threads[i], SIGUSR1);
        }
      }
      signalRecu = 1;
      break;
   default:
      printf("Signal inconnu\n");
      break;
   }
}

void client_thread_signal_handler(int signal){
   switch (signal) {
   case SIGINT:
      printf("client : SIGINT\n");
      break;
   case SIGPIPE:
      printf("client : SIGPIPE\n");
      break;
   case SIGUSR1:
      printf("client : SIGUSR1\n");

      break;
   default:
      printf("Signal inconnu\n");
      break;
   }
}

