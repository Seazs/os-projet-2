#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "database.h"
#include "../commun/commun.h"

// Global database of image paths
DB_Images_Path db_images_path = {
    .images_path = NULL,
    .file_count = 0
};

/**
 * Compares two strings.
 * @param a The first string.
 * @param b The second string.
 * @return A negative integer if a < b, 0 if a == b, a positive integer if a > b.
 */
int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/**
 * Initializes the database of image paths.
 */
void init_images_path(){
   DIR *d;
   struct dirent *dir;

   // Open the directory
   d = opendir("img/");
   if (d == NULL) {
        perror("Error opening the img directory");
        exit(1);
   }

   // Read each file in the directory
   while ((dir = readdir(d)) != NULL) {
         // Skip the current directory and parent directory
         if(strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0){
               continue;
         }
        // Allocate space for the new image path
        db_images_path.images_path = realloc(db_images_path.images_path, sizeof(char*) * (db_images_path.file_count + 1));
        db_images_path.images_path[db_images_path.file_count] = malloc(sizeof(char) * (strlen(dir->d_name) + 1));
        
        // Construct the image path
        strcpy(db_images_path.images_path[db_images_path.file_count], "img/");
        strcat(db_images_path.images_path[db_images_path.file_count], dir->d_name);
        clean_str(dir->d_name);
        
        // Increment the file count
        db_images_path.file_count++;
   }
   // Close the directory
   closedir(d);
   // Sort the image paths
   qsort(db_images_path.images_path, db_images_path.file_count, sizeof(char*), compareStrings);
}

/**
 * Cleans up the database of image paths.
 */
void clean_images_path(){
   // Free each image path
   for(int i = 0; i < db_images_path.file_count; i++){
      free(db_images_path.images_path[i]);
   }
   // Free the array of image paths
   free(db_images_path.images_path);
}