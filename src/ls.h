#ifndef LS_H
#define LS_H

#include <iostream>
#include <vector>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
using namespace std; 

//function for comparing cstrings
bool compcstrings(const char* cs1, const char* cs2)
{
	//copy contents into temporary cstrings
	char* a = new char [strlen(cs1) + 1];
	strcpy(a, cs1);
	if (a[0] == '.')
	{
		memmove(a, a+1, strlen(a+1));
	}
	//change all letters to lowercase for ease of comparison 
	//comparing 'A' to 'a', for example, wouldn't be wise.
	for (unsigned i=0; i < strlen(cs1); i++)
	{
		a[i] = tolower(a[i]);
	}
	//rinse and repeat for 2nd cstring
	char* b = new char [strlen(cs2) + 1];
	strcpy(b, cs2);

	if (b[0] == '.')
	{
		memmove(b, b+1, strlen(b+1));
	}

	for (unsigned i=0; i < strlen(cs2); i++)
	{
		b[i] = tolower(b[i]);
	}

	//compare new cstrings
	bool bret = strcmp(a, b) < 0;

	//delete cstrings so we don't have a memory leak
	delete[] a;
	delete[] b;

	return bret;
}

void printnorm(vector<char*> x)
{
	//get terminal size
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	unsigned int sperl = w.ws_col;

	//bool variable to determine print type
	bool multline = false; //if false, 2 spaces between file names

	//first iterate through to determine if multiple lines neet to be printed
	int tempsizecount = 0;
	for (unsigned i=0; i < x.size(); i++)
	{ 
		tempsizecount += (strlen(basename(x.at(i)))+2); //+2 accounts for spaces
		if (tempsizecount >= sperl)
		{
			multline = true;
			break;
		}
	}

	//if only one line is necessary, output all names with 2 spaces in between
	if (multline == false)
	{
		for (unsigned i = 0; i < x.size(); i++)
		{
			printf("%s  ", basename(x.at(i)));
		}
		printf("\n");
	}
	//else print accordingly
	else
	{
		//test outputs
		//printf("lines %d\n", w.ws_row);
		//printf("columns %d\n", w.ws_col);

		//number of columns = terminal width
		sperl = w.ws_col;

		//loop to find size of largest name
		int bnlen = strlen(basename(x.at(0)));
		//test output
		//cout << "longest file name: " << basename(x.at(0)) << endl;
		//cout << "file name length: " << bnlen << endl;
		for (unsigned i = 0; i < x.size(); i++)
		{
			if (strlen(basename(x.at(i))) > bnlen)
			{
				bnlen = strlen(basename(x.at(i)));
				//cout << "new longest file name: " << basename(x.at(i)) << endl;
				//cout << "file name length: " << bnlen << endl;
			}
		}

		//max number of words per line
		int wplmax = sperl/bnlen - 2;

		//print list of files
		//temp var to keep track of length
		int tempw = 0;
		for (unsigned i=0; i < x.size(); i++) 
		{
			tempw++;
			printf("%-*s", bnlen, basename(x.at(i)));
			if (tempw > wplmax)
			{
				printf("\n");
				tempw = 0;
			}
		}
	}

	return;
}

void printlist(const vector<char*> x)
{
	//integer used to count number of blocks used
	int blockcount = 0;
	if (!x.empty())
	{
		struct stat tempblockcount;
		for (unsigned i=0; i<x.size(); i++) 
		{
			stat(x.at(i), &tempblockcount);
			if (errno)
			{
				perror("block count stat failed");
				exit(1);
			}
			blockcount += tempblockcount.st_blocks;
		}
	}


	//first output total blocks used
	printf("total %i\n", blockcount/2);

	if (!x.empty())
	{
		//initial stat call to obtain first size output format int
		struct stat temp;
		stat(x.at(0), &temp);
		if (errno)
		{
			perror("initial stat failed");
			exit(1);
		}
		
		//integer used to format printf for size output
		int sizeform = log10(temp.st_size)+1;


		//now iterate through the rest of the files to find largest size
		for (unsigned j=1; j<x.size(); j++)
		{
			//cout << "stat called on: " << x.at(j) << endl;
			if (-1 == stat(x.at(j), &temp))
			{
				perror("formatting stat failed");
				cout << "ERRNO = " << errno << endl;
				exit(1);
			}
			//if the length is larger than existing documented length, overwrite 
			if ((log10(temp.st_size)+1) > sizeform) 
			{
				sizeform = log10(temp.st_size)+1;
			}
		}


		//iterate through and print out all info
		for (unsigned i=0; i< x.size(); i++)
		{
			//obtain info of file at x.at(i)
	//		cout << "stat being called on: " << x.at(i) << endl;
			struct stat s;
			if (-1 == stat(x.at(i), &s))
			{
				perror("stat failed");
				exit(1);
			}

			//output filetype
			cout << ((S_IFREG & s.st_mode)?"-":
				(S_IFDIR & s.st_mode)?"d":
				(S_IFBLK & s.st_mode)?"b":
				(S_IFLNK & s.st_mode)?"l":
				(S_IFCHR & s.st_mode)?"c":
				(S_IFSOCK & s.st_mode)?"s":
				(S_IFIFO & s.st_mode)?"f":"?");

			//output permissions
			cout << ((S_IRUSR & s.st_mode)?"r":"-")
				<< ((S_IWUSR & s.st_mode)?"w":"-")
				<< ((S_IXUSR & s.st_mode)?"x":"-")
				<< ((S_IRGRP & s.st_mode)?"r":"-")
				<< ((S_IWGRP & s.st_mode)?"w":"-")
				<< ((S_IXGRP & s.st_mode)?"x":"-")
				<< ((S_IROTH & s.st_mode)?"r":"-")
				<< ((S_IWOTH & s.st_mode)?"w":"-")
				<< ((S_IXOTH & s.st_mode)?"x":"-")
				<< " ";

			printf("%li ", s.st_nlink);

			//get user info 
			struct passwd* userinfo;
			userinfo = getpwuid(s.st_uid);
			if (errno)
			{
				perror("getpwuid failed");
				exit(1);
			}
			printf("%s ", userinfo->pw_name);

			//get group info
			struct passwd* groupinfo;
			groupinfo = getpwuid(s.st_uid);
			if (errno)
			{
				perror("getpwuid failed");
				exit(1);
			}
			printf("%s ", groupinfo->pw_name);

			//print size
			printf("%*li ", sizeform, s.st_size);
	//		printf("%li ", s.st_size);
		
			//obtain and print time last modified 
			struct tm lastmodtime;
			localtime_r(&s.st_mtime, &lastmodtime);
			//create null terminated array for time output
			char formattedtime[30];
			strftime((char*)formattedtime, 30, "%b %d %H:%M", &lastmodtime);
			printf("%s ", formattedtime);


			//TODO: add color
			printf("%s \n", basename(x.at(i)));
		}
	}
	
	return;
}

#endif
