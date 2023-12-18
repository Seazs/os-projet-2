#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include "io.h"

read_state_t ReadLine(int fd, char* buffer, size_t size, unsigned int *i) {
   int read_val = '\0';

   for (; *i < size && read_val != '\n'; ++*i) {
      errno = 0;
      read_val = read(fd, &buffer[*i], 1);
      
      if (read_val == 0) {
         return READ_EOF;
      } else if (read_val < 0) {
         if (errno != EINTR) {
            perror("read_line()");
            return READ_ERROR;
         } else if (errno == EINTR) {
            printf("read_line() : EINTR\n");
            return READ_INTERRUPTED;
         }
      }
      
      read_val = buffer[*i];
   }
   
   if (*i >= size) {
      buffer[size - 1] = '\0';
      fprintf(stderr, "[%d] data is too large to be stored in the buffer.\n", getpid());
      return READ_TOO_LARGE;
   }
   
   buffer[*i] = '\0';
   
   // Traitement supplémentaires pour les redirections de
   // stdin via des fichiers suivant la convention de fin 
   // de ligne Windows (\r\n au lieu de \n).
   if (*i >= 2 && buffer[*i - 2] == '\r') {
      --*i;
      buffer[*i] = '\0', buffer[*i - 1] = '\n';
   }
   
   return READ_OK;
}

bool WriteMessage(const int fd, const char message[], size_t size) {
   int ret, remaining = size;
   
   if (size == 0)
      return true;
   
   do {
      errno = 0;
      ret = write(fd, &message[size - remaining], remaining);
      if (ret > 0) {
         remaining -= ret;
      }
   } while ((ret < 0 && errno == EINTR) || (ret > 0 && remaining > 0));
   
   return ret > 0;
}

