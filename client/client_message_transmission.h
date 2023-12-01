#ifndef _CLIENT_MESSAGE_TRANSMISSION_H_
#define _CLIENT_MESSAGE_TRANSMISSION_H_


int send_message(int socket, char *message);

int send_image(int socket, char *image_path);

int receive_message(int socket, char *message);

void clean_path(char *path);

#endif