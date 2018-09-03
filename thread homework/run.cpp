#include "ThreadPoolManager.h"
#include "functions.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define QUEUE_LEN 20

using namespace std;

int main (int argc, const char* argv[]){
    ThreadPoolManager threadPool;
    int n = threadPool.ThreadPoolInit(3);

    pthread_create(&(threadPool.master),NULL,(f)executeTasks,(void*)&threadPool);	   

    //SERVER
    
    struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	//open socket
	int listenS = socket(AF_INET, SOCK_STREAM, 0);
	if (listenS < 0){
		perror("socket");
		return 1;
	}

    if (bind(listenS, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		perror("bind");
		return 1;
	}

	if (listen(listenS, QUEUE_LEN) < 0)
	{
		perror("listen");
		return 1;
	}

	struct sockaddr_in clientIn;
	int clientInSize = sizeof clientIn;
	int guessingNum,nrecv;
    while(1){
	
        int newfd = accept(listenS, (struct sockaddr*)&clientIn, (socklen_t*)&clientInSize);
		if (newfd < 0){
			perror("accept");
			return 1;
		}
		int* np =  &newfd;
		cout<<newfd<<endl;

		/** I tried to do it with threads but it does not work*/	
		//Task t1(&playGame,(void*)np);
		//n = threadPool.ThreadPoolInsertTask(t1);

		playGame((void*)np);
    }
    //////////////////


    threadPool.ThreadPoolDestroy();
    return 0;
}