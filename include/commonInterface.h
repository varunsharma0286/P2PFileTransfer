/**
 * @vsharma7_assignment1
 * @author  Varun Sharma <vsharma7@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 * File Name: displayWindow.h
   Description: 
   Header file for the displayWindow.h class.
 */



#include<string>
#include<vector>
#include<stdio.h>
#include<iostream>
using namespace std;


#include "displayWindow.h"
#include "socketClass.h"

#ifndef _COMMONINTERFACE_H
#define _COMMONINTERFACE_H

class commonInterface
{
protected:
	commonInterface();
	virtual ~commonInterface();
public:
	string mMyIP;
	string mPort;
	displayWindow *mWindow;
	string mMode;
	socketClass *mSocket;
	string mMyName;

	//Implemented common methods
	int runCommand(string command);
	static vector<string> getCommandList(string input, char delimiter);
	string createMessage(vector<string> input);
	vector<string> splitMessage(const char *, char);
	void printMessage(string message);
	string extractFileName(string& name);
	inline long int stringToInt(string& intStr)
	{
		long int intVal = 0;
		stringstream stream;
		stream<<intStr;
		stream>>intVal;
		return intVal;
	}
	
	//Unimplemented methods that the client and server must implement
	virtual bool registerCommand(string command) = 0; 
	virtual bool connectCommand(string command) = 0;
	virtual void listCommand() = 0;
	virtual void terminateCommand(string command) = 0;
	virtual void exitCommand() = 0;
	virtual void uploadCommand(string command) = 0;
	virtual void downloadCommand(string command) = 0;
	virtual void statCommand() = 0;
	virtual char checkMessage(char *msg);	

	//unimplemented message handling methods that client and server must implement
	virtual void handleRegister(char *, int) = 0;
	virtual bool broadCast(char *) = 0;	
	virtual void handleExit(int sockFD) = 0;
};

#endif
