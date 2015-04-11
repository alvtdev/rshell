#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
//using namespace std;

int main (int argc, char **argv)
{

	//bool value use to determine when to kill rshell loop
	bool killrshell = false; 
	

	while (!killrshell)
	{

		//string will store raw user input
		std::string rawinput;
		
	
		//simple command prompt print
		printf("%s", "$ ");

		//obtain user input commands
		std::getline(std::cin, rawinput);

		
		//TODO: Parse raw input string for commands and connectors

		
		//temporary break loop condition: user inputs 'exit' 
		if (rawinput.substr(0,4) == "exit") 
		{
			killrshell = true;
			break;
		}
		
		//temporary input handling -- if exit was not entered, output the input.
		printf("\"%s\" was entered.\n", rawinput.c_str());
		

	}

	return 0;
} 
