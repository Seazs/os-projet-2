#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../commun/commun.h"
#include "client_message_transmission.h"

int send_message(int socket, char *message){
    
    uint32_t longueur = strlen(message) + 1;
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    longueur = ntohl(longueur);
    checked_wr(write(socket, message, longueur));
    printf("client: Message envoyé : %s\n", message);
    return 0;
}

int send_image(int socket, char *image_path){
    FILE *image_file = fopen(image_path, "rb");
    if(image_file == NULL){
        perror("fopen()");
        return 1;
    }

    char buffer[1024];
    int bytes_read;
    // récupère la taille de l'image
    long longueur = 0;
    fseek(image_file, 0, SEEK_END);
    longueur = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    // envoie la taille de l'image
    checked_wr(write(socket, &longueur, sizeof(longueur)));

    while((bytes_read = fread(buffer, 1, sizeof(buffer), image_file)) > 0){
        
        checked_wr(write(socket, buffer, bytes_read));
    }

    fclose(image_file);
    printf("client: Image envoyée: %s\n", image_path);
    return 0;
}

int receive_message(int socket, char *buffer){
    uint32_t longueur;
    if(!lire_exactement(socket, (char*)&longueur, sizeof(longueur))){
        return 1;
    }
    if(!lire_exactement(socket, buffer, ntohl(longueur))){
        return 1;
    }
    printf("client: Message reçu: %s\n", buffer);
    return 0;
}