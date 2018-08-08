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
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define QUEUE_LEN 20

int main(int argc, char *argv[])
{
	//open socket
	int listenS = socket(AF_INET, SOCK_STREAM, 0);
	if (listenS < 0){
		perror("socket");
		return 1;
	}
	char readFromFile[256];
	char clientAction[256];
	char fileName[256];
	memset(clientAction,0,sizeof(clientAction));
	memset(readFromFile,0,sizeof(readFromFile));
	memset(fileName,0,sizeof(fileName));
	int nRead;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	long long fileSize;
	long usernID;

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
	//client loop
	while(1){
		nRead=1;
		int newfd = accept(listenS, (struct sockaddr*)&clientIn, (socklen_t*)&clientInSize);
		if (newfd < 0){
			perror("accept");
			return 1;
		}
		printf("Successfully connected.\n");
		
		char msg[256];
		char path[215];
		struct stat fileStatus;
		int fd;
		int nrecv;

		//get client action
		if ((nrecv = recv(newfd, clientAction, sizeof(clientAction), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
		//get file name from client
		if ((nrecv = recv(newfd, fileName, sizeof(fileName), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
		// open file descriptor
		strcpy(path,"files/");
		strcat(path,fileName);
		if((fd = open(path,O_RDONLY))<0){
			perror("open");
			strcpy(msg,"does not exist on the server.\n");
			if (send(newfd,msg, sizeof(msg), 0) < 0){
				perror("send");
				return 1;
			}
			continue;
		}
		if(fstat(fd,&fileStatus)){
			perror("stat");
			return 1;
		}
		strcpy(msg,"fd opened.\n");
		if (send(newfd,msg, sizeof(msg), 0) < 0){
			perror("send");
			return 1;
		}
		// send file info to the client
		if(!strcmp(clientAction,"get-file-info")){
			//printf("shira.txt - size: %lld bytes, owner uid: %ld\n",(long long)fileStatus.st_size, (long)fileStatus.st_uid);
				if (send(newfd, fileName, sizeof(fileName), 0) < 0)
				{
					perror("send");
					return 1;
				}
				fileSize = (long long) fileStatus.st_size;
				if (send(newfd,&fileSize, sizeof(fileSize), 0) < 0)
				{
					perror("send");
					return 1;
				}
				usernID = (long) fileStatus.st_uid;
				if (send(newfd,&usernID, sizeof(usernID), 0) < 0)
				{
					perror("send");
					return 1;
				}
				printf("file info: %lld %ld\n",fileSize, usernID);

		}
		//send file to the client
		else if(!strcmp(clientAction,"download-file")){

			while(nRead){
				//read from file
				if((nRead = read(fd,readFromFile,256))<0){
					perror("read");
					return 1;
				}
				//send to client
				if (send(newfd,&nRead, sizeof(nRead), 0) < 0)
				{
					perror("send");
					return 1;
				}
				if (send(newfd,readFromFile, sizeof(readFromFile), 0) < 0)
				{
					perror("send");
					return 1;
				}
			}
		}
		close(newfd);
	}
	close(listenS);
	return 0;
}
