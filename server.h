#ifndef __SERVER_H__
#define __SERVER_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <string.h>	//strlen
#include <stdlib.h>	//strlen
#include <unistd.h>	//write
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <pthread.h> //for threading , link with lpthread
#include <netdb.h>
#include <ifaddrs.h>

#define PORT 8888
#define MAX_CLIENT 5
#define MAX_LENGTH 1000

//the thread function
void* connection_handler(void*);
void outputAdrressPortNumber();

#endif /// __SERVER_H__

