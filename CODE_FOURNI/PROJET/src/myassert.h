/*****************************************************************************
 * auteur : Gilles Subrenat
 *
 * fichier : myassert.h
 *
 * note :
 *     Utiliser uniquement la macro myassert
 *        arg1 : booleen : si false, une erreur est déclenchée et le
 *                         programme s'arrête
 *        arg2 : string  : message à afficher en cas d'erreur
 *     note : définir la macro NDEBUG désactive le myassert
 * note :
 *     La macro myassert2 permet de préciser explicitement un nom de fichier,
 *     un nom de fonction et un numéro de ligne différents de ceux où la
 *     macro est appelée
 *
 * exemple d'appel :
 *   void f(int n)
 *   {
 *       myassert(n > 0, "n doit être strictement positif");
 *       ...
 *   }
 *   si NDEBUG est défini
 *       il ne se passe rien et la fonction s'exécute sans erreur ou non
 *       selon la valeur de n (mais il y aura vraisemblalement une erreur)
 *   sinon si n est positif
 *       il ne se passe rien et la fonction s'exécute sans erreur
 *   sinon
 *       le programme s'arrête avec un message d'erreur complet
 *****************************************************************************/

#ifndef MYASSERT_H
#define MYASSERT_H

    #ifndef NDEBUG
        #include <stdbool.h>
        #define myassert(condition, message) myassert_func((condition), (message), __FILE__, __func__, __LINE__, NULL, NULL, -1)
        #define myassert2(condition, message, filename, functionName, line) myassert_func((condition), (message), __FILE__, __func__, __LINE__, (filename), (functionName), (line))
        // do not use directly
        void myassert_func(bool condition, const char *message,
                           const char *filename, const char *functionName, int line,
                           const char *fileNameReal, const char *functionNameReal, int lineReal);
    #else
        #define myassert(condition, message)
        #define myassert2(condition, message, filename, functionName, line)
    #endif

#endif
