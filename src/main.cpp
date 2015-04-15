#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
using namespace boost;



/*TODO: refine the commandexec algorithm before proceeding further.
This current algorithm was an initial attempt.
It does not work, and will be replaced. */


//function for iterating through parsed string
void commandexec(vector<string> x)
{
	//position of iterator
	int pos = 0;
	while (pos < x.size())
	{
		//if 'ls' is found
		if (x.at(pos) == "ls")
		{
			int pid = fork();
			if (pid == -1) //if pid is -s, an error has occurred.
			{
				perror("fork");
				exit(1);
			}
			else if (pid == 0) //if pid is 0, you are in child process. execvp here.
			{
//				execvp(x.at(pos), x.at(pos+1)); 
			}
			else
				wait(0);
		}
	}
}


int main (int argc, char **argv)
{

	//bool value use to determine when to kill rshell loop
	bool killrshell = false; 

	while (!killrshell)
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

		//variables to keep track of whitespaces
		int wsbegin = 0;
		int wsend = 0;
		string parsedinput = rawinput;


		//first remove leading whitespaces at the very beginning of the string
		while (parsedinput.at(wsend) == ' ') wsend++;
		parsedinput = parsedinput.substr(wsend, parsedinput.size()-1);
		//then reinit wsend for removing inner whitespaces
		wsend = 0;

		//next remove trailing whitespaces at the end of the string
		wsbegin = parsedinput.size()-1;
		if(parsedinput.at(wsbegin) == ' ')
		{
			while (parsedinput.at(wsbegin) == ' ')
				--wsbegin;
		
			parsedinput = parsedinput.substr(0, wsbegin+1);
		}
		//then reinit wsbegin for removing inner whitespaces 
		wsbegin = 0;

		//remove extra whitespaces in the middle of the string
		for (int i=0; i<parsedinput.size(); i++) 
		{
		}
			


		parsedinput += ';';
		cout << "new input:" << parsedinput << endl;
		
		//initial scan for syntax errors
		if (!isalpha(parsedinput.at(0))) synerror = true;

		//bool variable to tell when the inner for loop is done
		bool seploopdone = false;

		//iterate through string and separate into commands and connectors
		while (synerror != true && seploopdone == false)
		{
			string parsecmd;
			for (int i=0; i<parsedinput.size(); i++)
			{
				if (i >= parsedinput.size()-1) break; //accomodates for concatenated ; at the end of the string
				//checks for & and |
				if ( (parsedinput.at(i) == '&' || parsedinput.at(i) == '|') && parsedinput.at(i+1) == parsedinput.at(i) )
				{
					//if 3 &&& |||, or 1 & | -- syntax error
					if (parsedinput.at(i+2) == parsedinput.at(i))
					{
						synerror =true;
						break;
					}
					else if (parsedinput.at(i+1) != parsedinput.at(i))
					{
						synerror=true;
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
				else 
				{
					parsecmd += parsedinput.at(i);
					if (!isalpha(parsedinput.at(i+1)))
					{
						cmds.push_back(parsecmd);
						parsecmd.clear();
					}
				}
			}
			if(synerror = true) break;
			seploopdone = true;
		}

		//PARSING TEST OUTPUT
		for (int i=0; i<cmds.size(); i++)
			cout << cmds.at(i) << endl;
	
		//temporary input handling -- if exit was not entered, output the input.
		printf("\"%s\" was entered.\n", rawinput.c_str());
		

	}

	return 0;
} 
