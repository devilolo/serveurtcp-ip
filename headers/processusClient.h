#ifndef PROCESSUSCLIENT_H
#define PROCESSUSCLIENT_H

#include <stdio.h>
#include <sys/types.h> /*Pour bind, socket, listen,connect*/
#include <sys/socket.h>
#include <netdb.h> /*pour gethostbytagname*/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "struct.h"
#include <fcntl.h>
#include "Write.h"
#include <dirent.h>
#include <signal.h>
#include <arpa/inet.h>


int processusClient(int sock);
void recuperationDesFichiers(int sock,int nbfic);
int quitServeur();
void processConsult(int sock,int commande);
void processExport(int sock);
void envoieDesFichiers(int sock,char *lesimports);
void ouvertureFichier(char *nom);

#endif