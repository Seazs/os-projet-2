#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "../commun/commun.h"
#include "client_message_transmission.h"


int send_annonce(int socket, char *annonce){
    // envoie "annonce" au serveur afin d'annoncer que le client souhaite envoyer un message
    errno = 0;
    uint32_t longueur;
    longueur = strlen(annonce) + 1;
    longueur = htonl(longueur);
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    if(errno != 0){
        perror("write()");
        return 1;
    }
    longueur = ntohl(longueur);
    int ret;
    if((ret = write(socket, annonce, longueur)) < 0){
        if(errno != 0){
            perror("write()");
            return 1;
        }
        perror("write()");
        return 1;
    }
    
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
        return 1;
    }
    clean_str(image_path);
    FILE *image_file = fopen(image_path, "rb");
    if(image_file == NULL){
        perror("fopen()");
        return 1;
    }
    // envoie "img" au serveur afin d'annoncer que le client souhaite envoyer une image
    if(send_annonce(socket, "img")){
        fclose(image_file);
        return 1;
    }
    
    // récupère la taille de l'image
    
    fseek(image_file, 0, SEEK_END);
    uint32_t longueur = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);

    // printf("client: Taille de l'image: %d\n", longueur);
    // printf("client: Envoi de l'image: %s\n", image_path);


    uint32_t longueur_net = htonl(longueur);
    // envoie la taille de l'image
    checked_wr(send(socket, &longueur_net, sizeof(longueur_net), 0));

    
    char *raw_image = (char *)malloc(longueur);
    if(raw_image == NULL){
        perror("malloc()");
        fclose(image_file);
        return 1;
    }

    fread(raw_image, 1, longueur, image_file);

    // printf("client: raw_image: %s\n", raw_image);

    // envoie l'image
    checked_wr(send(socket, raw_image, longueur, 0));
    free(raw_image);
    fclose(image_file);
    //printf("client: Image envoyée: %s\n", image_path);
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
    //printf("client: Message reçu: %s\n", buffer);
    return 0;
}
