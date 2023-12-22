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
Client *clients[MAX_CONNECTED_CLIENTS]; // Array to store client information

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

    Client *client = (Client *)arg_client;
    
    handle_server_response(client); // Perform operations to handle the client connection
    
    
    atomic_fetch_sub(&nb_clients, 1); // Decrement the number of connected clients
    threads[client->client_number] = 0; // Clear the thread ID for the client
    close(client->socket); // Close the client socket
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


/**
 * Handles the server's response.
 * @param client The client to handle the response for.
 */
void handle_server_response(Client *client) {
    int serveur_socket = client->socket;
    char buffer[10000];

    // Loop while the client is connected
    while(client->is_connected) {
        int ret = 0;
        // Receive a message from the server
        if((ret = receive_message(serveur_socket, buffer))){
            if (ret == 1){ // Unknown error
                continue; 
            }
            else if(ret == 2){ // EINTR
                client->is_connected = false;
                break;
            }
        }
        clean_str(buffer);
        // Handle different types of server responses
        if(strcmp(buffer, "img") == 0){
            handle_image(serveur_socket);
        }
        else if(strcmp(buffer, "exit") == 0){
             printf("Client disconnected\n");
             client->is_connected = false;
        }
        else{
            printf("Error, no announcement signal received\n");
            client->is_connected = false;
        }
    }
}

/**
 * Accepts a new connection from a client.
 * @param server_socket The server's socket.
 * @return The client's socket, or -1 if an error occurred.
 */
int accept_new_connection(int server_socket){
    int client_socket;
    // Accept a connection from a client
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
    return client_socket;
}

/**
 * Accepts connections from clients.
 * @param server_socket The server's socket.
 */
void accept_connections(int server_socket) {

    // Set the main signal handler
    if(set_main_signal_handler() != 0) {
        return;
    }

    // Loop until a signal is received
    while(!signalRecu) {
        // Check if the maximum number of connections has been reached
        if (nb_clients >= MAX_CONNECTED_CLIENTS) {
            printf("Maximum number of connections reached\n");
            sleep(2); // Wait 2 seconds for clients to disconnect
            continue;
        }
        printf("Waiting for connection...\n");
        int client_socket;
        // Accept a new connection from a client
        if((client_socket = accept_new_connection(server_socket)) < 0) {
            continue;
        }

        // Create a new client
        Client* client = (Client*)malloc(sizeof(Client));
        client->socket = client_socket;
        client->is_connected = true;

        pthread_t thread_id;
        // Create a new thread to handle the client
        if(pthread_create(&thread_id, NULL, handle_client, client) != 0) {
            perror("pthread_create");
            atomic_fetch_sub(&nb_clients, 1);
            free(client);
            continue;
        }

        // Set the client's thread ID and client number
        client->thread_id = thread_id;
        client->client_number = nb_clients;
        client->has_to_terminate = false;
        clients[nb_clients] = client;
        threads[nb_clients] = thread_id;
        atomic_fetch_add(&nb_clients, 1);

        printf("New client connected\n");
    }

    // Close the server socket and join all client threads
    close(server_socket);
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (threads[i] != 0) {
            pthread_join(threads[i], NULL);
        }
    }

}

/**
 * Sets the main signal handler.
 * @return 0 if successful, or 1 if an error occurred.
 */
int set_main_signal_handler() {
    struct sigaction action;
    action.sa_handler = main_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    // Set the signal handler for SIGINT
    if(sigaction(SIGINT, &action, NULL) != 0) {
        perror("sigaction()");
        return 1;
    }
    // Ignore SIGPIPE
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return 1;
    }
    return 0;
}

/**
 * Handles signals in the main thread.
 * @param signal The signal to handle.
 */
void main_signal_handler(int signal){
   switch (signal) {
   case SIGINT:
      // If the signal is SIGINT (Ctrl+C), print a message, set all clients to terminate,
      // send SIGUSR1 to all client threads, and set signalRecu to 1
      printf("main : SIGINT\n");
      printf("Fermeture du serveur\n");
      for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (threads[i] != 0) {
        clients[i]->has_to_terminate = true;
        pthread_kill(threads[i], SIGUSR1);
        }
      }
      signalRecu = 1;
      break;
   default:
      // If the signal is unknown, print a message
      printf("Signal inconnu\n");
      break;
   }
}

/**
 * Handles signals in the client thread.
 * @param signal The signal to handle.
 */
void client_thread_signal_handler(int signal){
   switch (signal) {
   case SIGINT:
      // If the signal is SIGINT (Ctrl+C), print a message
      printf("client : SIGINT\n");
      break;
   case SIGPIPE:
      // If the signal is SIGPIPE (broken pipe), print a message
      printf("client : SIGPIPE\n");
      break;
   case SIGUSR1:
      // If the signal is SIGUSR1 (user-defined signal), print a message
      printf("client : SIGUSR1\n");
      break;
   default:
      // If the signal is unknown, print a message
      printf("Signal inconnu\n");
      break;
   }
}

