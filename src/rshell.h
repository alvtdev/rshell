#ifndef RSHELL_H
#define RSHELL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <limits.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

void printprompt()
{
	//obtain login and hostname
	char* login = getlogin();
	if (login == NULL) perror("login");

	char hostname[256];
	if (-1 == gethostname(hostname, 256)) perror("hostname");

	printf("%s", login);
	printf("%s", "@");
	printf("%s ", hostname);

	//simple command prompt print
	printf("%s", "$ ");
}

string getinput()
{
	//string will store raw user input
	string rawinput;
	
	if(cin.fail())
	{
	//freopen("/dev/stdin", "w", stdin);
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	}

	//obtain user input commands
	getline(cin, rawinput);


	if (cin.fail())
	{
		perror("cin is broken");
		printf("\n");
		exit(1);
	}

	//exit shell condition
	if (rawinput.substr(0,4) == "exit") 
	{
		//killrshell = true;
		printf("Goodbye.\n");
		exit(0);
	}

	//parse through raw input and remove comments if entered
	//removes everything after '#'
	if (rawinput.find('#') != string::npos) rawinput = rawinput.substr(0, rawinput.find('#'));

	return rawinput;
}

string parseinput(string rawinput)
{
	//variables to keep track of whitespaces
	int wspos = 0;
	string parsedinput = rawinput;


	//first remove leading whitespaces at the very beginning of the string
	while (parsedinput.at(wspos) == ' ') wspos++;
	parsedinput = parsedinput.substr(wspos, parsedinput.size());
	//then reinit wsend for removing inner whitespaces
	wspos = 0;

	//next remove trailing whitespaces at the end of the string
	parsedinput += ' '; //add space at the end to make removal easier.
	wspos = parsedinput.size()-1;
	if(parsedinput.at(wspos) == ' ')
	{
		while (parsedinput.at(wspos) == ' ')
		{
			--wspos;
		}
	
		parsedinput = parsedinput.substr(0, wspos+1);
	}

	//add semicolon to the end of the string to make parsing easier -- only if there isn't already a semicolon there in the first place
	if (parsedinput.at(parsedinput.size()-1) != ';') parsedinput += ';';

	return parsedinput;
}

bool makecmds(const string parsedinput, vector<string> &cmds)
{
	//initial scan for syntax errors
	if (!isalpha(parsedinput.at(0))) return true;

	bool seploopdone = false;
	//iterate through string and separate into commands and connectors
	while (seploopdone == false)
	{
		string parsecmd;
		
		if (parsedinput.at(0) == ';') 
		{
			return true;
		}

		for (unsigned i=0; i<parsedinput.size(); i++)
		{
			if (i >= parsedinput.size()-1) break; //accomodates for concatenated ; at the end of the string
			//if 2+ ;; -- syntax error
			if (parsedinput.at(i) == ';' && parsedinput.at(i+1) == parsedinput.at(i))
			{
				return true;
			}
			//checks for & and |
			if ( (parsedinput.at(i) == '&' || parsedinput.at(i) == '|') && parsedinput.at(i+1) == parsedinput.at(i) )
			{
				//if 3 &&& ||| -- syntax error
				if (parsedinput.at(i+2) == parsedinput.at(i))
				{
					return true;
				}
				
				else
				{
					string con1;
					con1 += parsedinput.at(i);
					string con2;
					con2 += parsedinput.at(i+1);
					string constring = con1 + con2;
					cmds.push_back(constring);
					parsecmd.clear();
					i++;
					continue;
				}
			}
			else if (parsedinput.at(i) == '>')
			{
				if (parsedinput.at(i+1) == '>') 
				{
					cmds.push_back(">>");
					i++;
					if (i > parsedinput.size()) return true;
					continue;
				}
				else 
				{
					cmds.push_back(">");
					continue;
				}
			}
			else if (parsedinput.at(i) == '|')
			{
				cmds.push_back("|");
				continue;
			}
			else if (parsedinput.at(i) == '<')
			{
				cmds.push_back("<");
				continue;
			}
			else if (parsedinput.at(i) != ' ')
			{
				parsecmd += parsedinput.at(i);
				if ((!isalpha(parsedinput.at(i+1)) && !isdigit(parsedinput.at(i+1))) && parsedinput.at(i+1) != '.')
				{
					cmds.push_back(parsecmd);
					parsecmd.clear();
				}
			}
		}
		seploopdone = true;
	}

	//preliminary syntax error checks before exec command call
	if (cmds.at(cmds.size()-1) == "&&" || cmds.at(cmds.size()-1) == "||") return true;
	if (cmds.at(cmds.size()-1) == ">" || cmds.at(cmds.size()-1) == ">>") return true;
	if (cmds.at(cmds.size()-1) == "<" || cmds.at(cmds.size()-1) == "|") return true;
	if (cmds.at(0) == ";") return true;

	else
	{
		cmds.push_back(string(";"));
		return false;
	}
}

int pipecount(const string parsedinput)
{
	int temp = 0;
	for (unsigned i=0; i < parsedinput.size(); i++)
	{
		if (parsedinput.at(i)=='|') temp++;
	}
	return temp;
}

bool execcmds(const vector<string> &cmds, int pcount)
{
	int pcntbckup = pcount;
	//input and output file handles, set to -1 so it won't interfere with other file descriptors
	int in = -1;
	int out = -1;
	//declare pipefd in case pipes exit
	int pipefd[2];
	//create pipe if pipes found
	if (pcount >  0)
	{
		if (pipe(pipefd) == -1)
		{
			perror("pipe");
			exit(1); 
		}
	}
	//backup stdin and stdout 
	
	int inbackup = dup(0);
	if (-1 == inbackup)
	{
		perror("backup stdin failed");
		exit(1);
	}
	int outbackup = dup(1);
	if (-1 == outbackup)
	{
		perror("backup stdout failed");
		exit(1);
	}


	vector<string> newvec;
	for (unsigned i = 0; i < cmds.size(); i++)
	{
		//temp is the position of the argument after a redireection operator is found
		unsigned temp = i;

		//check for syntax errors with redirection symbols
		if (cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>"/* || cmds.at(i) == "|"*/)
		{
			//if && and || are found after a redirection symbol -- syntax error
			if (cmds.at(i+1) == "&&" || cmds.at(i+1) == "||")  return true;
			//if characters are first thing input -- syntax error
			if (i == 0) return true;
			//temp int keeping track of where i should be after loop
			temp = i+1;

			//check for output redirection
			//if complementary output redirection, temp+=2
			if (cmds.at(temp+1) == ">" || cmds.at(temp+1) == ">>") temp+=2;
		}


		if (cmds.at(i) == "&&" || cmds.at(i) == "||" || cmds.at(i) == ";" || 
			cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "|")
		{

			//if "&& ; " or "|| ;" was entered -- syntax error
			if ((cmds.at(i) == "&&" || cmds.at(i) == "||") && cmds.at(i+1) == ";") return true;

			if (cmds.at(i) == "<" && cmds.at(i+2) == "|") continue;

			//else, proceed to convert to char** for execvp
			char** newargv = new char*[newvec.size()+1];
			for (unsigned j = 0; j < newvec.size(); j++)
			{
				newargv[j] = new char[newvec.at(j).size()+1]; 
				strcpy(newargv[j], newvec.at(j).c_str());
				//cout << newvec.at(j) << " added." << endl;
			}
			
			newargv[newvec.size()] = '\0';


		  //cout << "execvp called on " << newargv[0] << endl;

			int execret = 0; //keeps track of exec return value. init to 0
			int pid = fork();
			if (pid == -1) perror("fork");
			//if we're in the child
			if (pid == 0)
			{	
				//handle piping
				if (pcntbckup - pcount == 0 && cmds.at(i) == "|")
				{
					if(-1 == close(pipefd[0]))
					{
						perror("child close failed");
						exit(1);
					}
					if(-1 == dup2(pipefd[1], 1))
					{
						perror("child dup failed");
						exit(1);
					}
				} 

				//handle input redirection
				else if (cmds.at(i) == "<")
				{
					++i;
					in = open(cmds.at(i).c_str(), O_RDONLY);
					if (in == -1)
					{
						perror("open in failed");
						exit(1);
					}
					if (-1 == dup2(in, 0))
					{
						perror("dup failed");
						exit(1);
					}

					++i;
					//handle piping
					if (cmds.at(i) == "|")
					{
						/*
						if(-1 == dup2(in, pipefd[1]))
						{
							perror("child dup 1 failed");
							exit(1);
						}
						*/
						if(-1 == dup2(pipefd[1], 1))
						{
							perror("child dup failed");
							exit(1);
						}
					} 
				} 


				//handle output redirection
				if (cmds.at(i) == ">")
				{
					++i;
					out = open(cmds.at(i).c_str(), O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
					if (out == -1)
					{
						perror ("open out failed");
						exit(1);
					}
					if (-1 == dup2(out, 1))
					{
						perror("dup failed");
						exit(1);
					}
				}
				if (cmds.at(i) == ">>")
				{
					++i;
					out = open(cmds.at(i).c_str(), O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
					if (out == -1)
					{
						perror ("ERROR");
					}
					if ( out != -1 && -1 == dup2(out, 1))
					{
						perror("dup failed");
						exit(1);
					}
				}


				//execute
				execret = execvp(newargv[0], newargv);

				//if an error happens, call perror, delete data, and exit
				if (-1 == execret)
				{	
					perror(*newargv);
					for (unsigned k=0; k <= newvec.size(); k++)
					{
						delete[] newargv[k];
					}
					delete[] newargv;
					exit(1);
				}

			}
			else
			{
				if (-1 == wait(0)) 
				{
					perror("wait");
					exit(1);
				}
				pcntbckup--;
				//if not the last pipe, only close write end
				if (pcntbckup >= 0)
				{
					if (-1 == close(pipefd[1]))
					{
						perror("parent close pipe 1 failed");
						exit(1);
					}
					if (-1 == close(0))
					{
						perror ("close 0 failed");
						exit(1);
					}
					if(-1 == dup(pipefd[0]))
					{
						perror("parent dup failed");
						exit(1);
					}
				} 
			}
			//close in
			if (in != -1)
			{
				if (-1 == close(in))
				{
					perror("close in failed"); 
					exit(1);
				}
				in = -1;
			}
			//close out
			if (out != -1)
			{
				if (-1 == close(out))
				{
					perror("close out failed");
					exit(1);
				}
				out = -1;
			}
				//restore stdin and stdout if pipe commands done
				if (pcntbckup < 0 && pcount > 0)
				{
					if (-1 == close(pipefd[0]))
					{
						perror("parent close pipe 0 failed");
						exit(1);
					}
					/*
					if(-1 == close(pipefd[1]))
					{
						perror("parent close pipe 1 failed");
						exit(1);
					}
					*/
					if (-1 == dup2(inbackup, 0))
					{
						perror("restore in failed");
						exit(1);
					}
					
					if (-1 == dup2(outbackup, 1))
					{
						perror("restore out failed");
						exit(1);
					}

					if (-1 == close(inbackup))
					{
						perror("close inbackup failed");
						exit(1);
					}
					if (-1 == close(outbackup))
					{
						perror("close outbackup failed");
						exit(1);
					}
				}
			//clear all memory
			if (newvec.size() > 0)
			{
				newvec.clear();
				for (unsigned k=0; k <= newvec.size(); k++)
				{
						delete[] newargv[k];
				}
				delete[] newargv;
			}
			if (cmds.at(i) == "&&" && cmds.at(i-1) == "false") return false;
			if (cmds.at(i) == "||" && execret == 0 && cmds.at(i-1) != "false") return false; 
			if (cmds.at(i) == ";" && i == cmds.size()-1) return false;
			if (temp > i) i = temp;
//			cout << "new i = " << i << ", " << cmds.at(i) << endl;
			if (i==cmds.size()-2) return false;
		}
		else if (cmds.at(i) == "exit" && (cmds.at(i-1) == ";" || cmds.at(i-1) == "&&"))
		{
			//killrshell = true;
			printf("Goodbye\n");
			exit(0);
		}
		else 
		{
			newvec.push_back(cmds.at(i));
		}
	}
	return false;
}

#endif
