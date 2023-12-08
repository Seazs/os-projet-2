#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


#include "database.h"

DB_Images_Path db_images_path = {
    .images_path = NULL,
    .file_count = 0
    };


int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void init_images_path(){
   DIR *d;
   struct dirent *dir;

   d = opendir("img/");
   if (d == NULL) {
        perror("erreur lors de l'ouverture du dossier img");
        exit(1);
   }

   while ((dir = readdir(d)) != NULL) {
        db_images_path.images_path = realloc(db_images_path.images_path, sizeof(char*) * (db_images_path.file_count + 1));
        db_images_path.images_path[db_images_path.file_count] = malloc(sizeof(char) * (strlen(dir->d_name) + 1));
        
        
        strcpy(db_images_path.images_path[db_images_path.file_count], "./img/");
        strcat(db_images_path.images_path[db_images_path.file_count], dir->d_name);
        clean_str(dir->d_name);
        
        db_images_path.file_count++;
   }
   closedir(d);
   qsort(db_images_path.images_path, db_images_path.file_count, sizeof(char*), compareStrings);
}

void clean_images_path(){
   for(int i = 0; i < db_images_path.file_count; i++){
      free(db_images_path.images_path[i]);
   }
   free(db_images_path.images_path);
}