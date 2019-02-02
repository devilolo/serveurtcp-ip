#ifndef WRITE_H
#define WRITE_H

#include <stdio.h>
#include <sys/types.h> /*Pour bind, socket, listen,connect*/
#include <sys/socket.h>
#include <netdb.h> /*pour gethostbytagname*/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

char * lectureSocket(int sock);
char ** getNomFichiers(int *value);
char **decoupage(char *liste,int *i);
char *chaineChar();
void writeSocketEntier(int sock,int value);
int readSocketEntier(int sock);
int tailleFichier(char *f);
char* itoa(int num, char* str);

#endif