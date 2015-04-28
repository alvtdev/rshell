#ifndef LS_H
#define LS_H

#include <iostream>
#include <vector>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <libgen.h>
using namespace std; 

//function for comparing cstrings
bool compcstrings(const char* cs1, const char* cs2)
{
	//copy contents into temporary cstrings
	char* a = new char [strlen(cs1) + 1];
	strcpy(a, cs1);
	//change all letters to lowercase for ease of comparison 
	//comparing 'A' to 'a', for example, wouldn't be wise.
	for (unsigned i=0; i < strlen(cs1); i++)
	{
		a[i] = tolower(a[i]);
	}
	//rinse and repeat for 2nd cstring
	char* b = new char [strlen(cs2) + 1];
	strcpy(b, cs2);
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
	//assume terminal width is 72 or 80, 
	//TODO: format print accordingly.
	for (unsigned i = 0; i < x.size(); i++)
	{
		printf("%s  ", basename(x.at(i)));
	}
	cout << endl;
	return;
}

void printlist(const vector<char*> x)
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
		stat(x.at(j), &temp);
		if (errno)
		{
			perror("formatting stat failed");
			exit(1);
		}
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
		stat(x.at(i), &s);
		if (errno != 0)
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
	
	return;
}

#endif
