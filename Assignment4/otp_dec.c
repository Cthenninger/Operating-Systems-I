#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>

#define IP		"127.0.0.1"
#define ENCODER		10

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void main(int argc, char *argv[]){
/*___________ PARAMETER BINDING ______________________ */
	
	int serverPort;							/* target port */
	char * inFile;
	char * keyName = NULL;			/* pointer to crypt text file name, key file name */
	inFile = strdup(argv[1]);			
	keyName = strdup(argv[2]);				
	sscanf (argv[3],"%d",&serverPort);				

/*___________ GRABBING KEY & CRYPT TEXT ________________ */
	
	int textLen;
	int keyLen;				/* lengths for lines to help w/ testing key & socket sending */
	FILE * cryptFile; 
	FILE * keyFile;
	char * cryptText=NULL;
	char * key=NULL;					/* for the contents of the file, key */
	ssize_t len1=0;
	ssize_t len2 = 0; 
	if((cryptFile = fopen(inFile, "r"))== NULL){		/* Open File to be decrypted */	
		error("Opening Text File");
	}
	if(textLen = getline(&cryptText,&len1,cryptFile)==-1){	/* Get text to be decrypted */
		error("Reading Text File");
	}
	fclose(cryptFile);
	free(inFile);
	if((keyFile = fopen(keyName, "r"))==NULL){
		error("Opening Key File");
	}
	if((keyLen = getline(&key,&len2,keyFile))==-1){
		error("Reading Key File");
	}
	fclose(keyFile);
	free(keyName);
	
	if(textLen > keyLen){				/* Check that key length and text length match */
		perror("Key Length is too short");
		exit(1);
	}
	
/* _________ SOCKET CREATION _____________________________*/
	
	int socket1;
	struct sockaddr_in Sender;
	bzero((char *) &Sender, sizeof(Sender));
    Sender.sin_family = AF_INET;
    Sender.sin_addr.s_addr=inet_addr(IP);
    Sender.sin_port = htons(serverPort);
	socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket1 < 0){
        error("opening socket");
    }
	
/* _________ COMMUNICATION ____________________________ */
	
    if (connect(socket1,(struct sockaddr *) &Sender,sizeof(Sender)) < 0){
		perror("Bad Port");
		exit(2);
    }
	else{
		int btLength, daemonPID, identifier,textlen;
		btLength = recv(socket1,&daemonPID,sizeof(daemonPID),0); /*gets daemon's pid to kill it if there's an error */
		btLength = recv(socket1, &identifier, sizeof(identifier),0); /* gets identifier to make sure it's not enc_d */
		if(identifier == ENCODER){
		kill(daemonPID, SIGQUIT);
			perror("Cannot Access Encoder port");
			exit(2);
		}
		textlen= strlen(cryptText);
		int len = 1024;
		char keynu[1024];
		char textnu[1024];
		strncpy(textnu,cryptText,1024);
		strncpy(keynu,key,1024);
		free(cryptText);
		free(key);
		send(socket1,&textLen,sizeof(textlen),0);	/*sending the size of the incoming data to server */
		int total = 0;        /* how many bytes we've sent*/
		int bytesleft = len; /* how many we have left to send*/
		while(total < len) {	/* send text */
			btLength = send(socket1, textnu, bytesleft, 0);
			if (btLength == -1) { break; }
			total += btLength;
			bytesleft -= btLength;
		}
		total = 0;
		bytesleft = len;
		while(total < len) {	
			btLength = send(socket1, keynu, bytesleft, 0);
			if (btLength == -1) { break; }
			total += btLength;
			bytesleft -= btLength;
		}
		char plaintext[1024];
		bzero((char *)plaintext,sizeof(plaintext));
		total = 0;
		while((total = recv(socket1,plaintext,1024,0))!=1024){  /*get plain text. for some reason, always empty*/
			if(total == -1) { break; }
		}
			printf("%s\n",plaintext);
		
	}
	close(socket1);
	exit(0);
}


