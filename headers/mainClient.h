#ifndef MAINClient_H
#define MAINClient_H


#include <stdio.h>
#include <sys/types.h> /*Pour bind, socket, listen,connect*/
#include <sys/socket.h>
#include <netdb.h> /*pour gethostbytagname*/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "processusClient.h"
#include <sys/wait.h>

void signalHandler();
void finProcessusFils();

#endif