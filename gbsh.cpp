#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <sys/wait.h>
#include <stdlib.h>
#include <vector>
#include <fcntl.h>
using namespace std;

bool ImplementCD(char* UserName,char* CurrentDirectory,string UserInput,size_t size){

		if (UserInput.size() < 3){

			string Home = "/home/";
			Home.append(UserName);
			int check = chdir(Home.c_str()) ;

			if (check <0){
				cout<<"Error: could not find directory."<<endl;
				return false;
			}

			getcwd(CurrentDirectory,size);
		}
		else if (UserInput[3] != '/'){

			string Home = "/home/";
			Home.append(UserName);
			Home.append("/");

			for (int j=3 ; UserInput[j] != '\0' ;j++)
				Home +=  UserInput[j] ;

			int check = chdir(Home.c_str()) ;

			if (check <0){
				cout<<"Error: could not find directory."<<endl;
				return false;
			}

			getcwd(CurrentDirectory,size);
		}
		else{
			char* directory= new char[UserInput.size()];

			for (int i=3,j=0 ; UserInput[i] != '\0' ; i++,j++)
				directory[j] += UserInput[i];

			int check = chdir(directory);

			if (check <0){
				cout<<"Error: could not find directory."<<endl;
				return false;
			}

			getcwd(CurrentDirectory,size);
	}

	return true;
}

void ImplementLS(char* UserName,char* CurrentDirectory,string UserInput,size_t size){

	if (UserInput.size() < 3){

		int check = chdir(CurrentDirectory) ;

		if (check <0){
			cout<<"Error: could not find directory."<<endl;
			return;
		}

		system("ls");
	}
	else if (UserInput[3] != '/'){

		string Home = "/home/";
		Home.append(UserName);
		Home.append("/");

		for (int j=3 ; UserInput[j] != '\0' ;j++)
			Home +=  UserInput[j] ;

		int check = chdir(Home.c_str()) ;

		if (check <0){
			cout<<"Error: could not find directory."<<endl;
			return;
		}

		system("ls");
	}
	else{
		char* directory= new char[UserInput.size()];

		for (int i=3,j=0 ; UserInput[i] != '\0' ; i++,j++)
			directory[j] += UserInput[i];

		int check = chdir(directory);

		if (check <0){
			cout<<"Error: could not find directory."<<endl;
		}

		system("ls");
	}
}

int main(int argc, char *argv[]) {

	char* HostName,*CurrentDirectory,*UserName;
	size_t size = 100;
	string UserInput;
	HostName = new char[size];
	CurrentDirectory = new char[size];
	UserName = new char[size];

	gethostname(HostName,size);
	getcwd(CurrentDirectory,size);
	getlogin_r(UserName,size);

	string Shellpath = CurrentDirectory;
	Shellpath += "/gbsh";

	int Flag = setenv("SHELL",Shellpath.c_str(),1);

	if (Flag == 0){
		cout<<"Shell envar CurrentDirectory set Succesfully"<<endl;
	}
	else
		cout<<"Shell envar CurrentDirectory set Failed"<<endl;


	while( 1 ){

			cout<<UserName<<"@"<<HostName<<" "<<CurrentDirectory<<" ";
			getline(cin,UserInput);

			bool RedirectionOut = false,RedirectionIn = false;
			bool Pipe = 0;

			for (int i=0 ; i<UserInput.size() ; i++){
				if (UserInput[i] == '>')
					RedirectionOut = true;
				if (UserInput[i] == '<')
					RedirectionIn = true;

			}

			vector<string> SplitCommand;
			bool backgroundExec = false;
			string command;

			for (int i=0; true ; i++){

				if (UserInput[i] == ' '){
					SplitCommand.push_back(command);
					command = "";
					i++;
				}
				if (UserInput[i] == '\0'){
					SplitCommand.push_back(command);
					break;
				}

				if (UserInput[i] == '|'){
						pipe = true;
						SplitCommand.push_back(command);
						command = "";
						i++;
				}

				if (UserInput[i] == '&'){
					backgroundExec = true;
					SplitCommand.push_back(command);
					break;
				}

				command += UserInput[i];

			}

			cout<<SplitCommand[0]<<endl;

			if (RedirectionOut == true and RedirectionIn == true){

					cout<<"Inside RedirectionIn and Out "<<endl;

					int pid = fork();

					if ( pid == 0){
						int input = -1,output = -1;
						int inputIndex = -1,outputIndex = -1;
						for (int i=0 ; i<SplitCommand.size() ; i++){
							if (SplitCommand[i] == "<")
								inputIndex = i;
							if (SplitCommand[i] == ">")
								outputIndex = i;
						}

						input = open (SplitCommand[inputIndex + 1].c_str(), O_RDONLY);
						output = open (SplitCommand[outputIndex + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

						dup2(input,0);
						dup2(output,1);

						close(input);
						close(output);

						////////// Dynamic argv array /////////////////////
						char** argv1= new char*[SplitCommand.size()];
						int index =0;
						argv1[index] = (char*)SplitCommand[0].c_str();
						index++;
						for (int i = 1 ; i<inputIndex ; i++,index++){
							argv1[index] = (char*)SplitCommand[i].c_str();
						}
						argv1[index] = NULL;
						////////////////////////////////////////////////////
						execvp(SplitCommand[0].c_str(),argv1);

						cout<<"REDIRECTION EXEC FAILED"<<endl;
				}
				else{
					wait(NULL);
				}

			}
			else if (RedirectionIn == true){

					cout<<"Inside RedirectionIn"<<endl;

					int pid = fork();

					if ( pid == 0){
						int input = -1;
						int inputIndex = -1;
						for (int i=0 ; i<SplitCommand.size() ; i++){
							if (SplitCommand[i] == "<")
								inputIndex = i;
						}

						input = open (SplitCommand[inputIndex + 1].c_str(), O_RDONLY);

						dup2(input,0);

						close(input);

						////////// Dynamic argv array /////////////////////
						char** argv1= new char*[SplitCommand.size()];
						int index =0;
						argv1[index] = (char*)SplitCommand[0].c_str();
						index++;
						for (int i = 1 ; i<inputIndex ; i++,index++){
							argv1[index] = (char*)SplitCommand[i].c_str();
						}
						argv1[index] = NULL;
						////////////////////////////////////////////////////
						execvp(SplitCommand[0].c_str(),argv1);

						cout<<"REDIRECTION EXEC FAILED"<<endl;
				}
				else{
					wait(NULL);
				}

			}
			else if (RedirectionOut == true){

				cout<<"Inside RedirectionOut "<<endl;

				int pid = fork();

			if (pid == 0){
				int output = -1;
				int index1=0;

					for (int i=0 ; i<SplitCommand.size() ; i++){
						if (SplitCommand[i] == ">")
							index1 = i;
					}

					cout<<"Index: "<<index1<<endl;
					cout<<"directory: "<<SplitCommand[index1 + 1]<<endl;

					output = open(SplitCommand[index1 + 1].c_str() ,O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

					dup2(output,1);

					close (output);

					////////// Dynamic argv array /////////////////////
					char** argv1= new char*[SplitCommand.size()];
					int index =0;
					argv1[index] = (char*)SplitCommand[0].c_str();
					index++;
					for (int i = 1 ; i<index1 ; i++,index++){
						argv1[index] = (char*)SplitCommand[i].c_str();
					}
					argv1[index] = NULL;
					////////////////////////////////////////////////////
					execvp(SplitCommand[0].c_str(),argv1);

					cout<<"REDIRECTION EXEC FAILED"<<endl;
				}
				else{
					wait(NULL);
				}
			}
			else if (SplitCommand[0] == "exit"){
				exit(0);
			}
			else if (SplitCommand[0] == "pwd"){
				char* CurrentPwdDirectory;
				size_t size = 100;
				CurrentPwdDirectory = new char[size];

				getcwd(CurrentPwdDirectory,size);
				cout<<CurrentPwdDirectory<<endl;

			}
			else if (SplitCommand[0] == "clear"){
				cout<<"\033c";
			}
			else if (SplitCommand[0] == "cd"){

				ImplementCD(UserName,CurrentDirectory,UserInput,size);
			}
			else if (SplitCommand[0] == "ls"){

				int pid = fork();
				int status = 0;
				if (pid == 0){

					ImplementLS(UserName,CurrentDirectory,UserInput,size);
					exit(status);
				}
				else{
					wait(&status);
				}
			}
			else if (SplitCommand[0] == "environ"){

					extern char** environ;

					while(*environ){
						cout<<*environ<<endl<<endl;
						environ++;
					}

			}
			else if (SplitCommand[0] == "setenv"){

				int chec=-1;

				if (SplitCommand[2] != "")
					chec = setenv(SplitCommand[1].c_str(),SplitCommand[2].c_str(),false);
				else
					chec = setenv(SplitCommand[1].c_str(),"",false);

				if (chec == 0 ){
					cout<<"Succesfulluy changed."<<endl;
				}
				else
					cout<<"Failed to change"<<endl;

			}
			else if (SplitCommand[0] == "unsetenv"){
					int check = unsetenv(SplitCommand[1].c_str());

					if (check == 0){
						cout<<"Succesfulluy changed."<<endl;
					}
					else
						cout<<"Failed to change"<<endl;

				}
			else if (backgroundExec){

					cout<<"entered background execution: "<<endl;

					int pid = fork();

					if ( pid == 0){

						setpgid( 0 , 0);
						char** argv= new char*[SplitCommand.size()+ 1];
						int index =0;
						argv[index] = (char*)SplitCommand[0].c_str();
						index++;
						for (int i = 1 ; i<SplitCommand.size() ; i++,index++){
							argv[index] = (char*)SplitCommand[i].c_str();
						}
						argv[index] = NULL;

						execvp( SplitCommand[0].c_str() , argv );
					}
					else{}

			}
			else if (pipe){

			}
			else{

					///////////// Making dynamic argv array //////////////////
					char** argv= new char*[SplitCommand.size()];
					int index =0;
					argv[index] = (char*)SplitCommand[0].c_str();
					index++;
					for (int i = 1 ; i<SplitCommand.size() ; i++,index++){
						argv[index] = (char*)SplitCommand[i].c_str();
					}
					argv[index] = NULL;

					////////////////////////////////////////////////////////////
					int pid = fork();

					if (pid == 0 ){
						int check = setenv("parent",CurrentDirectory,false);

						char* cparent  = getenv("parent");
						string cpar = cparent;
						cout<<"Child process parent: "<<cpar<<endl<<endl;
						execvp( SplitCommand[0].c_str() , argv );

						cout<<"EXEC failed"<<endl;
					}
					else{
						wait(NULL);
					}
				}
		}

	// shell code here

	// ...

	// ...

	exit(0); // exit normally
}
