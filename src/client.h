#ifndef CLIENT_H
#define CLIENT_H

static void enterSC(int semId);
static void exitSC(int semId);
static int myOpen(const char * pipeId, int parameter);
static void myClose(int pipeId);
static void exitWaiting(int semaphoreId);

#endif