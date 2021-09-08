#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <sys/wait.h>
#include <fstream>
#include <fcntl.h>
using namespace std;
void ignoreSIGINT(int ignoresig);
void prompt();
void runcommand(char * input);
int main(int argc, char *argv[]) 
{
	signal(SIGINT,ignoreSIGINT);
	//fprintf(stdout, "Hello World!\n");
	char shellreplace[1000]; 
	getcwd(shellreplace,sizeof(shellreplace));
	setenv("SHELL",shellreplace,1);
	// shell code here.
	char input[1000] = "\0";
	//system("clear");//Clear screen.
	printf("\033c");//Clear screen.
	while(1) 
	{
		prompt();
		fgets(input,1000,stdin);
		runcommand(input);
	}

	exit(0); // exit normally	
}

void prompt()
{
	//---------------P R O M P T C O D E---------------//
	char buffer[1000];
	char * myuser;
	char host[1000];
	char directory[1000];
	getcwd(directory,sizeof(directory));
	gethostname(host,sizeof(host));
	myuser = getenv("USER");
	printf("\033[1;31m");
	printf("%s@%s",myuser,host);
	printf("\033[1;32m");
	printf("%s > ",directory);
	//fprintf(stdout,buffer);
}
void runcommand(char * input)
{
	int inFD = 0;
	int outFD = 0;
	//fstream IFile;
	//fstream OFile;
	bool noWait = false;
	bool Iredir = false;
	bool Oredir = false;
	bool runExec = false;
	string inputFile;
	string outputFile;
	int argcount = 0;
	string Iarray[10];
	string tempinput;
	stringstream breakinput(input);
	while(1)
	{
		if (breakinput >> tempinput)
		{
			Iarray[argcount] = tempinput;
			argcount++;
		}
		else
		{
			break;
		}
	}
	if(argcount > 2)//I/O Redirection.
	{
		for (int i = 0; i < argcount; i++)
		{
			if (Iarray[i] == "<")
			{
				inputFile = Iarray[i+1];
				Iredir = true;
				runExec = true;
			}
			if (Iarray[i] == ">")
			{
				outputFile = Iarray[i+1];
				Oredir = true;
				runExec = true;
			}
			if (Iarray[i] == "&")
			{
				noWait = true;
			}
		}
	}
	if (Iarray[0] == "exit")
	{
		exit(0);
	}
	else if (Iarray[0] == "pwd" && runExec == false)
	{
		char directory[1000];
		getcwd(directory,sizeof(directory));
		printf("%s",directory);
		printf("\n");
	}
	else if (Iarray[0] == "clear")
	{
		//system("clear");
		printf("\033c");
	}
	else if (Iarray[0] == "ls" && runExec == false)
	{
		struct dirent **listdir;
		int numdir;
		if (argcount == 1)//Only LS.
		{
			//cout << "Only LS entered !" << endl;
			numdir = scandir(".",&listdir,NULL,alphasort);
			for (int i = numdir-1; i > 0; i--)
			{
				if(listdir[i]->d_name[0] == '.')//Extra dots at end.
				{
					free(listdir[i]);
					break;
				}
				printf("%s \n",listdir[i]->d_name);
				free(listdir[i]);
			}
			free(listdir);
		}
		else if (argcount == 2 && Iarray[1][0] != '-')
		{
			//cout << "LS has something with it !" << endl;
			const char * dtory = Iarray[1].c_str();
			numdir = scandir(dtory,&listdir,NULL,alphasort);
			for (int i = numdir-1; i > 0; i--)
			{
				if(listdir[i]->d_name[0] == '.')//Extra dots at end.
				{
					free(listdir[i]);
					break;
				}
				printf("%s \n",listdir[i]->d_name);
				free(listdir[i]);
			}
			free(listdir);
		}
	}
	if (Iarray[0] == "cd")
	{
		if ( argcount == 1 )//Only CD.
		{
			char dtory[1000];
			char * homedtory;
			homedtory = getenv("HOME");
			//getcwd(dtory,sizeof(dtory));
			chdir(homedtory);
		}
		else if (argcount == 2)
		{
			const char * dtory = Iarray[1].c_str();
			chdir(dtory);
		}
	}
	else if (Iarray[0] == "environ" && runExec == false)//Display all environment variables.
	{
		char ** env = environ;
		int i = 0;
		while (env[i] != NULL)
		{
			printf("%s \n",env[i]);
			i++;
		}
	}
	else if (Iarray[0] == "setenv")
	{
		if (argcount == 2)//Only setenv with envar variable.
		{
			const char * eVar = Iarray[1].c_str();
			if (getenv(eVar) != NULL)
			{
				printf("%s \n","Variable was already undefined !");
			}
			setenv(eVar,"\0",0);
		}
		else if (argcount == 3)//Setenv with envar variable and value.
		{
			const char * eVar = Iarray[1].c_str();
			const char * value = Iarray[2].c_str();
			if (getenv(eVar) != NULL)
			{
				printf("%s \n","Variable was already undefined !");
			}
			setenv(eVar,value,0);
		}
	}
	else if (Iarray[0] == "unsetenv")
	{
		if (argcount == 2)
		{
			const char * eVar = Iarray[1].c_str();
			char * check = getenv(eVar);
			if (check == NULL)
			{
				printf("%s \n","Variable was already undefined !");
			}
			else
			{
				unsetenv(eVar);
			}
			//unsetenv(eVar);
		}
	}
	else
	{
		//printf("%s \n","Command not found");
		pid_t newproc;
		newproc = fork();
		if (newproc > 0)//Parent.
		{
			if (noWait == false)
			{
				wait(NULL);
			}
			//dup2(1,outFD);
			//dup2(0,inFD);
			/*int check1 = close(inFD);
			if (check1 == 0)
			{
				cout << "SUCCESS input!"  << endl;
			}
			else
			{
				cout << "FAILED input! " << endl;
			}
			int check2 = close(outFD);
			if (check2 == 0)
			{
				cout << "SUCCESS output!" << endl;
			}
			else
			{
				cout << "FAILED output!" << endl;
			}*/
			
		}
		else if (newproc == 0)//Child.
		{
			if (Iredir == true)
			{
				//cout << inputFile << endl;
				inFD = open(inputFile.c_str(),O_RDONLY);
				if (inFD == -1)
				{
					printf("%s ","ERROR WHILE OPENING INPUT FILE!");
					exit(0);
				}
				dup2(inFD,0);
				
			}
			if (Oredir == true)
			{
				//cout << outputFile << endl;
				outFD = open(outputFile.c_str(),O_WRONLY | O_TRUNC);
				if (outFD == -1)
				{
					//printf("%s ","ERROR WHILE OPENING OUTPUT FILE!");
					//exit(0);
					outFD = open(outputFile.c_str(),O_WRONLY | O_CREAT,0777);
				}
				dup2(outFD,1);
			}
			char parentreplace[1000]; 
			getcwd(parentreplace,sizeof(parentreplace));
			setenv("parent",parentreplace,1);
			const char* argexec = Iarray[0].c_str();
			int size = 0;
			for (int i = 0; i < argcount; i++)
			{
				if (Iarray[i] == ">" || Iarray[i] == "<" || Iarray[i] == "&")
				{
					break;
				}
				else
				{
					size++;
				}
			}
			//cout << "SIZE IS " << size << endl;
			if (argcount > 1)
			{
				size = size + 1;
				char * arguments[size];
				int iter = 0;
				for (int i = 0; i < argcount; i++)
				{
					if (Iarray[i] == ">" || Iarray[i] == "<" || Iarray[i] == "&")
					{
						break;
					}
					else
					{
						arguments[iter] = new char [strlen(Iarray[i].c_str())];
						//cout << "Copying : " << Iarray[i] << endl;
						strcpy(arguments[iter],Iarray[i].c_str());
						//cout << arguments[iter] << endl;
						iter = iter +1 ;
					}
				}
				arguments[size-1] = NULL;
				int checkrun = execvp(argexec,arguments);
				if (checkrun == -1)
				{
					//printf("%s","Didn't execute.");
					//wc -m & < wc-test.txt > wc-test.cnt
				}
				else
				{
					//printf("%s","Did execute.");
				}
			}
			else
			{
				char * arguments[argcount+1];
				arguments[0] = new char [strlen(Iarray[0].c_str())];
				strcpy(arguments[0],Iarray[0].c_str());
				arguments[1] = NULL;
				execvp(argexec,arguments);
			}
			exit(0);
		}
		else
		{
			printf("%s \n","Fork failed !");
		}
	}
	
	return;
}
void ignoreSIGINT(int ignoresig)
{
	signal(SIGINT,ignoreSIGINT);
	fprintf(stdout,"\n");
	prompt();
	fflush(stdout);
}