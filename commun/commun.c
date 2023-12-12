#include <string.h>

#include "commun.h"


int _checked(int ret, char* calling_function){
    if(ret < 0){
        perror(calling_function);
        exit(EXIT_FAILURE);
    }
    return ret;
}

bool lire_exactement(int fd, char* buffer, int size) {
   int lu, i = 0;
   while (i < size && (lu = read(fd, buffer, size - i)) > 0) {
      i += lu;
   }
   
   if (lu < 0) {
      perror("read()");
      return false;
   }
   
   return lu > 0;
}

bool lire_int(int fd, int* value, u_int32_t size) {
    char buffer[sizeof(int)];
    bool result = lire_exactement(fd, buffer, size);
    if (result) {
        memcpy(value, buffer, sizeof(int));
    }
    return result;
}

void clean_str(char *path){
    char *cleaned_path = malloc(strlen(path) + 1);
    int i = 0;
    int j = 0;
    while(path[i] != '\0'){
        if(path[i] != '\n'){
            cleaned_path[j] = path[i];
            j++;
        }
        i++;
    }
    cleaned_path[j] = '\0';
    strcpy(path, cleaned_path);
    free(cleaned_path);
}