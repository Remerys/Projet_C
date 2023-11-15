#ifndef MASTER_H
#define MASTER_H

static int myOpen(const char * pipeId, int parameter);
static void myClose(int pipeId);
static void exitWaiting(int semaphoreId);
static int my_semget();
static int my_mkfifo(const char * pipeName);
static void my_destroy_semaphore(int semId);
static void my_destroy_pipe(const char * pipeName);

#endif