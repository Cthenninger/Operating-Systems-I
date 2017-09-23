#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
/*****************************************************************************
 * Author: Charles Henninger, 931927607
 * Date:February 8,2016
 * Operating Systems Assigment 2
 * **************************************************************************/

/****************************************************************************
 * Name: Room Structure
 * Description:struct to keep all relevent information for a room variable 
 * 		for easy access when randomizing connections and names
 * *************************************************************************/

struct Room{
        int numcon;
        char kind[6];
        char name[11];
        int connections[6];
}rooms[7];


/****************************************************************************
*  Name: Name Structure
*  Description: A struct containing a char array. really only used because
*  		it is easy to get a jagged array with this method.
* 		Contains the ten names of which seven will be used randomly
*  *************************************************************************/

struct Names{
	char title[11];
}names[10];


/****************************************************************************
 * *  Name: Paths Structure
 * *  Description: pretty much the same as the name struct. could have just 
 * 		   used the first and made two more arrays, but oh well.
 * 		   the paths array of this struct is for keeping track of the
 * 		   player's selected route. the cons is used as a formatting 
 * 		   aid when printing out the possible connections for each room
 * *  *************************************************************************/


struct Paths{
	char leg[11];
}paths[20],cons[6];


/****************************************************************************
 * *  Name: stat dir 
 * *  Description: Used in the directory checking part in the begginning of the main
 * *  *************************************************************************/


struct stat dir = {0};

void setnames();
void addrooms();
void setconnections();
void addconn(int i, int r);
void addcontents();
void game();
void display(int i);
int getindex(char x[]);


/****************************************************************************
 * *  Name: Main
 *	Description:	the main is a bit messy. if i had time, or any insentive
 *	 		beyond a desire for cleanness i would stream line it with
 *	 		a few helper functions. starts off by adding random names
 *	 		and connections to each of the seven rooms. this information
 *	 		is kept in the structures until it can be written into the
 *	 		corresponding files. then the relevant directory is either
 * 	 		made or opened and the process of creating room files and filling them
 * 			begins. then the game UI function is called.
 *
 * *  *************************************************************************/


int  main(){	
	addrooms(rooms);
	setconnections(rooms);
       FILE * openfile;
       struct Names listnames;
       if(stat("Henninch.rooms", &dir) == -1)
       {
            mkdir("Henninch.rooms", 0777);
       }
        chdir("Henninch.rooms");
	addcontents(rooms);
	game();
	return 0;
	
};
/****************************************************************************
 Function: addrooms
 Description: fills the global structure array "rooms" with types, null values,
		random names.
 *************************************************************************/

void addrooms(){
        strcpy(rooms[0].kind,"start");
	strcpy(rooms[1].kind,"mid");
	strcpy(rooms[2].kind,"mid");
        strcpy(rooms[3].kind,"mid");
        strcpy(rooms[4].kind,"mid");
        strcpy(rooms[5].kind,"mid");
        strcpy(rooms[6].kind,"end");
	int z;
	int ty;
	for(z=0;z<7;z++){
		rooms[z].numcon=0;
		for(ty=0;ty<7;ty++){
			rooms[z].connections[ty]=9;
		}
	}
	
	setnames(names);
	int count=0;
	srand(time(NULL));
	int exceptions[10]= {13};
	int i;
	for(i=0;i<7;i++){
		int r=rand()%10;
		int j=0;
		do{
			if(r==exceptions[j]){
				r=rand()%10;
				j=-1;
			}
			else{
				j++;
			}
		}while(j<count);
		strcpy(rooms[i].name,names[r].title);
		exceptions[count]=r;
		count++;
	}
}
/****************************************************************************
*  Function: setnames
*   Description: fills the global structure array "names with the ten names
*                   that can be randomly selected
* ***************************************************************************/


void setnames(){
	strcpy(names[0].title,"Warden");
	strcpy(names[1].title,"Hawke");
	strcpy(names[2].title,"Inquisitor");
	strcpy(names[3].title,"Varric");
	strcpy(names[4].title,"Morrigan");
	strcpy(names[5].title,"Sten");
	strcpy(names[6].title,"Blackwall");
	strcpy(names[7].title,"Allistair");
	strcpy(names[8].title,"Leliana");
	strcpy(names[9].title,"Cole");

 }

/****************************************************************************
 * *  Function: setconnections
 * *   Description: this function adds connections between rooms while making sure
 * 			that each room has at least three connections. no more than
 * 			six connections can occur. the function tracks the 
 * 			current room, rooms added to the current room, and 
 * 			number of connections the current room has via the rooms
 * 			struct and a local array.
 * *                   
 * * ***************************************************************************/


void setconnections(){
	int exceptions[7];
	int i;
	int z;
	srand(time(NULL));
	for(i=0;i<7;i++){
		int n;
		exceptions[0]=i;
		for(n=0;n<6;n++){
			exceptions[n+1]=rooms[i].connections[n];
		} 
		z=0;
		for(z=rooms[i].numcon;z<6;z++){
			if(z>=3){
				int t=rand()%2;
				if(t>0){
					z=10;
				}	
			}
			if(z<6){
				int j;
				int r=rand()%7;
				for(j=0;j<6;j++){
					if(r==exceptions[j]){
						r=rand()%7;
						j=-1;
					}
					else{
					}
				}
				addconn(i,r);
				exceptions[rooms[i].numcon]=r;
			}
		}	
	}

}


/****************************************************************************
 * *  Function: addconn
 * *   Description: helper function for the previous function. adds connections
 * 			between two rooms by updating each room's connections and 
 * 			iterating their connection numbers.
 * *                   
 * * ***************************************************************************/


void addconn(int i, int r){
	rooms[i].connections[rooms[i].numcon]=r;
	rooms[r].connections[rooms[r].numcon]=i;
	rooms[i].numcon+=1;
	rooms[r].numcon+=1;
}

/****************************************************************************
 * *  Function: addcontents
 * *   Description: creates room files 1-7 inside the rooms directory.
 * 			fills the files after making them with the information
 * 			stored in the global rooms structure array
 * * ***************************************************************************/



void addcontents(){
	FILE * openfile;
	openfile=fopen("room1","w+");
	fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[0].name);
	int i;
	for(i=0;i<rooms[0].numcon;i++){
		fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[0].connections[i]].name);
	}
	fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[0].kind);
	fclose(openfile);
        openfile=fopen("room2","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[1].name);
        for(i=0;i<rooms[1].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[1].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[1].kind);
        fclose(openfile);
        openfile=fopen("room3","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[2].name);
        
        for(i=0;i<rooms[2].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[2].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[2].kind);
        fclose(openfile);
        openfile=fopen("room4","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[3].name);
        for(i=0;i<rooms[3].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[3].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[3].kind);
        fclose(openfile);
        openfile=fopen("room5","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[4].name);
        for(i=0;i<rooms[4].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[4].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[4].kind);
        fclose(openfile);
        openfile=fopen("room6","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[5].name);
        for(i=0;i<rooms[5].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[5].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[5].kind);
        fclose(openfile);
        openfile=fopen("room7","w+");
        fprintf(openfile,"%s %s: %s \n","ROOM","NAME",rooms[6].name);
        for(i=0;i<rooms[6].numcon;i++){
                fprintf(openfile,"%s %i: %s \n","CONNECTION",i+1,rooms[rooms[6].connections[i]].name);
        }
        fprintf(openfile,"%s %s: %s","ROOM","TYPE",rooms[6].kind);
        fclose(openfile);
}


/****************************************************************************
 * *  Function: game
 * *   Description: this function contains the UI and tracks the win condition.
 * 			prompts the user for inputs while disploying relevant information
 * 			on the user's current location via a loop. 
 * * ***************************************************************************/


void game(){

	int location=0;
	int i;
	strcpy(paths[0].leg,rooms[0].name);
	int valid=0;
	int index=0;
	int iterate=0;
	char place[60];
	while(true){
		valid=0;
		if(location==6){
                        break;
                }
		display(location);
		printf("WHERE TO>");
		if(fgets(place,sizeof(place),stdin)){
			index=getindex(place);
			for(i=0;i<rooms[location].numcon;i++){
				if(index==rooms[location].connections[i]){
					strcpy(paths[iterate].leg,place);
					iterate++;
					valid=1;
				}
			}	
			if(valid==1){	
				location=index;
			}
			else{
				printf("NOT A VALID CONNECTION\n");
			}
		}
		else{
			printf("INVALID\n");
		}
	}
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n",iterate);
	int z;
	for(z=0;z<iterate;z++){
		printf("%s",paths[z].leg);
	}
}



/****************************************************************************
 * *  Function: display
 * *   Description: used by the previous function to display the information
 * 			on a particular room via the coresponding file. prints
 * 			information in the desired format.
 * * ***************************************************************************/


void display(int i){
	FILE * openfile;
	char *line;
	size_t len = 0;
    	ssize_t read;
	switch (i){
		case 0 :
			openfile=fopen("room1","r");
			break;
                case 1 :
			openfile=fopen("room2","r");
                        break;
                case 2 :
			openfile=fopen("room3","r");
                        break;
                case 3 :
			openfile=fopen("room4","r");
                        break;
                case 4 :
			openfile=fopen("room5","r");
                        break;
                case 5 :
			openfile=fopen("room6","r");
                        break;
                case 6 :
			openfile=fopen("room7","r");
                        break;
	}
	char thisline[25];
	int k=0;
	int j;
	for(j=0;j<6;j++){
		for(k=0;k<11;k++){
			cons[j].leg[k]='\0';
		}
	}
	k=0;
	while ((read = getline(&line, &len, openfile)) != -1){
	        strcpy(thisline,line);
		if(thisline[5]=='N'){
			printf("ROOM NAME: ");
                        for(j=11;j<sizeof(thisline);j++){
				if(thisline[j]==' '){
					printf("\n");
					break;
				}
				else{
					printf("%c",thisline[j]);
                    		}
			}
                }
                else if (thisline[0]=='C'){
			for(j=14;j<sizeof(thisline)-1;j++){
				if(thisline[j]==' '){
					break;
				}
				else{
					cons[k].leg[(j-14)]=thisline[j];
				}
			}
			k++;                                
                }
		
	}
	printf("POSSIBLE CONNECTIONS: ");
	printf("%s",cons[0].leg);
	for(j=1;j<k;j++){
		printf(", %s",cons[j].leg);
	}
	printf(".\n");
	fclose(openfile); 
}


/****************************************************************************
 * *  Function: getindex
 * *   Description: used by the game function to convert the user's input, meaning
 * 			the name of the room, into the relevant index number, this
 * 			makes updating the user's new location way easier.
 * * ***************************************************************************/


int getindex(char x[]){
	if(x[0]==rooms[0].name[0]){
			return 0;
	}
	else if(x[0]==rooms[1].name[0]){
                        return 1;
	}
	else if(x[0]== rooms[2].name[0]){
                        return 2;
	}
        else if(x[0]== rooms[3].name[0]){
                        return 3;
	}
        else if(x[0]==rooms[4].name[0]){
                        return 4;
	}
        else if(x[0]== rooms[5].name[0]){
                        return 5;
	}
        else if(x[0]==rooms[6].name[0]){
                        return 6;
	}		
}
