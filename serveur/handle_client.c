#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

void * handle_comparison(int client_socket){
    char *raw_image;
    FILE *image_file = fopen("image_recue.bmp", "wb");
    receive_image(client_socket, raw_image, image_file);
    fclose(image_file);
    system("xdg-open image_reçue.bmp");
    printf("Image reçue\n");

    
}
void handle_image(int serveur_socket) {
    // image de max 20000 octets
    char *raw_image;
    printf("ouverture du fichier image_recue.bmp\n");
    FILE *image_file = fopen("image_recue.bmp", "wb");
    if(!receive_image(serveur_socket, raw_image, image_file)){
        send_message(serveur_socket, "Image reçue avec succès");
        handle_threads(raw_image, serveur_socket);
    }
    else{
        printf("Erreur lors de la réception de l'image\n");
    }
    fclose(image_file);
}

void handle_message(int serveur_socket, char* buffer) {
    printf("Server:  Message reçu \n");
    if(!receive_message(serveur_socket, buffer)){
        printf("Recu du cote serveur : %s\n", buffer);
        if (strcmp(buffer, "exit") == 0) {
            printf("Serveur déconnecté\n");
            send_message(serveur_socket, "serveur déconnecté");
            return;
        }
        send_message(serveur_socket, buffer);
    }
}

void handle_server_response(int serveur_socket) {
    char buffer[1024];
    receive_message(serveur_socket, buffer);
    printf("Annonce Recu du cote serveur : %s\n", buffer);
    clean_str(buffer);

    if(strcmp(buffer, "img") == 0){
        handle_image(serveur_socket);
    }
    else if(strcmp(buffer, "message") == 0){
        handle_message(serveur_socket, buffer);
    }
    else{
        printf("Erreur, pas de signal d'annonce reçu\n");
    }
}