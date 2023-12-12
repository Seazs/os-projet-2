#ifndef _CLIENT_MESSAGE_TRANSMISSION_H_
#define _CLIENT_MESSAGE_TRANSMISSION_H_


int send_message(int socket, char *message);

int send_image(int socket, char *image_path);

int send_annonce(int socket, char *annonce);

int receive_message(int socket, char *message);

int receive_number(int socket, int *number);

int receive_result(int socket, char *buffer, int *number);

#endif