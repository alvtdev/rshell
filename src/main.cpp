#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <vector>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "rshell.h"
#include <pwd.h>
using namespace std;

int main (int argc, char** argv)
{
	currstate.sa_handler = handle;
	if (-1 == sigaction(SIGINT, &currstate, NULL))
	{
		perror("sigaction-SIGINT failed");
		exit(1);
	}
	if (-1 == sigaction(SIGTSTP, &currstate, NULL))
	{
		perror("sigaction-SIGTSTP failed");
		exit(1);
	}
	
	cout << "Welcome to Rshell!" << endl;

	while (true)
	{
		signalfound = false;
		vector<string> paths = getpaths();
		/*
		for (unsigned i=0; i < paths.size(); i++)
		{
			cout << "paths.at(" << i << ") = " << paths.at(i) << endl;
		}
		*/
		//get home directory
		const char *homedir;
		if ((homedir=getenv("HOME")) == NULL)
		{
			homedir = getpwuid(getuid()) ->pw_dir;
			if (homedir == NULL)
			{
				perror("get home directory failed");
				exit(1);
			}
		}
		printprompt();
		string rawinput; 
		getinput(rawinput);
		if (signalfound == true) continue;
		vector<string> cmds;
		if (!rawinput.empty())
		{
			string parsedinput = parseinput(rawinput);		
			//bool will store syntax error
			bool synerror = makecmds(parsedinput, cmds);
			/*
			for (unsigned i =0; i<cmds.size(); i++)
			{
				cout << i << " " << cmds.at(i) << endl;
			}
			*/
			int pcount = pipecount(parsedinput);		
//			bool cdfound = findcd(cmds);
//			cout << "cdfound = " << cdfound << endl;
			//bool cdfound = cdcheck(cmds);
			//cout << "pcount = " << pcount << endl;
			if (synerror != true)	synerror = execcmds(cmds, pcount, paths);
			if (synerror == true)	printf("Syntax error.\n");
		}
	}
	return 0;
}
