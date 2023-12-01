#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../commun/commun.h"
#include "message_transmission.h"

int send_message(int socket, char *message){
    uint32_t longueur = strlen(message) + 1;
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    longueur = ntohl(longueur);
    checked_wr(write(socket, message, longueur));
    printf("server: Message envoyé\n");
    return 0;
}

void receive_image(int socket, char raw_image[], FILE *image_file){
    
    int bytes_read;
    // récupère la taille de l'image
    long longueur = 0;
    lire_exactement(socket, (char*)&longueur, sizeof(longueur));
    longueur = ntohl(longueur);

    char buffer[longueur];
    // reçoit l'image
    while((bytes_read = read(socket, buffer, sizeof(buffer))) > 0){
        strcat(raw_image, buffer);
        fwrite(buffer, 1, bytes_read, image_file);
    }
    printf("server: Image reçue\n");
}


int receive_message(int socket, char *buffer){
    uint32_t longueur;
    if(!lire_exactement(socket, (char*)&longueur, sizeof(longueur))){
        return 1;
    }
    if(!lire_exactement(socket, buffer, ntohl(longueur))){
        return 1;
    }
    printf("server: Message reçu\n");
    return 0;
}