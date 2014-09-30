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
 * File Name: server.h
   Description: 
   Header file for the server.h class.
 */



#include "displayWindow.h"
#include "configFile.h"
#include "socketClass.h"
#include "commonInterface.h"
#include "peer.h"
#include "timer.h"
#include "stats.h"

#include<stdio.h>
#include<vector>
#include<string>
#include<iostream>
using namespace std;


#ifndef _SERVER_H
#define _SERVER_H

class server:public commonInterface
{
private:
	fd_set mClientSet;
	vector<peer> mPeerList;
public:
	timer serverTimer;
	map<string, vector<stats> > mServerStats;	

        server();
	server(int port);
	void run();

	int handleRecv(int recvSockFD);
        bool registerCommand(string command);
        bool connectCommand(string command);
        void listCommand();
        void terminateCommand(string command);
        void exitCommand();
        void uploadCommand(string command);
        void downloadCommand(string command);
        void statCommand();
	string getPeerList();
	bool broadCast(char *); 
	void updateStats(string, string);
	void displayStats();
	//handle methods
	void handleRegister(char* msg, int sockFD);
	void handleExit(int);	
	~server();
};

#endif

