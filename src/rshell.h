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
#include <sys/wait.h>
#include <linux/limits.h>
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

	//obtain user input commands
	getline(cin, rawinput);

	if (!cin.good())
	{
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
				}
			}
			if (parsedinput.at(i) == '>')
			{
				if (parsedinput.at(i+1) == '>') 
				{
					cmds.push_back(">>");
					i+=2;
					if (i > parsedinput.size()) return true;
					continue;
				}
				else 
				{
					cmds.push_back(">");
					i++;
					continue;
				}
			}
			if (parsedinput.at(i) == '|')
			{
				cmds.push_back("|");
				i++;
				continue;
			}
			if (parsedinput.at(i) == '<')
			{
				cmds.push_back("<");
				i++;
				continue;
			}
			else if (parsedinput.at(i) != ' ')
			{
				parsecmd += parsedinput.at(i);
				if (!isalpha(parsedinput.at(i+1)) && parsedinput.at(i+1) != '.')
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
	if (cmds.at(0) == ";") return true;

	else
	{
		cmds.push_back(string(";"));
		return false;
	}
}

bool execcmds(const vector<string> &cmds)
{
	vector<string> newvec;
	for (unsigned i = 0; i < cmds.size(); i++)
	{
		if (cmds.at(i) == "<" || cmds.at(i) == ">" || cmds.at(i) == ">>" || cmds.at(i) == "|")
		{
			//if && and || are found after a redirection symbol -- syntax error
			if (cmds.at(i+1) == "&&" || cmds.at(i+1) == "||")  return true;
			//if characters are first thing input -- syntax error
			if (i == 0) return true;


		}


		if (cmds.at(i) == "&&" || cmds.at(i) == "||" || cmds.at(i) == ";")
		{

			//if "&& ; " or "|| ;" was entered -- syntax error
			if ((cmds.at(i) == "&&" || cmds.at(i) == "||") && cmds.at(i+1) == ";") return true;

			//else, proceed to convert to char** and call execvp
			char** newargv = new char*[newvec.size()+1];
			for (unsigned j = 0; j < newvec.size(); j++)
			{
				newargv[j] = new char[newvec.at(j).size()+1]; 
				strcpy(newargv[j], newvec.at(j).c_str());
			}
			newargv[newvec.size()] = '\0';

			int execret = 0; //keeps track of exec return value. init to 0
			int pid = fork();
			if (pid == -1) perror("fork");
			//if we're in the child
			if (pid == 0)
			{	
				execret = execvp(newargv[0], newargv);
				if (-1 == execret)
				{	
					perror(*newargv);
					for (unsigned k=0; k < newvec.size(); k++)
					{
						delete[] newargv[i];
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
			}
			if (newvec.size() > 0)
			{
				newvec.clear();
				for (unsigned k=0; k < newvec.size(); k++)
				{
						delete[] newargv[i];
				}
				delete[] newargv;
			}
			if (cmds.at(i) == "&&" && cmds.at(i-1) == "false") return false;
			if (cmds.at(i) == "||" && execret == 0 && cmds.at(i-1) != "false") return false; 
			if (cmds.at(i) == ";" && i == cmds.size()-1) return false;
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
