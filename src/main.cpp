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
#include "rshell.h"
using namespace std;

int main (int argc, char** argv)
{
	while (cin.good())
	{
		printprompt();
		string rawinput = getinput();
		//vector of strings, will convert to array for execvp
		vector<string> cmds;
		//execute the following code ONLY IF STRING IS NOT EMPTY
		if (!rawinput.empty())
		{
			string parsedinput = parseinput(rawinput);		
			//bool will store syntax error
			bool synerror = makecmds(parsedinput, cmds);
			//test output: print contents of cmds
			/*
			cout << "-- Contents of cmds --" << endl; 
			for (unsigned i=0; i<cmds.size(); i++)
			{
				cout << "cmds.at(" << i << "): " << cmds.at(i) << endl << endl;
			}
			*/	
			if (synerror != true)	synerror = execcmds(cmds);
			if (synerror == true)	printf("Syntax error.\n");
		}
	}
	//printf("Goodbye\n");
	return 0;
}
