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
			if (synerror != true)	synerror = execcmds(cmds);
			if (synerror == true)	printf("Syntax error.\n");
		}
	}
	return 0;
}
