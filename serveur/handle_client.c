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

#define MAX_CONNECTED_CLIENTS 2345 // nombre arbitraire


volatile sig_atomic_t signalRecu = 0;
atomic_int nb_clients = 0;
pthread_t threads[MAX_CONNECTED_CLIENTS];
Client clients[MAX_CONNECTED_CLIENTS];


void * handle_client(void* arg_client){
    // ignorer SIGINT
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
        return NULL;
    }

    // gérer SIGPIPE et SIGUSR1(envoyé par le main_thread lors de la réception de SIGINT)
    struct sigaction action;
    action.sa_handler = client_thread_signal_handler;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGUSR1, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }
    if(sigaction(SIGPIPE, &action, NULL) != 0) {
        perror("sigaction()");
        return NULL;
    }

    Client client = *(Client*)arg_client;
    
    
    handle_server_response(client);
    atomic_fetch_sub(&nb_clients, 1);
    close(client.socket);
    threads[nb_clients-1] = 0;
    free(arg_client);    
    return NULL;
}


void handle_image(int serveur_socket) {
    // image de max 20000 octets
    //printf("Reception d'une image\n");
    Image *image = (Image *)malloc(sizeof(Image));
    if(!receive_image(serveur_socket, image)){
        handle_threads(image, serveur_socket);
        free(image);
    }
    else{
        printf("Erreur lors de la réception de l'image\n");
        free(image);
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
        receive_message(serveur_socket, buffer);
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

void accept_connections(int server_socket) {

    struct sigaction action;
    action.sa_handler = main_signal_handler;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGINT, &action, NULL) != 0) {
        perror("sigaction()");
        return;
    }
    // ignorer SIGPIPE
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if(pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask");
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
        if((client_socket = accept(server_socket, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        printf("Connexion acceptée\n");
        Client* client = (Client*)malloc(sizeof(Client));
        printf("Client connecté\n");
        client->socket = client_socket;
        client->is_connected = true;
        if(client->socket < 0) {
            perror("accept");
            continue;
        }

        pthread_t thread_id;
        threads[nb_clients] = thread_id;
        atomic_fetch_add(&nb_clients, 1);
        if(pthread_create(&thread_id, NULL, handle_client, client) != 0) {
            perror("pthread_create");
            atomic_fetch_sub(&nb_clients, 1);
            continue;
        }

        
        printf("Nouveau Client connecté\n");
    }

    close(server_socket);

}

void main_signal_handler(int signal){
   switch (signal) {
   case SIGINT:
      printf("main : SIGINT\n");
      for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
         if (threads[i] != 0) {
            pthread_kill(threads[i], SIGUSR1);
         }
      }
      signalRecu = 1;
      break;
   case SIGPIPE:
      printf("main : SIGPIPE\n");
      break;
   case SIGUSR1:
      printf("main : SIGUSR1\n");
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