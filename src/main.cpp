#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h>
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
				pos++;
				execvp("ls", x);
			}
		}
	}
}


int main (int argc, char **argv)
{

	//bool value use to determine when to kill rshell loop
	bool killrshell = false; 
	

	while (!killrshell)
	{

		//string will store raw user input
		string rawinput;
		
	
		//simple command prompt print
		printf("%s", "$ ");

		//obtain user input commands
		std::getline(std::cin, rawinput);

		//temporary break loop condition: user inputs 'exit' 
		if (rawinput.substr(0,4) == "exit") 
		{
			killrshell = true;
			break;
		}

		//parse through raw input and remove comments if entered
		//removes everything after '#'
		if (rawinput.find('#') != std::string::npos) rawinput = rawinput.substr(0, rawinput.find('#'));
	
		
		//TODO: Parse raw input string for commands and connectors
		char_separator<char> delim(" -", "|");
		tokenizer< char_separator<char> > mytok(rawinput);

		
		//tokenizer test: storing tokenized objects in a vector
		vector<string> parsedinput;
		for(auto it = mytok.begin(); it != mytok.end(); ++it)
		{
			parsedinput.push_back(*it);
		}
		//token test output loop
		for (int i=0; i < parsedinput.size(); i++)
			cout << parsedinput.at(i) << ' ';
		
		cout << endl;
	
		//temporary input handling -- if exit was not entered, output the input.
		printf("\"%s\" was entered.\n", rawinput.c_str());
		

	}

	return 0;
} 
