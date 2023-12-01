#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "handle_client.h"

typedef struct {
    int socket;
    
} Client;

void * handle_client(void* socket){
    Client client = *(Client*)socket;
    int client_socket = client.socket;
    
    handle_comparison(client_socket);

    close(client_socket);
    free(socket);
    free(client.socket);
    
}

void * handle_comparison(int client_socket){
    char raw_image[20000];
    FILE *image_file = fopen("image_recue.bmp", "wb");
    receive_image(client_socket, raw_image, image_file);
    fclose(image_file);
    system("xdg-open image_reçue.bmp");
    printf("Image reçue\n");

    
}