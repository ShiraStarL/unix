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


#define PORT 0x0da2
#define IP_ADDR 0x7f000001

/*Created by Shira Levy & Sahar Gezer */


int main(int argc, char *argv[])
{
	int r, fd,nWrite=1;
	char path[256];
	char fileName[256];
	char readFromFile[256];
	char buffer[256];
	memset(path,0,sizeof(path));
	memset(fileName,0,sizeof(fileName));
	memset(readFromFile,0,sizeof(readFromFile));
	long long fileSize;
	long userID;
	int sock = socket(AF_INET, SOCK_STREAM, 0), nrecv;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0){
		perror("connect");
		return 1;
	}
	printf("Successfully connected.\n");

	//send client action + file name to server
	if (send(sock, argv[1], sizeof(argv[1])*8, 0) < 0){
		perror("send");
		return 1;
	}
	if (send(sock, argv[2], sizeof(argv[2])*8, 0) < 0){
		perror("send");
		return 1;
	}
	//recive approval for opening file descriptor
	if ((nrecv = recv(sock,buffer, sizeof(buffer), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
	if (!strcmp(buffer,"does not exist on the server.\n")){
		printf("%s - %s",argv[2],buffer);
		return 1;
	}
	//recive file info
	if(!strcmp(argv[1],"get-file-info")){
		//file name
		if ((nrecv = recv(sock, fileName, sizeof(fileName), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
		printf("%s - size: ",fileName);
		//file size
		if ((nrecv = recv(sock, &fileSize, sizeof(fileSize), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
		printf("%lld bytes, owner uid: ",fileSize);
		//file owner uid
		if ((nrecv = recv(sock, &userID, sizeof(userID), 0)) < 0)
		{
			perror("recv");
			return 1;
		}
		printf("%ld \n",userID);
	}
	//download file
	if(!strcmp(argv[1],"download-file")){
		strcpy(path,"clientFiles/");
		strcat(path,argv[2]);
		if((fd = open (path, O_WRONLY | O_CREAT | O_TRUNC, 00700))<0){
			perror("open");
		}
		else while(nWrite){
			if ((nrecv = recv(sock, &nWrite, sizeof(nWrite), 0)) < 0)
			{
				perror("recv");
				return 1;
			}
			if ((nrecv = recv(sock, readFromFile, sizeof(readFromFile), 0)) < 0)
			{
				perror("recv");
				return 1;
			}
			if((nWrite = write(fd,readFromFile,nWrite))<0){
				perror("write");
				return 1;
			}
		}
		printf("%s - download completed.\n",argv[2]);
	}
    return 0;
}