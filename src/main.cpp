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
	while (true)
	{
		printprompt();
		string rawinput = getinput();
		vector<string> cmds;
		if (!rawinput.empty())
		{
			string parsedinput = parseinput(rawinput);		
			//bool will store syntax error
			bool synerror = makecmds(parsedinput, cmds);
			int pcount = pipecount(parsedinput);		
			if (synerror != true)	synerror = execcmds(cmds, pcount);
			if (synerror == true)	printf("Syntax error.\n");
		}
	}
	return 0;
}
