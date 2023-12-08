#ifndef _IMAGE_COMPARISON_H_
#define _IMAGE_COMPARISON_H_

unsigned int compare_image(char *raw_image, char *db_image);

void *compare_images_thread(void *arg);

void handle_threads(char *raw_image, int client_socket);



#endif // _IMAGE_COMPARISON_H_