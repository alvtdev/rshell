#include <iostream> 
#include <sys/stat.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include "Timer.h"
using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "ERROR: Not enough arguments were passed in." << endl;
		return 0;
	}

	//char str[BUFSIZ] = {"/usr/share/dict/linux.words"};
	//initial variables to store files
	char srcf[BUFSIZ];
	strcpy(srcf, argv[1]);
	char dstf[BUFSIZ];
	strcpy(dstf, argv[2]);
	vector<char*> filenames;
	struct stat s;


	if ( -1 == stat(srcf, &s))
	{
		cout << "ERROR: source file does not exist";
		exit (1);
	}
	

	struct stat t1;
	if (0 == stat(dstf, &t1))
	{
		cout << "ERROR:destination file already exists" << endl; 
		exit (1);
	}
	
	

		//method 1 of lab4 
		cout << "###Method 1 - istream::get() and ostream::put()###" << endl;
		//prep timer data for timing algorithm
		Timer t;
		double eTime;
		t.start();
		//the actual algorithm
		ifstream inFS(srcf);
		ofstream outFS(dstf);
		char ch;
		while(inFS.get(ch))
		{
			outFS.put(ch);
		}
		//end time
		t.elapsedUserTime(eTime);
		inFS.close();
		outFS.close();
		cout << "Time taken: " << eTime << endl;

		//method 2 of lab4 
		cout << "###Method 2 - Unix read() and write(): 1 char at a time###" << endl;
		//prep timer data for timing algorithm
		Timer t2;
		double eTime2;
		t2.start();
		//the actual algorithm
		//open files for read/writing
		int fdnew;
		int fdold;
		if (-1 == (fdnew = open(dstf, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)))
		{
			perror("open() failed. ");
			exit(1);
		}
		if (-1 == (fdold = open(srcf, O_RDONLY)))
		{ perror("open() failed. ");
			exit(1);
		}
		//read files and write chars
		int size;
		char c[1];
		if (-1 == (size = read(fdold, c, sizeof(c))))
		{
			perror("read() failed. ");
			exit(1);
		}
		while (size > 0)
		{
			if (-1 == write(fdnew, c, size))
			{
				perror("write() failed. ");
				exit(1);
			}
			if (-1 == (size = read(fdold, c, sizeof(c))))
			{
				perror("read() failed. ");
				exit(1);
			}
		}
		//close files
		if (-1 == close(fdnew))
		{
			perror("close() failed. ");
			exit(1); 
		}
		if (-1 == close(fdold))
		{
			perror("close() failed. ");
			exit(1); 
		}
		//end time
		t2.elapsedUserTime(eTime2);
		cout << "Time taken: " << eTime2 << endl;

		//method 3 of lab4 
		cout << "###Method 3 - Unix read() and write(): 1 buffer at a time###" << endl;
		//prep timer data for timing algorithm
		Timer t3;
		double eTime3;
		t3.start();
		//the actual algorithm
		//open files for read/writing
		int fdnew3;
		int fdold3;
		if (-1 == (fdnew3 = open(dstf, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)))
		{
			perror("open() failed. ");
			exit(1);
		}
		if (-1 == (fdold3 = open(srcf, O_RDONLY)))
		{ perror("open() failed. ");
			exit(1);
		}
		//read files and write chars
		int size3;
		char c3[BUFSIZ];
		if (-1 == (size3 = read(fdold3, c3, sizeof(c3))))
		{
			perror("read() failed. ");
			exit(1);
		}
		while (size3 > 0)
		{
			if (-1 == write(fdnew3, c3, size3))
			{
				perror("write() failed. ");
				exit(1);
			}
			if (-1 == (size3 = read(fdold3, c3, sizeof(c3))))
			{
				perror("read() failed. ");
				exit(1);
			}
		}
		//close files
		if (-1 == close(fdnew3))
		{
			perror("close() failed. ");
			exit(1); 
		}
		if (-1 == close(fdold3))
		{
			perror("close() failed. ");
			exit(1); 
		}
		//end time
		t3.elapsedUserTime(eTime3);
		cout << "Time taken: " << eTime3 << endl;
	return 0;
}

