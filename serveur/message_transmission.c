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
    //printf("server: Message envoyé\n");
    return 0;
}

int send_number(int socket, int number){
    uint32_t longueur = sizeof(number);
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    longueur = ntohl(longueur);
    checked_wr(write(socket, &number, longueur));
    return 0;
}
/*
*/
int receive_image(int socket, Image* image, FILE *image_file){


    // récupère la taille de l'image
    long longueur;
    recv(socket, &longueur, sizeof(longueur), 0);
    if (longueur == 0) {
        printf("Erreur lors de la réception de la taille de l'image\n");
        return 1;
    }
    else if (longueur > 20000) {
        printf("Erreur : image trop grande, veuillez a rentrer une image < 20ko\n");
        return 1;
    }
    image->taille = longueur;
    image->raw_image = (char *)malloc(longueur);
    // reçoit l'image
    recv(socket, image->raw_image, longueur, 0);
    // écrit l'image dans un fichier
    fwrite(image->raw_image, 1, longueur, image_file);
    printf("server: Image reçue\n");
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
    clean_str(buffer);
    return 0;
}