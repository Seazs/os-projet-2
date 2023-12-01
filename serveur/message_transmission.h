#ifndef _MESSAGE_TRANSMISSION_H_
#define _MESSAGE_TRANSMISSION_H_


int send_message(int socket, char *message);
int receive_image(int socket, char* raw_image[], FILE *image_file);

int receive_message(int socket, char *message);

#endif