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



void main(int argc, char *argv[]){
	int length = atoi(argv[1]);
	char * fileName = strdup(argv[3]);
	char key[length];
	int mykey;
	time_t t;
	srand((unsigned) time(&t));
	int i;
	int x = length;
	for(i=0;i<length;i++){
		key[i] = ((rand() % 25) +65);
	}
	printf("%s",key);
	free(fileName);
	exit(0);
}
