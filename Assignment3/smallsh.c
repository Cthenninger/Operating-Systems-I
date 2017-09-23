#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#define DELIM " \t\r\n\v\f"

pid_t foreGRNDpid;	/* had to have these global if i wanted to use in in the signal handlers */


struct Commands{          /* struct to keep track of how many arguments there are from input, as well as a pointer to
                                the specific argument's char pointers, which will be filled when the input line is
                                cleared of whitespace in Getcmd */
        int cmdcount;
        char ** cmds;
        int exitstate;
	int isbackground;	/* 1 denotes background, 0 denotes foreground */
};

void INTHandle(int x){
	signal(SIGINT,INTHandle);
	kill(foreGRNDpid,SIGQUIT);
}

void newcmdstrct(struct Commands **x){
        (*x)->cmds = NULL;
        (*x)->cmdcount=0;
        (*x)->exitstate=0;
	(*x)->isbackground=0;
}

void Getcmd(struct Commands ** commands) {
        int buffer2=50;
        free((*commands)->cmds); /* frees whatever previous commands might have been there */
        (*commands)->cmdcount=0;
        (*commands)->cmds = malloc(buffer2 * sizeof(char*)); /* remallocs for new incoming commands */
        ssize_t buffer1 = 0;
        char * input = NULL;
	printf(": ");
        getline(&input,&buffer1,stdin); /* gets the initial commands from the user */
        char* cmdpoint;
        cmdpoint = strtok(input, DELIM); /* first argument from input without white space */
        while(cmdpoint != NULL){ /* repeats to get all arguments from input after the first one without white space */
                (*commands)->cmds[(*commands)->cmdcount]=cmdpoint;
                (*commands)->cmdcount=(*commands)->cmdcount+1;
                if((*commands)->cmdcount>=buffer2){  /* reallocating space if there is more arguments than initial space */
                        buffer2=buffer2+50;
                        (*commands)->cmds=realloc((*commands)->cmds,buffer2 * sizeof(char *));
                }
                cmdpoint = strtok(NULL,DELIM);
        }
        (*commands)->cmds[(*commands)->cmdcount]=NULL; /* Null will serve as the delimiter for the last command in cmds */
}

void Execs(struct Commands ** commands){
        int waitpidstatus1=0;
	pid_t pid1;
	foreGRNDpid=fork();
	if(foreGRNDpid > 0 && (*commands)->isbackground==1){  
		printf("Creating background process with PID: %d \n",foreGRNDpid);	
	}
	
	if(foreGRNDpid<0){
		perror("Fork Failure");
		(*commands)->exitstate=1;
	}
	else if(foreGRNDpid==0){   
		if(execvp((*commands)->cmds[0],(*commands)->cmds)<0){
			perror("Command not recognized");
			(*commands)->exitstate=1;
			exit(1);
		}	
	}
	else{
		do{		/* this loop runs as long as the child isn't terminated or ends */
			if((*commands)->isbackground==1){ /* checking if process is background */
				pid1=waitpid(-1,&waitpidstatus1,WNOHANG); 
			}
			else{
				signal(SIGINT,INTHandle);
				pid1=waitpid(foreGRNDpid,&waitpidstatus1,WUNTRACED);		
			}
		}while(!WIFEXITED(waitpidstatus1) && !WIFSIGNALED(waitpidstatus1));
	}
	if(waitpidstatus1 != 0 || WIFSIGNALED(waitpidstatus1)){  /* last check of child status */
		(*commands)->exitstate=1;
	}
}

int main(){
        int exit = 0;
        int waitpidstatus=0;
        pid_t pid;
        struct Commands x;
        struct Commands *commands;
        commands=&x;
        newcmdstrct(&commands);
	do{
		fflush(stdout);
		signal(SIGINT, SIG_IGN);		/*  for ensuring CNTL-C commands don't exit shell/background process*/
		pid=waitpid(-1,&waitpidstatus,WNOHANG);
		WTERMSIG(waitpidstatus);
		if(pid>0){
                       	if(waitpidstatus>0){
				printf("Process %d Terminated by signal %i \n",pid,waitpidstatus);
			}
			else{
				printf("Background process %d, is done: exit value %i \n",pid,commands->exitstate);
                	}
		}
		commands->isbackground=0;
		fflush(stdout);
		Getcmd(&commands);
		if(commands->cmdcount!=0 && !strcmp(commands->cmds[commands->cmdcount-1],"&")){
                        commands->isbackground=1;
			commands->cmds[commands->cmdcount-1]=NULL;
                        commands->cmdcount=commands->cmdcount-1;
                }
		if(commands->cmdcount==0){
                        
		}
		else if(!strcmp(commands->cmds[0],"exit") && commands->cmdcount==1){     /* if block for single term commands */
                        exit = 1;
                        commands->exitstate=0;
                }
                else if(!strcmp(commands->cmds[0],"cd")){
                        if(commands->cmdcount==1){
                                chdir(getenv("HOME"));
                                commands->exitstate=0;
			}
                        else if(commands->cmdcount==2){
                                if(chdir(commands->cmds[1])==-1){
                                        perror("cd Error");
                                        commands->exitstate=1;
				}
                                else{
                                        commands->exitstate=0;
				}
                        }
                        else{
                                perror("cd Error: Too Many Arguments");
                        }
                }
                else if(!strcmp(commands->cmds[0],"status") && commands->cmdcount==1){
			if(waitpidstatus>0){
				printf("Terminated by signal %i \n",waitpidstatus);
			}
			else{	
				printf("Exit Value: %i \n",commands->exitstate);
                	}
		}
                else if(commands->cmdcount==3 && ((!strcmp(commands->cmds[1],"<")) || (!strcmp(commands->cmds[1],">")))){
			int out;
			int in;
			out = dup(1);
			in = dup(0);
			int file;
			if(!strcmp(commands->cmds[1],"<")){		/* for reading file redirection */
				file = open(commands->cmds[2], O_RDONLY);
				if(file==-1){
					printf("The file you entered, %s, could not be found. \n",commands->cmds[2]);
					commands->exitstate=1;
				}
				else{
					commands->cmds[1]=NULL; /* Got errors when I left "<" in */ 
					dup2(file,0);
					close(file);
					Execs(&commands);		
				}
			}
			else{						/* for writing file redirection */
				file = open(commands->cmds[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
				if(file==-1){
                                        printf("The file you entered, %s, could not be found. \n",commands->cmds[2]);
                                        commands->exitstate=1;
                                }
				else{
					commands->cmds[1]=NULL; /* Got errors when I left ">" in */
					dup2(file,1);
					close(file);
					Execs(&commands);
				}
			}
			dup2(in,0);
			close(in);
			dup2(out,1);
			close(out);
		}
                else{
                        Execs(&commands);
                }

        }while(!exit);
	signal(SIGQUIT, SIG_IGN);       /* terminates any child processes before it quits the program */
        kill(0,SIGQUIT);
        return 0;
}





