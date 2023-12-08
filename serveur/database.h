#ifndef DATABASE_H
#define DATABASE_H

#include <stdlib.h>

typedef struct{
   char** images_path; 
   int file_count;
} DB_Images_Path;

extern DB_Images_Path db_images_path;

// extern DB_Images_Path db_images_path;

int compareStrings(const void *a, const void *b);

void init_images_path();

void clean_images_path();

#endif