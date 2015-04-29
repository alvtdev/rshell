#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <linux/limits.h>

//custom ls header file
#include "ls.h"

using namespace std;

int main(int argc, char** argv)
{
	//bool variables for flag status
	//TODO: implement switch-cases to trigger flags
	bool aflag = false;
	bool lflag = false;
	bool Rflag = false;

	//keeps track of all arguments passed after ls
	vector<char*> lsargs;

	//sort through args for flags or directories
	for (unsigned i=1; i < argc; i++)
	{
		//if first char in char** is '-', the argument is a flag
		if (argv[i][0] == '-')
		{
			//if flag is detected, set flag bools accordingly
				for (unsigned j = 1; j < strlen(argv[i]); j++)
				{
					if (aflag == false && argv[i][j] == 'a') aflag = true;
					if (lflag == false && argv[i][j] == 'l') lflag = true;
					if (Rflag == false && argv[i][j] == 'R') Rflag = true;
				}
		}
		else //if the argument is not a flag, it is a parameter that should be kept
		{
			lsargs.push_back(argv[i]);
		}
	}

	//parsing and flag test output
	/* #define FLAGTEST(x) cout << #x " = " << (x==true)?"true":"false"; cout << endl;
	#define PARSETEST(x) for(unsigned i=0; i < x.size() ; i++) { cout << x.at(i) << endl; }

	FLAGTEST(aflag);
	FLAGTEST(lflag);
	FLAGTEST(Rflag);
	PARSETEST(lsargs); */
	
	//push back "." if arguments are not present.
	//accounts for an "ls [flag]" call without a directory passed in.
	if(lsargs.empty())
	{
		lsargs.push_back((char*)".");
	}

	//vector that stores all filenames in directory
	vector<char*> filenames;

	//get directory pointer 
	DIR* dirptr; 
	for (unsigned i=0; i < lsargs.size(); i++)
	{
		dirptr = opendir(lsargs[i]);
		if (dirptr == NULL)
		{
			perror("opendir failed");
			exit(1);
		}
		//get directory entry pointers and store file names
		struct dirent* de;
		while (de = readdir(dirptr))
		{
			if (errno !=0) 
			{
				perror("readdir failed");
				exit(1);
			}
			if (de->d_name[0] != '.') 
			{
				char* fncstr = new char[PATH_MAX];
				strcpy(fncstr, lsargs[i]);
				strcat(fncstr, "/");
				strcat(fncstr, de->d_name);
				filenames.push_back(fncstr);
			}
			if (de->d_name[0] == '.' && aflag)
			{
				char* fncstr = new char[PATH_MAX];
				strcpy(fncstr, lsargs[i]);
				strcat(fncstr, "/");
				strcat(fncstr, de->d_name);
				filenames.push_back(fncstr);
			}
		}
		delete[] de;
		
		sort(filenames.begin(), filenames.end(), compcstrings);

//test output for checking contents of filenames vector
/*
	for (unsigned i=0; i < filenames.size(); i++)
		cout << filenames.at(i) << ' ';
	cout << endl;
*/

	//print branches and conditions

	//if multiple ls args were put in, print folder first
		if (lsargs.size() >= 2 || Rflag) printf("%s:\n", lsargs.at(i));

		if (Rflag == true)
		{
		}
		else if (lflag == true)
		{
			printlist(filenames);
		}
		else
		{
			printnorm(filenames);
		}
		if (lsargs.size() >= 2 && i != (lsargs.size()-1)) printf("\n");

		//deallocate filenames memory
		for(unsigned j=0; j < filenames.size(); j++)
		{
			delete[] filenames.at(j);
		}
		filenames.clear();
		
		//closedir
		if (-1 == closedir(dirptr))
		{
			perror("closedir failed");
			exit(1);
		}
	}

	return 0;
}
