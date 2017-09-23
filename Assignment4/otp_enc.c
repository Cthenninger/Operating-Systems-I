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

#define IP                      "127.0.0.1"
#define DECODER         20

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int badChars(char * line,int length){  /* check to make sure each character is either an alphabet character or a space */
        int i;
        for(i=0;i++;i<length){
        	if(line[i]<'A' || line[i] > 'Z'){
			if(line[i]==' '){}
			else{
				return -1;
			}
		}
        }
        return 0;
}

void main(int argc, char *argv[]){
/* ______ PARAMETER BINDING ______________________________ */
        
		int serverPort;                                                 /* target port */
        char * inFile;
        char * keyName;                                 /* pointer to plain text file name, key file name */
        inFile = strdup(argv[1]);
        keyName = strdup(argv[2]);
        sscanf (argv[3],"%d",&serverPort);

/* ________ GRABBING KEY & PLAIN TEXT ______________________ */
        
		int textLen, keyLen;                            /* lengths for lines to help w/ testing key & socket sending */
        FILE * plainFile;
        FILE * keyFile;
        char * plaintext=NULL;
        char * key=NULL;  		/* for the contents of the file, key */
		ssize_t len1, len2 = 0;
        if((plainFile = fopen(inFile, "r"))== NULL){  /* Open File to be Encrypted */
                error("Opening Text File");
        }
        if((textLen = getline(&plaintext,&len1,plainFile))==-1){ /* Get text to be Encrypted */
                error("Reading Text File");
        }
		fclose(plainFile);
        free(inFile);
        if((keyFile = fopen(keyName, "r"))==NULL){
                error("Opening Key File");
        }
        if((keyLen = getline(&key,&len2,keyFile))==-1){
                error("Reading Key File");
        }
        
        fclose(keyFile);
        free(keyName);
        if(textLen > keyLen){                                           /* Check that key length and text length match */
                perror("Key Length too short");
                exit(1);
        }
/* _________ SOCKET CREATION _________________________________*/

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
		
/* _________ COMMUNICATION ___________________________________ */

        if (connect(socket1,(struct sockaddr *) &Sender,sizeof(Sender)) < 0){
                perror("Bad Port");
                exit(2);
        }
        else{
                int btLength, daemonPID, identifier;
                btLength = recv(socket1,&daemonPID,sizeof(daemonPID),0); /*gets daemon's pid to kill it if there's an error */
                btLength = recv(socket1, &identifier, sizeof(identifier),0); /* gets identifier to make sure it's not enc_d */
                if(identifier == DECODER){
                        kill(daemonPID, SIGQUIT);
                        perror("Cannot Access Decoder port");
                        exit(2);
                }
				int textlen = strlen(plaintext);
				int len = 1024;
				char keynu[1024];
				char textnu[1024];
				strncpy(textnu,plaintext,1024);
				strncpy(keynu,key,1024);
				free(plaintext);
				free(key);
				fflush(stdout);
				if((badChars(textnu,textLen))==-1){
                			printf("Textfile '%s' contains illegal characters \n",textnu);
               			 	kill(daemonPID,SIGQUIT);
					exit(1);
			        }

				send(socket1,&textlen,sizeof(textlen),0);
				int total = 0;        /* how many bytes we've sent*/
				int bytesleft = len; /* how many we have left to send*/
				while(total < len) {	/* send plaintext */
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
                		char crypttext[1024];
                		bzero((char *)crypttext,sizeof(crypttext));
				total = 0;
                        	while((total = recv(socket1,crypttext,1024,0))!=1024){
                                	if(total == -1) { break; }
                        	}
				printf("%s",crypttext);
                	
				
        }
        close(socket1);
        exit(0);
}


                     


