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
   File Name: server.cpp
   Description:
   This file has the implementation of the methods specified in "server.h" 
   and interface "commonInterface.h"
   It has the implementation of functionality specific to server mode of the 
   program      
 */



#include "../include/server.h"

/*
Default constructor for the server class.
*/
server::server()
{
	commonInterface::mMyIP = socketClass::getMyIP();

	commonInterface::mMode = SERVER;
	FD_ZERO(&mClientSet);

	//commonInterface::mSocket = new socketClass(mMode);
	commonInterface::mWindow = new displayWindow(mMode);
	mPeerList.clear();
}

/*
Parameterized constructor for the server class that takes the parameter as the port number.
This is the port entered by the user.
*/
server::server(int port)
{
	stringstream stream;
	stream<<port;
	stream>>commonInterface::mPort;

	commonInterface::mMyIP = socketClass::getMyIP();
	commonInterface::mMode = SERVER;
	commonInterface::mWindow = new displayWindow(commonInterface::mMode);
	commonInterface::mSocket = new socketClass(commonInterface::mMode);

	mPeerList.clear();
	FD_ZERO(&mClientSet);

}

/*
Main function that implements the functionality of the server.
1) Firstly it starts the server on the port mentioned
2) Then adds this socket's fd and stdin's fd in the a fd_Set
3) set select on this fd_set.
4) It then triggers the code in case of any user's input/network activity
6) In case of new connection, adds the new sockfd in the reader fd_set
7) In case of any data received, calls the handleRecv() function.
*/
void server::run()
{
	if(!mSocket->initServerSocket(mPort))
	{
                printMessage("Failed to start the server on the port "+mPort);
                printMessage("The reason for this failure could be:");
                printMessage("\t-Port is being used by another application running on this machine");
                printMessage("\t-Port specified falls in the reserved ports range.");
                printMessage("Closing the server");
                return;
	}

	printMessage("\nStarting the Server..........................");

        if(!mWindow)
                mWindow = new displayWindow();

        //mWindow->displayMainMenu();

        string command;
        int retVal = SUCCESS;

	//Setup for using the select() command
	fd_set readerSet; //set from which we need to read
	fd_set activeSet;

	FD_ZERO(&activeSet); //clear the "readfd" set

	FD_SET(mSocket->mSocketFD, &readerSet); //Set my socketFD in the reader set
	FD_SET(STDIN, &readerSet);
	int maxFD = mSocket->mSocketFD;
	int nextMaxFD = maxFD;
	
	activeSet = readerSet;
	int selectRetVal;
	
        while(retVal != EXIT_PROGRAM)
        {
		cout<<"\n[PA1]$";
        	fflush(stdout);

                retVal = SUCCESS;
                command.clear();
		readerSet = activeSet;
		selectRetVal = select(maxFD+1, &readerSet, NULL,NULL,NULL);
		fflush(stdin);
		//fflush(stdout);

		if(selectRetVal < 0)
		{
			cout<<"\n ERROR server::run() select() failed"<<endl;
			break;
		}

		int currSockFD = 0;
		int newSocketFD = -1;
		if(selectRetVal > 0)
		{
			for (currSockFD =0; currSockFD <= maxFD; currSockFD++)
			{
				if(FD_ISSET(currSockFD, &readerSet))
				{
					//Check if we have to take an input from the standard input
					if(currSockFD == STDIN)
					{
						command = mWindow->runShell();
						if(command.length() <= 0)
							continue;
						else
							retVal = runCommand(command);
					}
					else if(currSockFD == mSocket->mSocketFD) //Check if we have a new connection request
					{
						newSocketFD = mSocket->acceptConnection();
						if(newSocketFD == ERROR)
						{
							retVal = EXIT_PROGRAM;
							continue;
						}
						else
						{
							FD_SET(newSocketFD, &activeSet);
							FD_SET(newSocketFD, &mClientSet);
						}

						if(newSocketFD > maxFD)
						{
							nextMaxFD = maxFD;		
							maxFD = newSocketFD;
						}
					}
					else
					{
						int recvBytes = 0;
						recvBytes = handleRecv(currSockFD);
						if(recvBytes <= 0)
        	                                {
							handleExit(currSockFD);
							if(currSockFD == maxFD)
								maxFD = nextMaxFD;
							FD_CLR(currSockFD, &activeSet);
                                	        }        
					}
				}
				
			}
		
		}
        }
	//FD_ZERO(&activeSet);	
	//FD_ZERO(&readerSet);
        //      fflush(stdin);
        //    fflush(stdout);
 
	cout<<"\n";
}

/*
This is just a stub to implement the methods specified in the commonInteface.
*/

bool server::registerCommand(string command)
{
	//Just a stub
	//Must not be called in any condition.
	return false;
}

/*
Handles the event when the user presses the connect command.
*/       
bool server::connectCommand(string command)
{
	printMessage("");
	printMessage("Server is already connected to all the clients(if any) present in the system");
	printMessage("Use the \"LIST\" command to check for the connected clients");
	printMessage("");
	return true;
}
        
/*
Methods called in case the user enters the "LIST" command.
It just prints out all the peers currently registered with the server.
*/
void server::listCommand()
{
        int id = 0;
	if(mPeerList.size() <= 0)
	{
		cout<<"\n No client has been registerd till now"<<endl;
		return;
	}
	cout<<"\n\n";

        const char *hostname;
        const char *ip_addr;
        int port_num = 0;

        int host_id = 0;

        for(int i = 0; i < mPeerList.size();i++)
        {
		hostname = mPeerList[i].mName.c_str();
                ip_addr = mPeerList[i].mIP.c_str();
                port_num = stringToInt(mPeerList[i].mPort);
		host_id++;

		printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
        }

}

/*
Handles the case when the user tries to run terminate command on the server
*/      
void server::terminateCommand(string command)
{
        printMessage("");
        printMessage("This command should be run on the client only");
        printMessage("Terminate command cannot be run on the server");
        printMessage("");

}
  
/*
stub to implement the methods of the interface
*/ 
void server::exitCommand()
{

}

/*
Handles the case when the user tries to run upload command on the server
*/ 
void server::uploadCommand(string command)
{
        printMessage("");
        printMessage("This command should be run on the client only");
        printMessage("No files should be downloaded on or uploaded from the server");
        printMessage("");
}
 
/*
Handles the case when the user tries to run download command on the server
*/
void server::downloadCommand(string command)
{
	//This command must not be executed on the server.
	printMessage("");
	printMessage("This command should be run on the client only");
	printMessage("No files should be downloaded on or uploaded from the server");
	printMessage("");	
}

/*
For getting the statistics for the server
a) Server send the request for the stats to all the registered clients
b) Clients sends their stats to the server
c) Server on getting the stats from the clients, it updates its stats.
d) display the stats.
*/
void server::statCommand()
{
	if(mPeerList.size() <= 0)
	{
		listCommand();
	}

	//So we have the peer in the system, send a request for the update from each one of the client.
	//And recv an update from each client.
	//Format for the update request "STAT"
	//Format for the reply "
	bool done = false;
	string request = "STATS";
	string reply;
	int recvBytes = 0;
	char msgPacket[1024]; //This will limit the length of the stat command

	for(int i = 0; i<mPeerList.size(); i++)
	{
		if(mPeerList[i].mSockFD > 0)
		{
			reply = "";
			done = mSocket->sendMessage(request, mPeerList[i].mSockFD);
			if(!done)
			{
				printMessage("Failed to get the updates from the peer "+mPeerList[i].mName);
			}
			
			//Get the reply from the peer, update the stats from this peer
			memset(msgPacket, 0, sizeof(char)*1024);
			recvBytes = recv(mPeerList[i].mSockFD, msgPacket, 1024, 0);
			if(recvBytes <= 0)
			{
				continue;
			}

			//So now i have the reply, update the stats.
			updateStats(mPeerList[i].mName, msgPacket);
		}	
	}

	displayStats();	
}

/*
Handles the message sent by the peers having the stats.
It parses the message and updates the stats for the 
peer having the peerIP
*/
void server::updateStats(string peerIP, string message)
{
	if(message.length() <= 0 || peerIP.length() <= 0)
		return;


	vector<string> tokens = splitMessage(message.c_str(), PEER_SEPERATOR);
	
	if(tokens.size() <= 1)
	{
		return;
	}	

	vector<string> statTokens;
	vector<stats> peerStats;
	peerStats.clear();

	for(int i = 1; i<tokens.size(); i++)
	{
		statTokens.clear();
		statTokens = splitMessage(tokens[i].c_str(), ':');
		stats newStats;
		if(statTokens.size() == 5)
		{
			newStats.mPeerName = statTokens[0];
			newStats.mUploadSpeed = stringToInt(statTokens[1]);
			newStats.mUploadCount = stringToInt(statTokens[2]);
                        newStats.mDownloadSpeed = stringToInt(statTokens[3]);
                        newStats.mDownloadCount = stringToInt(statTokens[4]);	
			peerStats.push_back(newStats);
		}
	}
	
	//So now we have the vector having the stats.
	if(peerStats.size() > 0)
	{
		mServerStats[peerIP] = peerStats;
	}

}

/*
Handles the functionality of displaying the statistics on the server in the 
format specified in the PA1 document
*/
void server::displayStats()
{
	if(mServerStats.size() == 0)
		return;

        printf("%-30s%-30s%-15s%-20s%-15s%-20s", "Host Name1 ", "Host Name2","Total", "Average upload", "Total", "Average download");
        printf("\n%-60s%-15s%-20s%-15s%-20s","", "Uploads", "Speed(bpms)", "Downloads", "Speed(bpms)");
	printf("\n");

	map<string, vector<stats> >::iterator itr;
	vector<stats> tempStats;
	string hostName1;
	for(itr = mServerStats.begin(); itr != mServerStats.end(); itr++)
	{
		tempStats.clear();
		tempStats = itr->second;
		hostName1 = "";
		hostName1 = itr->first;
		
		for(int i = 0;i< tempStats.size(); i++)
		{
			printf("%-30s%-30s%-15ld%-20ld%-15ld%-20ld\n", hostName1.c_str(), tempStats[i].mPeerName.c_str(),tempStats[i].mUploadCount, tempStats[i].getAvgUploadSpeed(),
								tempStats[i].mDownloadCount, tempStats[i].getAvgDownloadSpeed());
		}
	}	
		
	printMessage("");
}


/*
Function that handles the recv() event triggered in the select().
It parses the message received and triggers an appropriate action
corresponding to this message.
*/
int server::handleRecv(int recvSockFD)
{
	int recvBytes = 0;
	char msgPacket[MAX_PACKET_LEN];
	
	memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);
	recvBytes = recv(recvSockFD, msgPacket, MAX_PACKET_LEN, 0);
	
	if(recvBytes <= 0)
		return recvBytes;
	
	char msg = checkMessage(msgPacket);
	
	switch(msg)
	{
		case REGISTER:
			handleRegister(msgPacket, recvSockFD);
			break;
		default:
			break;
	}
	
	return recvBytes;	
}

/*
This methods handles the registeration of client
First it takes the message, validates it.
Then it sends the list of current clients back to the client who send the register request
Then it broadcasts the information of this newly registered client to all the other clients
*/

void server::handleRegister(char *msg, int sockFD)
{
	if(!msg || (strlen(msg) <= 0))
	{
		if(LOG_LEVEL == HIGH)
			printMessage("\nhandleRegister, empty Message sent");
		return;
	}

	vector<string> tokens = splitMessage(msg, ':');
	if(tokens.size() < 3 )
		return;
	
	string clientIP = tokens[1];
	string clientPort = tokens[2];
	
	//send an ack with list of current clients to the client who send this message

	bool done =false;
	string peerList = "ACK";
	peerList = peerList + PEER_SEPERATOR + getPeerList();
	done  = mSocket->sendMessage(peerList,sockFD);
	if(!done)
	{
		if(LOG_LEVEL == HIGH)
			printMessage("\nfailed to send the register ack");
		return;
	}

	done  = broadCast(msg);
	if(!done)
	{
		if(LOG_LEVEL == HIGH)
			printMessage("\nfailed to brodcast the message ");
		return;
	}

	//Now the client is registered, add it in the list
	peer tempPeer;
	tempPeer.mSockFD = sockFD;
	tempPeer.mPort = clientPort;

	if(isdigit(clientIP[0]))
	{
		tempPeer.mIP = clientIP;
		tempPeer.mName = mSocket->getNameFromIP(clientIP);
	}
	else
	{
		tempPeer.mIP = mSocket->getIPFromName(clientIP);
		tempPeer.mName = clientIP;
	}
	mPeerList.push_back(tempPeer);

	printMessage("\nClient "+tempPeer.mName+" has been registered with this system");
	listCommand();	
}


/*
Handles the event when a peer has exited from the system.
Just send the information of the exited peer to all the 
remaining peers so that they can update their peer list
accordingly.
*/
void server::handleExit(int sockFD)
{
	if(sockFD <= 0)
		return;

	vector<peer>::iterator currPeer;
	bool found = false;
	stringstream stream;
	stream.clear();
	string peerIP = "";
	for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
	{
		if(currPeer->mSockFD == sockFD)
		{
			stream<<"Peer "<<currPeer->mIP<<" has exited the system";
			stream<<"\nClosing the connection from this peer";
			printMessage(stream.str());
			peerIP = currPeer->mIP;
			mPeerList.erase(currPeer);
			found = true;
			break;
		}
	}

	bool done = false;	
	string message = "EXIT:"+peerIP;
	for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
	{
		if(currPeer->mSockFD > 0)
		{
			done = mSocket->sendMessage(message, currPeer->mSockFD);
		}
	}	
}

/*
Does exactly what the name says
*/
bool server::broadCast(char *msg)
{
	//Check if this is the first client registering
	if(mPeerList.size() <= 0)
		return true;

	vector<peer>::iterator itr;

	bool done = true;
	for(itr = mPeerList.begin();itr != mPeerList.end();itr++)
	{
		if(itr->mSockFD > 0)
			done = mSocket->sendMessage(msg, itr->mSockFD);
	}

	return done;
}

/*
This methods prepares the message having the information of the 
peers present in the mPeerList.
*/
string server::getPeerList()
{
	if(mPeerList.size() <= 0)
		return "";
	string message = "";
	vector<peer>::iterator itr;
	for(itr=mPeerList.begin();itr != mPeerList.end();itr++)
	{
		message = message+itr->mIP + SEPERATOR + itr->mName + SEPERATOR + itr->mPort + PEER_SEPERATOR;
	}

	message = message.substr(0, message.length()-1);
	return message;
}

//Destructor for the server.
//Nothing to be done here as we have not used any DMA in server.
server::~server()
{

}

