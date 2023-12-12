#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>

#include "handle_client.h"
#include "image_comparison.h"

typedef struct {
    int socket;
    
} Client;



void * handle_client(void* socket){
    Client client = *(Client*)socket;
    int client_socket = client.socket;
    
    handle_server_response(client_socket);

    close(client_socket);
    free(socket);
    free(client.socket);
    
}


void handle_image(int serveur_socket) {
    // image de max 20000 octets
    Image *image = (Image *)malloc(sizeof(Image));
    printf("ouverture du fichier image_recue.bmp\n");
    FILE *image_file = fopen("image_recue.bmp", "wb");
    if(!receive_image(serveur_socket, image, image_file)){
        printf("raw_image : %s\n", image->raw_image);
        fclose(image_file);
        //send_message(serveur_socket, "Image reçue avec succès");
        handle_threads(image, serveur_socket);
        free(image);
    }
    else{
        printf("Erreur lors de la réception de l'image\n");
    }
    
}

void handle_message(int serveur_socket, char* buffer) {
    if(!receive_message(serveur_socket, buffer)){
        printf("Recu du cote serveur : %s\n", buffer);
        send_message(serveur_socket, buffer);
    }
}

void handle_server_response(int serveur_socket) {
    char buffer[1024];
    bool is_connected = true;

    while(is_connected) {
        receive_message(serveur_socket, buffer);
        printf("Annonce Recu du cote serveur : %s\n", buffer);
        clean_str(buffer);

        if(strcmp(buffer, "img") == 0){
            handle_image(serveur_socket);
        }
        else if(strcmp(buffer, "message") == 0){
            handle_message(serveur_socket, buffer);
        }
        // else if(strcmp(buffer, "exit") == 0){
        //     printf("Client déconnecté\n");
        //     is_connected = false;
        // }
        else{
            printf("Erreur, pas de signal d'annonce reçu\n");
            is_connected = false;
        }
        // wait 5 seconds
    }
}

void accept_connections(int server_socket) {
    while(1) {
        Client* client = (Client*)malloc(sizeof(Client));
        client->socket = accept(server_socket, NULL, NULL);
        if(client->socket < 0) {
            perror("accept");
            continue;
        }

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, handle_client, client) != 0) {
            perror("pthread_create");
            continue;
        }
    }
}