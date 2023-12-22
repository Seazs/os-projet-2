#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "../commun/commun.h"
#include "message_transmission.h"

/**
 * Sends a message over a socket.
 * @param socket The socket to send the message over.
 * @param message The message to send.
 * @return 0 on success, non-zero on failure.
 */
int send_message(int socket, char *message){
    // Calculate the length of the message, including the null terminator
    uint32_t longueur = strlen(message) + 1;
    // Convert the length to network byte order
    longueur = htonl(longueur);
    // Write the length to the socket
    checked_wr(write(socket, &longueur, sizeof(longueur)));
    // Convert the length back to host byte order
    longueur = ntohl(longueur);
    // Write the message to the socket
    checked_wr(write(socket, message, longueur));
    return 0;
}

/**
 * Receives an image over a socket.
 * @param socket The socket to receive the image from.
 * @param image The image to receive.
 * @return 0 on success, non-zero on failure.
 */
int receive_image(int socket, Image* image){
    // Receive the size of the image
    uint32_t longueur;
    recv(socket, &longueur, sizeof(longueur), 0);
    // Convert the size to host byte order
    longueur = ntohl(longueur);
    if (longueur < 0) {
        printf("Erreur lors de la réception de la taille de l'image\n");
        return 1;
    }
    else if (longueur > 20000) {
        printf("Erreur : image trop grande, veuillez a rentrer une image < 20ko\n");
        return 1;
    }
    
    // Set the size of the image and allocate memory for the image data
    image->taille = longueur;
    image->raw_image = (char *)malloc(longueur);
    
    // Receive the image data
    recv(socket, image->raw_image, longueur, 0);
    return 0;
}

/**
 * Receives a message over a socket.
 * @param socket The socket to receive the message from.
 * @param buffer The buffer to store the received message.
 * @return 0 on success, non-zero on failure.
 */
int receive_message(int socket, char *buffer){
    uint32_t longueur;
    // Read the length of the message
    if(!lire_exactement(socket, (char*)&longueur, sizeof(longueur))){
        if (errno = EINTR) {
            return 2;
        }
        return 1;
    }
    
    // Read the message
    if(!lire_exactement(socket, buffer, ntohl(longueur))){
        if (errno = EINTR) {
            return READ_INTERRUPTED;
        }
        return 1;
    }
    // Clean the message
    clean_str(buffer);
    return 0;
}