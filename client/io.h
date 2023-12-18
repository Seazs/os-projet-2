#ifndef IO_H_
#define IO_H_

#include <stdbool.h>
#include <stddef.h>

typedef enum {
   READ_INTERRUPTED,
   READ_EOF,
   READ_ERROR,
   READ_TOO_LARGE,
   READ_OK
} read_state_t;

/**
 * Écrit dans buffer (à partir de l'indice *i) la première ligne du flux
 * référencé par fd. Si aucun '\n' n'est lu après (size - *i) bytes, la
 * fonction s'arrête aussi et retourne READ_TOO_LARGE.
 *
 * @return
 *   • READ_OK : si le caractère '\n' a été rencontré avant d'atteindre
 *     la taille limite size.
 *   • READ_TOO_LARGE : si la ligne était plus longue que (size - *i)
 *     bytes.
 *   • READ_ERROR : si une erreur de lecture s'est produite.
 *   • READ_EOF : si la fin de fichier a été détectée.
 *   • READ_INTERRUPTED : si une interruption est survenue. Certaines
 *     interruptions peuvent ne pas être détectées en fonction des
 *     imbrications ("interleaving") qui ont lieu.
 **/
read_state_t ReadLine(int fd, char* buffer, size_t size, unsigned int *i);

/**
 * Écrit `size` bytes de `message` sur le flux référencé par `fd` en
 * ignorant les éventuels signaux reçus.
 * 
 * @return true en cas de succès et false en cas d'erreur.
 **/
bool WriteMessage(const int fd, const char message[], size_t size);

#endif
