#ifndef _IMAGE_COMPARISON_H_
#define _IMAGE_COMPARISON_H_
#include "handle_client.h"

unsigned int compare_image(Image *image, char *db_image);

void *compare_images_thread(void *arg);

void handle_threads(Image *image, Client *client);



#endif // _IMAGE_COMPARISON_H_