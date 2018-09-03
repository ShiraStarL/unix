
#include <stdio.h>
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
#include <fcntl.h>
#include <iostream>



#define PORT 0x0da2
#define IP_ADDR 0x7f000001

using namespace std;

int main(int argc, char *argv[])
{
	int r, fd,nWrite=1;
	char buffer[256];
	int sock = socket(AF_INET, SOCK_STREAM, 0), nrecv;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);

	int TheNumber;
	
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0){
		perror("connect");
		return 1;
	}
	printf("Successfully connected.\n");
	//sending to the server the first guess
	while(1){
		memset(buffer,0,sizeof(buffer));
		while(1){
			cout<< "number between 1000-9999: ";
			string num;
			cin >> TheNumber ;
			//TheNumber = stoi(num);
			if( (TheNumber >= 1000) && (TheNumber<=9999) )
				break;	
		}
		if (send(sock, &TheNumber, sizeof(TheNumber), 0) < 0){
			perror("send");
			return 1;
		}
		//get result from server
		if ((nrecv = recv(sock, buffer, sizeof(buffer), 0)) < 0)
        {
        	perror("recv");
        	return 1;
        }
		cout<< buffer<<endl;
		if(!strcmp(buffer,"you win!"))
			return 0;
	}

    return 0;
}