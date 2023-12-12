#ifndef _MESSAGE_TRANSMISSION_H_
#define _MESSAGE_TRANSMISSION_H_

#include "handle_client.h"


int send_message(int socket, char *message);

int send_number(int socket, int number);

int receive_image(int socket, Image *image, FILE *image_file);

int receive_message(int socket, char *message);

#endif