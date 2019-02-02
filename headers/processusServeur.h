#ifndef PROCESSUSServeur_H
#define PROCESSUSServeur_H


#include <stdio.h>
#include <sys/types.h> /* Consultez NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>          /* Consultez NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "struct.h"
#include <dirent.h>
#include <string.h>
#include "Write.h"
#include <sys/file.h>
#include <signal.h>


void processImport(int sock);
int processusFils(int SocketListener,int Socketfils);
void processusConsult(int sock,int commande);
void finProcessusFils();
void processusExport(int sock);
int verifMimeType(char tmp[30]);
int closePipe(int tunnel[2]);
void signalHandler();


#endif