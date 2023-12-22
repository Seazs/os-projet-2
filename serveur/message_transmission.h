#ifndef _MESSAGE_TRANSMISSION_H_
#define _MESSAGE_TRANSMISSION_H_

#include <stddef.h>

#include "handle_client.h"


typedef enum {
   READ_INTERRUPTED,
   READ_EOF,
   READ_ERROR,
   READ_TOO_LARGE,
   READ_OK
} read_state_t;


int send_message(int socket, char *message);

int receive_image(int socket, Image *image);

int receive_message(int socket, char *message);

#endif