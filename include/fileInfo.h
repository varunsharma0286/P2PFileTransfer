#include "timer.h"

#include<fstream>
#include<stdio.h>
#include<iostream>
using namespace std;



#ifndef _FILEINFO_H
#define _FILEINFO_H
class fileInfo
{
public:
	ofstream file;
	long int fileSize;
	long int origFileSize;
	string fileName;
	timer fileTimer;

	inline fileInfo()
	{
		fileSize = 0;
		fileName = "";
		origFileSize = 0;
	}

	~fileInfo()
	{
		if(file.is_open())
		{
			file.close();
		}
	}
};
#endif
