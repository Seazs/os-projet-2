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
   }
   
   return lu > 0;
}