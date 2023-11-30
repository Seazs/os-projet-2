#ifndef COMMUN_H_
#define COMMUN_H_

#define PORT 5555
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int _checked(int ret, char* calling_function);

bool lire_exactement(int fd, char* buffer, int size);


// The macro allows us to retrieve the name of the calling function
#define checked(call) _checked(call, #call)

// Même macro que checked mais pour write() (où 0 signifie
// aussi une erreur).
#define checked_wr(call) _checked(((call) - 1), #call)

#endif



