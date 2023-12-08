#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../commun/commun.h"
#include "client_message_transmission.h"

int send_annonce(int socket, char *annonce){
    // envoie "annonce" au serveur afin d'annoncer que le client souhaite envoyer un message
    uint32_t longueur;
    longueur = strlen(annonce) + 1;
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    longueur = ntohl(longueur);
    checked_wr(write(socket, annonce, longueur));
    return 0;
}

int send_message(int socket, char *message){
    // envoie "message" au serveur afin d'annoncer que le client souhaite envoyer un message
    send_annonce(socket, "message");

    // envoie le message
    uint32_t longueur = strlen(message) + 1;
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    longueur = ntohl(longueur);
    checked_wr(write(socket, message, longueur));
    printf("client Message envoyé : %s\n", message);
    return 1;
}

int send_image(int socket, char *image_path){

    
    // vérifie que l'extension du fichier est bien .bmp
    if (strstr(image_path, ".bmp") == NULL){
        return 0;
    }
    clean_str(image_path);
    FILE *image_file = fopen(image_path, "rb");
    if(image_file == NULL){
        perror("fopen()");
        return 0;
    }
    // envoie "img" au serveur afin d'annoncer que le client souhaite envoyer une image
    send_annonce(socket, "img");
    
    // récupère la taille de l'image
    
    fseek(image_file, 0, SEEK_END);
    long longueur = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    // envoie la taille de l'image
    checked_wr(send(socket, &longueur, sizeof(longueur), 0));

    
    char *raw_image = (char *)malloc(longueur);
    if(raw_image == NULL){
        perror("malloc()");
        fclose(image_file);
        return 0;
    }

    fread(raw_image, 1, longueur, image_file);

    // envoie l'image
    checked_wr(send(socket, raw_image, longueur, 0));
    free(raw_image);

    fclose(image_file);
    printf("client: Image envoyée: %s\n", image_path);
    return 1;
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

