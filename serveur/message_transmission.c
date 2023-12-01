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

int receive_image(int socket, char* raw_image[], FILE *image_file){
    int bytes_read;
    // récupère la taille de l'image
    long longueur;
    //lire_exactement(socket, (char*)&longueur, sizeof(longueur));
    recv(socket, &longueur, sizeof(longueur), 0);



    // reçoit l'image
    recv(socket, raw_image, longueur, 0);

    // écrit l'image dans un fichier
    fwrite(raw_image, 1, longueur, image_file);

    

    printf("server: Image reçue\n");
    return 0;
}


int receive_message(int socket, char *buffer){
    uint32_t longueur;
    printf("1\n");
    if(!lire_exactement(socket, (char*)&longueur, sizeof(longueur))){
        return 1;
    }
    printf("2\n");
    if(!lire_exactement(socket, buffer, ntohl(longueur))){
        return 1;
    }
    printf("server: Message reçu\n");
    return 0;
}