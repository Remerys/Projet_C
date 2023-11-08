#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*****************************************************************************
 * auteur : Gilles Subrenat
 *
 * fichier : myassert.c
 *
 * note :
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include "myassert.h"

void myassert_func(bool condition, const char *message,
                   const char *fileName, const char *functionName, int line,
                   const char *fileNameReal, const char *functionNameReal, int lineReal)
{
    if (! condition)
    {
        fprintf(stderr, "/---------------------------\n");
        fprintf(stderr, "| Erreur détectée !\n");
        // duplication de code pour une meilleure lisibilité
        if (fileNameReal == NULL)
        {
            fprintf(stderr, "|       fichier  : %s\n", fileName);
            fprintf(stderr, "|       ligne    : %d\n", line);
            fprintf(stderr, "|       fonction : %s\n", functionName);
        }
        else
        {
            fprintf(stderr, "|       fichier  : %s (%s)\n", fileName, fileNameReal);
            fprintf(stderr, "|       ligne    : %d (%d)\n", line, lineReal);
            fprintf(stderr, "|       fonction : %s (%s)\n", functionName, functionNameReal);
        }
        fprintf(stderr, "|       pid      : %d\n", getpid());
        fprintf(stderr, "|    Message :\n");
        fprintf(stderr, "|       -> %s\n", message);
        fprintf(stderr, "|    Message systeme:\n");
        fprintf(stderr, "|       -> ");
        perror("");
        fprintf(stderr, "|    On stoppe le programme\n");
        fprintf(stderr, "\\---------------------------\n");
        exit(EXIT_FAILURE);
    }
}
