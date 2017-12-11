#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_BUFFER 1024			//Max Line Buffer
#define MAX_ARGS 64			//Max # of args
#define SEPARATORS " \t\n"		//Token Separators

int check (char ** line);
static char* path;

int main(int argc, char ** argv){
	char buffer[MAX_BUFFER];	//Line Buffer
	char * args[MAX_ARGS];		//Pointers to arg strings
	char ** arg;			//Working pointers to thru args
	size_t size;
	size = (size_t) pathconf(".", _PC_PATH_MAX);
	char * prompt = malloc(size);		//Shell prompt
	extern char **environ;		//Environment Array
	pid_t processId;
	int status;
	static char *directory;
	directory = strdup(prompt);

	if (prompt !=0){
		prompt = getcwd(prompt ,size);
	}
	
	strcat(prompt, "==>");

	/*Keep reading input until "quit" command or eof of redirected input*/
	while(!feof(stdin)){
		/*Get command line from input*/
		fputs(prompt, stdout);		//Write prompt	
		
		if (fgets(buffer, MAX_BUFFER, stdin)){		//Read a line
			char* userInput = strdup(buffer);
			/*Tokenize the input into args array*/
			arg = args;
			*arg++ = strtok(buffer, SEPARATORS);		//Tokenize input

			while ((*arg++ = strtok(NULL, SEPARATORS)));		//Last entry will be NULL

			if (args[0]){		//If there's anything there

				/*Check for internal/external command*/
				if (!strcmp(args[0], "clr")){		//"clear" command
					int hold = system("clear");
					switch (processId = fork())
					{
						case -1: exit(-1); //error
							 break;
						case 0: execlp("clear", "clear", NULL);
							break;
						default: waitpid(processId,&status, WUNTRACED);
					} 
					continue;
				}

				if (!strcmp(args[0], "dir")){		//"directory" command
					char command [MAX_BUFFER];
				       	strcpy(command, "ls -al ");

					if (args[1] == 0){
						//int hold = system(command);
						
						switch (processId = fork())
						{
							case -1: exit(-1); //error
								 break;
							case 0: execlp("ls", "ls","-al", NULL);
								break;
							default: waitpid(processId,&status, WUNTRACED);
						} 
						continue;
					}
					char* requestedDirectory = strdup(args[1]);
					char* requestedDirectoryCall = strcat(command, requestedDirectory);
				//	int hold = system(requestedDirectoryCall);
					switch (processId = fork())
					{
						case -1: exit(-1); //error
							 break;
						case 0: execlp("ls", "ls","-al",args[1],  NULL);
							break;
						default: waitpid(processId,&status, WUNTRACED);
					} 
					free(requestedDirectory);
					continue;
				}

				if (!strcmp(args[0], "environ")){		//"environment" command
					char ** envi = environ;
					while(*envi){
						printf("%s\n", *envi++);
					}
					continue;
				}
				
				if (!strcmp(args[0], "quit")){		//"quit" command
					exit(0);
				}

				if(!strcmp(args[0], "cd")){		//"cd" command
					if (args[1] == 0){		//No directory, display the current
						if (directory !=0){		//Check for empty directory
							printf("%s\n",directory);
							continue;
						}

						else{		//error check
							printf("%s", "An error occured while obtaining directory:( \n");
							continue;
						}
					}
					
					//Changes the directory
					else{
						if (chdir(args[1]) != -1){
							directory = malloc(size);
							directory = strdup(getcwd(directory, size));	//update current directory
							path = strdup("PWD = ");
							path = strdup(strcat(path, directory));		//sets new working directory
							putenv(path);
							prompt = strdup(strcat(directory, "==>"));
							continue;
						}
						
						//Error check
						else{
							printf("%s\n", "Directory not found");
							continue;
						}
					}
				}

				//runs for redirection of stdin stdout
				if (check(args)){
					char ** pointer;		//Pointer to be used throughout
					char * values [MAX_ARGS];
					int count = 0;			//counter to be used thoughout

					switch(processId = fork()){	//Fork for exec call
						case -1: exit(-1);
							 break;

						case 0:
							 pointer = args;
							 //Loop to get command, args, and redirection of files
							 while (*pointer){
								 if (strcmp(*pointer, "<") ==0){	//for input redirection
									 *pointer++;
									 if (*pointer == NULL){		//check file existance error
										 printf("%s\n", "No file entered. Command failed");
										 exit(0);
									 }

									 char * file = strdup(*pointer);	//copy of string for filename
									 char ** temp_pointer = pointer;	//Uses of temp to prevent incrementing the pointer used throughout
									 *temp_pointer++;
										
									 //error check
									 if (*temp_pointer != NULL && strcmp(*temp_pointer, ">") !=0 && strcmp(*temp_pointer, ">>") !=0){
										 printf("%s", "Error, Expected no arg, or > / >> after the file.");

										 //free file to prevent leaks
										 if (file !=0){
											 free (file);
										 }
										 exit(0);
									 }

									 //stdin to file and error check
									 if (freopen(file, "r", stdin) == NULL){
										 printf("%s", "There was an error opening the file");
										 if (file != 0){
											 free(file);
										 }
										 exit(0);
									 }
									 free (file);
									 continue;
								 }

								 //stdout redirection error check
								 if (strcmp(*pointer, ">") == 0){
									 *pointer++;
									 if (*pointer == NULL){
										 printf("%s\n", "No file entered.");
										 exit(0);
									 }

									 char *file = strdup(*pointer);
									 *pointer++;

									 if (*pointer !=0){
										 if (file != 0){
											 free(file);
										 }
										 printf("Expected no arguments after the file.");
										 exit(0);
									 }
									 
									 freopen(file, "w", stdout);
									 free (file);
									 break;
								 }
								 
								 if (strcmp(*pointer, ">>") == 0){
									 *pointer++;
									 if (*pointer == NULL){
										 printf("%s\n", "No file entered.");
										 exit(0);
									 }

									 char *file = strdup(*pointer);
									 *pointer++;

									 if (*pointer !=0){
										 if (file != 0){
											 free(file);
										 }
										 printf("Expected no arguments after the file.");
										 exit(0);
									 }
									 
									 freopen(file, "a", stdout);
									 free (file);
									 break;
								 }

								 values [count] = strdup(*pointer);
								 *pointer++;		//increment pointer
								 ++count;		//increment count
							 }
							 values[count] = NULL;		//values must end with Null 

							 //error check for valid command
							 if(execvp(values[0], values) == -1){
								 printf("%s", "Not a valid command. Try again \n");
								 exit(0);
							 }
							 break;

						default: waitpid(processId, &status, WUNTRACED);
					}
					continue;
				}



			//`	int hold = system(userInput);
				free(userInput);
				//Pass to OS
				switch (processId = fork())
				{
					case -1: exit(-1); //error
						 break;
					case 0: 
						if ( execvp(args[0], args) == -1)
						{
							printf("%s", "Not a valid command \n");
							exit(0);
						}
						break;
					default: waitpid(processId,&status, WUNTRACED);
				} 
			}
		}
	}
	return 0;
}

//checks for input output operators
int check(char** pointer){
	char ** tracker = pointer;

	while (*tracker){
		if (strcmp(*tracker, "<") == 0 || strcmp(*tracker, ">") == 0 || strcmp(*tracker, ">>") == 0){
			return 1;
		}
		*tracker++;
	}
	return 0;
}
