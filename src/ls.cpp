#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>

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
	#define FLAGTEST(x) cout << #x " = " << (x==true)?"true":"false"; cout << endl;
	#define PARSETEST(x) for(unsigned i=0; i < x.size() ; i++) { cout << x.at(i) << endl; }

	FLAGTEST(aflag);
	FLAGTEST(lflag);
	FLAGTEST(Rflag);

	PARSETEST(lsargs);


	return 0;
}
