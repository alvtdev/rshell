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
using namespace std;


/*TODO: refine the commandexec algorithm before proceeding further.
This current algorithm was an initial attempt.
It does not work, and will be replaced. */

int main (int argc, char** argv)
{

	//bool value use to determine when to kill rshell loop
	bool killrshell = false; 

	while (!killrshell && cin.good())
	{

		//bool will store syntax error
		bool synerror = false;
		//string will store raw user input
		string rawinput;
		//vector of strings, will convert to array for execvp
		vector<string> cmds;
	
		//simple command prompt print
		printf("%s", "$ ");

		//obtain user input commands
		getline(cin, rawinput);

		//temporary break loop condition: user inputs 'exit' 
		if (rawinput.substr(0,4) == "exit") 
		{
			killrshell = true;
			break;
		}

		//parse through raw input and remove comments if entered
		//removes everything after '#'
		if (rawinput.find('#') != string::npos) rawinput = rawinput.substr(0, rawinput.find('#'));
		

		//execute the following code ONLY IF STRING IS NOT EMPTY
		if (!rawinput.empty())
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

			//add semicolon to the end of the string to make parsing easier
			parsedinput += ';';
			
			//initial scan for syntax errors
			if (!isalpha(parsedinput.at(0))) synerror = true;

			//bool variable to tell when the inner for loop is done
			bool seploopdone = false;

			//iterate through string and separate into commands and connectors
			while (synerror != true && seploopdone == false)
			{
				string parsecmd;
				for (unsigned i=0; i<parsedinput.size(); i++)
				{
					if (i >= parsedinput.size()-1) break; //accomodates for concatenated ; at the end of the string
					//checks for & and |
					if ( (parsedinput.at(i) == '&' || parsedinput.at(i) == '|') && parsedinput.at(i+1) == parsedinput.at(i) )
					{
						//if 3 &&& ||| -- syntax error
						if (parsedinput.at(i+2) == parsedinput.at(i))
						{
							synerror =true;
							break;
						}
						//if 2+ ;; -- syntax error
						if (parsedinput.at(i) == ';' && (parsedinput.at(i+1) == parsedinput.at(i)))
						{
							synerror = true;
							break;
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
					else if (parsedinput.at(i) != ' ')
					{
						parsecmd += parsedinput.at(i);
						if (!isalpha(parsedinput.at(i+1)))
						{
							cmds.push_back(parsecmd);
							parsecmd.clear();
						}
					}
				}
				if(synerror == true) break;
				seploopdone = true;
			}
		}

		if (synerror != true)
		{
				
			cmds.push_back(string(";"));

			vector<string> newvec;
			for (unsigned i = 0; i < cmds.size(); i++)
			
				if (cmds.at(i) == "&&" || cmds.at(i) == "||" || cmds.at(i) == ";")
				{
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
					if (cmds.at(i) == "||" && execret == 0) break; 
					if (cmds.at(i) == ";" && i == cmds.size()-1) break;
				}
				else if (cmds.at(i) == "exit" && (cmds.at(i-1) == ";" || cmds.at(i-1) == "&&"))
				{
					killrshell = true;
					break;
				}
				else
				{
					newvec.push_back(cmds.at(i));
				}
			}
			if (synerror == true)
			{
				printf("Syntax error.\n");
			}
		}
	
	return 0;
} 
