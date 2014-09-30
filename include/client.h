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
 * File Name: client.h
   Description: 
   Header file for the client.h class.
 */



#include "displayWindow.h"
#include "configFile.h"
#include "commonInterface.h"
#include "peer.h"
#include "timer.h"
#include "stats.h"
#include "fileInfo.h"

#include<fstream>
#include<string>
#include<cstring>
#include<stdio.h>
#include<iostream>
using namespace std;


#ifndef _CLIENT_H
#define _CLIENT_H

class client:public commonInterface
{
public:	
	string mIP;
	bool mRegistered;
	vector<peer> mPeerList;
	peer mServer;
	fd_set mReaderSet;
	fd_set mActiveSet;
	int mMaxFD;
	timer clientTimer;
	map<string, stats> mClientStats;
	map<int, fileInfo *> mDownloadMap;
	
	bool mDownloadFlag;

	client();
	client(int portNumber);
	void run();
	//int runCommand(string command);

	void testFileSend(string &, string &);
	//Command specific implementation
        bool registerCommand(string command);
      	bool connectCommand(string command);
        void listCommand();
	void listCommand(bool);
        void terminateCommand(string command);
        void exitCommand();
        void uploadCommand(string command);
        void downloadCommand(string command);
        void statCommand();
	bool broadCast(char *);
	bool setPeerList(char* msg);
	bool downloadFile(string&, int);

	//hanlder methods
	void handleRegister(char *, int);
	int handleRecv(int recvSockFD);
	void handleConnect(char *message, int peerSockFD);
	void handleExit(int);
	void handleExit(char *);
	void handleUpload(char *, int);
	void handleDownload(char *, int);
	void handleStats(char *);

	void checkDownloadStatus(int recvSockFD, char *packet, int recvBytes);
	bool validateId(int);	
	~client();
};

#endif
