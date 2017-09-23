#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>

#define IP      "127.0.0.1"
#define ID		20

int getNum(char x){
	char alphabet[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	int i;
	for(i=0;i<26;i++){
		if(x==alphabet[i]){
			return i;
		}
	}
	return -1;
}

char getChar(int x){
	char alphabet[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	if(x<0){
		x=x+26;
	}
	 int file;
	return alphabet[x];
}

void doDecrypt(char * crypt,char * plain,char * key, int length){
	int i,x;
	for(i=0;i<length;i++){
		if(crypt[i]==' '){
			plain[i] = ' ';
		}
		else{
			x=((getNum(crypt[i]))-(getNum(key[i])));
			plain[i] = getChar(x);
		}
	}

}
int main(int argc, char *argv[]){
/* ____________ DAEMONIZE ______________________________ */

	pid_t pid;
	pid = fork();		/* First fork */
	if (pid < 0){
		perror ("Fork");
		exit(1);
	}
	if (pid > 0){	/* kills parent process on success */
		exit(0);
	}
	if (setsid() < 0){	/* child is now session leader */
		perror("setsid");
		exit(1);
	}
	pid = fork();   	/* Second Fork */
	if (pid < 0){
		perror("Fork");
		exit(1);
	}
	if (pid > 0){                   
		exit(0);
	}

/*_______ CREATE SOCKETS ____________________________________ */

	int listenPort,listenSocket,talkSocket;
	sscanf (argv[1],"%d",&listenPort);
	struct sockaddr_in listener, encrypter;		/* info of the two sockets */
	listener.sin_family = AF_UNIX;
	listener.sin_addr.s_addr =inet_addr(IP);
	listener.sin_port = htons(listenPort);          
	socklen_t addr_size;
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	/* creates listener socket */
		perror("socket");
		exit(1);
	}
	if (bind(listenSocket, (struct sockaddr *)&listener, sizeof(listener)) == -1) {	/*Binds socket to struct info */
		perror("bind");
		exit(1);
	}
	
/*__________ LISTEN LOOP _____________________________________________ */

	for(;;){
		if ((talkSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  /* creates socket */
		perror("socket");
		exit(1);
		}
		if(listen(listenSocket,5)==-1){
			perror("Listen");
			exit(1);
		}
		if ((talkSocket = accept(listenSocket, (struct sockaddr *)&encrypter, &addr_size)) == -1) {   
			perror("Accept");
			exit(1);
		}
		pid = fork(); 	/* third Fork */
		if (pid < 0){
			exit(1);
		}
		if (pid > 0){ 	/* parent closes accepted socket, repeates listening loop */
			close(talkSocket);
		}
		else {
			int len,thisPID, btLength,textLen,textlen;
			int id = 20;
			close(listenSocket);    /* child closes listen socket, proceeds to communicating */
                        thisPID = getpid();
                        send(talkSocket,&thisPID,sizeof(thisPID),0);    /* send pid to process for termination if error */
                        send(talkSocket,&id,sizeof(ID),0);              /* sends ID to identify as decoder daemon */
                        btLength=recv(talkSocket,&textLen,sizeof(textLen),0);   /*get the length of the text */
                        char textLine[1024];
                        char key[1024];
                        char plaintext[1024];
                        int total;
                        while((total = recv(talkSocket,textLine,1024,0))!=1024){ /*recieve text loop */
                                if(total==-1){break;}
                        }
                        total = 0;
                        while((total=recv(talkSocket,key,1024,0))!=1024){       /*recieve key loop */
                                if(total == -1) { break; }
                        }
                        doDecrypt(textLine,plaintext,key,textLen);      /*preform decryption */
                        len = 1024;
                        total = 0;
                        int bytesleft = len;
                        while(total < len) {            /*sending decrypted data loop*/
                                btLength = send(talkSocket, plaintext, bytesleft, 0);
                                if (btLength == -1) { break; }
                                total += btLength;
                                bytesleft -= btLength;
                        }
			close(talkSocket);
			exit(0);
		}
	}
}


