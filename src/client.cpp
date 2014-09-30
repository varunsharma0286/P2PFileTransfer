
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
 * File Name: client.cpp
   Description:
   This file has the implementation of the methods specified in "client.h" 
   and interface "commonInterface.h"
   It has the implementation of functionality specific to client mode of the 
   program	
 */


#include "../include/client.h"

//Ideally the default constructor must not called as we need the port information

/*
Default constructor for the client.
*/

client::client()
{
	mMyIP = socketClass::getMyIP();
	mWindow = new displayWindow(mMode);
	mRegistered = false;
	mPeerList.clear();
	FD_ZERO(&mReaderSet);
	FD_ZERO(&mActiveSet);
	mMaxFD = -1;
	mClientStats.clear();
	
	mDownloadMap.clear();
}

/*
Parameterized constructor for the client class.
Takes the port number of the client as the argument
*/

client::client(int portNumber)
{
        stringstream stream;
        stream<<portNumber;
        stream>>commonInterface::mPort;

        commonInterface::mMode = CLIENT;
        commonInterface::mMyIP = socketClass::getMyIP();
        commonInterface::mSocket = new socketClass(CLIENT);
        commonInterface::mWindow = new displayWindow(CLIENT);	
	
	mMyName = mSocket->getNameFromIP(mMyIP);
	mRegistered = false;
	mPeerList.clear();
	mClientStats.clear();
        FD_ZERO(&mReaderSet);
        FD_ZERO(&mActiveSet);	
	mMaxFD = -1;

	mDownloadMap.clear();
}


/*
Main function that runs the clients.
It first creates the listening socket on the port of the client that is specified at command line
Then it runs the client and uses select() for multiplexing of the std. input and the network activities
*/

void client::run()
{
	//Initiating the listening port of this client
	bool done = mSocket->initServerSocket(mPort);
	if(!done)
	{
		printMessage("Failed to start the client on the port "+mPort);
		printMessage("The reason for this failure could be:");
		printMessage("\t-Port is being used by another application running on this machine");
		printMessage("\t-Port specified falls in the reserved ports range.");
		printMessage("Closing the client");
		return;
	}

	printMessage("\nStarting the Client..........................");
        if(!mWindow)
                mWindow = new displayWindow();

        //mWindow->displayMainMenu();

	//cout<<"\n\n[PA1]$";
	//fflush(stdout);
        string command;
        int retVal = SUCCESS;

	//Setup for using the select() command
	FD_ZERO(&mReaderSet);
	FD_ZERO(&mActiveSet);

	FD_SET(mSocket->mSocketFD, &mReaderSet); //Set my socketFD in the reader set
	FD_SET(STDIN, &mReaderSet);

	int maxFD = mSocket->mSocketFD;
	int nextMaxFD = maxFD;
	
	mActiveSet = mReaderSet;
	int selectRetVal;
	
	mMaxFD = maxFD;
        while(retVal != EXIT_PROGRAM)
        {
		if(!mDownloadFlag)
		{
		        cout<<"\n[PA1]$";
        		fflush(stdout);
		}
                retVal = SUCCESS;
                command.clear();
		mReaderSet = mActiveSet;//readerSet = activeSet;
		selectRetVal = select(maxFD+1, &mReaderSet, NULL,NULL,NULL);
		//fflush(stdin);
		//fflush(stdout);

		if(selectRetVal < 0)
		{
			cout<<"\n ERROR server::run() select() failed"<<endl;
			perror("select");
			break;
		}

		int currSockFD = 0;
		int newSocketFD = -1;
		if(selectRetVal > 0)
		{
			for (currSockFD =0; currSockFD <= maxFD; currSockFD++)
			{
				if(FD_ISSET(currSockFD, &mReaderSet))
				{
					//Check if we have to take an input from the standard input
					if(currSockFD == STDIN)
					{
						command = mWindow->runShell();
						if(command.length() <= 0)
							continue;
						else
							retVal = runCommand(command);
						
						//This is to reset the maxFD so that we can include the 
						//Newly create sockFD's form the "CONNECT" and "REGISTER"
						if(mMaxFD > maxFD)
						{
							nextMaxFD = maxFD;
							maxFD = mMaxFD;
						}
						else
						{	//This checks if we have executed the "TERMINATE" command.
							if(command[0] == 'T')
							{
							        mMaxFD = 0;
								nextMaxFD = 0;
							        for(int i=0;i<=maxFD;i++)
							        {
							                if(FD_ISSET(i, &mActiveSet))
							                {
                        							if(i > mMaxFD)
                        							{
											nextMaxFD = mMaxFD;
                                							mMaxFD = i;
                        							}
                							}
        							}
								maxFD = mMaxFD;
							}
						}
					}
					else if(currSockFD == mSocket->mSocketFD) //Check if we have a new connection request
					{
						//On getting a new connection we get a socket descriptor for that socket and then
						//we add that sockfd in the select's watch list.
						newSocketFD = mSocket->acceptConnection();
						if(newSocketFD == ERROR)
						{
							retVal = EXIT_PROGRAM;
							continue;
						}
						else
						{
							FD_SET(newSocketFD, &mActiveSet);
						}

						if(newSocketFD > maxFD)
						{
							nextMaxFD = maxFD;		
							maxFD = newSocketFD;
							mMaxFD = maxFD;
						}
					}
					else
					{
						//This is the section that receives the messages from the connected clients.
						//It also handles the case when the connection is closed by any clients.
						int recvBytes = 0;
						recvBytes = handleRecv(currSockFD);
						if(recvBytes <= 0)
        	                                {
							handleExit(currSockFD);
							if(currSockFD == maxFD)
							{
								maxFD = nextMaxFD;
								mMaxFD = nextMaxFD;
							}
							FD_CLR(currSockFD, &mActiveSet);
                                	        }        
					}
				}
				
			}
		
		}
        }

	cout<<"\n";

}

/*
Input: the string of the register command entered by the user at the prompt.
output: bool value indicating the client was registered or not.
Logic:
1) Validates the register command
2) Check if we are already registered.
3) Sends the register request to the server. 
4) Server on receiving the request replies with the success/failure message 
   along with the peers information.(in case of success)
5) Client on receiving the message, check the list and displays the same.
6) In case of success, sets the "mRegistered" flag to true.
*/

bool client::registerCommand(string command)
{
	if(mRegistered)
	{
		printMessage("\nClient already registered");
		return true;
	}
	//First validate the command
	//cout<<"\n Command:"<<command;
	vector<string> commandList;
	commandList.clear();
	commandList = getCommandList(command, ' ');
	
	//Validate the command list;
	if(commandList.size() != 2)
	{
		printMessage("\nIncorrect Register command");
		printMessage("\nCorrect usage: REGISTER  <server  IP>  <port_no>");
		return false;
	}
	
	//Now validate the IP and the port;
	string serverIP = commandList[0];
	string serverPort = commandList[1];

	if(serverIP == mMyIP || serverIP == mMyName)
	{
		printMessage("Cannot register to self. Please check the server address used in the command");
		printMessage(USE_HELP);
		return false;	
	}
	
	if(!mSocket->validatePort(serverPort))
	{
		return false;
	}

	vector<string> msgVector;
	msgVector.clear();
	msgVector.push_back("REGISTER");
	msgVector.push_back(mMyIP);
	msgVector.push_back(mPort);
	
	//We have the valid IP and the port now, lets send the register command
	string message = createMessage(msgVector);
	
	int sockFD = mSocket->initClientSocket(serverIP, serverPort);
	if(sockFD == ERROR)
	{
		stringstream stream;
		stream<<"Failed to register the peer with the server: "<<serverIP<<" at port: "<<serverPort;
		stream<<"\nThe reasons for this failures could be: ";
		stream<<"\n\t -Server IP/Name provided is wrong";
		stream<<"\n\t -Server port provided in the command is incorrect";
		printMessage(stream.str());
		return false;
	}

	bool done = mSocket->sendMessage(message, sockFD);
	if(!done)
		return false;

	int recvBytes = 0;
	char msgPacket[MAX_PACKET_LEN];
	memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);
	recvBytes = recv(sockFD, msgPacket, MAX_PACKET_LEN, 0);
	if(recvBytes <= 0)
	{
		printMessage("\nRegisteration:: unable to get the server ack");
		printMessage("Registeration Failed");
		return false;
	}
	
	printMessage("\nRegisteration with the server successfull");
	setPeerList(msgPacket);
	
	mRegistered = true;
	
	//Reset the fd_sets to include the newly created sockfd
	FD_SET(sockFD, &mActiveSet);
	if(mMaxFD < sockFD)
		mMaxFD = sockFD;

	mServer.mPort = serverPort;
	mServer.mSockFD = sockFD;
	if(isdigit(serverIP[0]))
	{
		mServer.mIP = serverIP;
		mServer.mName = mSocket->getNameFromIP(serverIP);
	}
	else
	{
		mServer.mName = serverIP;
		mServer.mIP = mSocket->getIPFromName(serverIP);
	}

	listCommand(true);	
	return true;
}


/*
bool connectCommand():
Input: Command string entered by the user
Returns: bool value indicating the success or the failure
Logic:
1) Check if the command entered by the user is valid or not
   if the command is not valid, then we displays an error message.
2) Checks if the client is registerd or not. If not registered, displays an 
   error message for the registeration
3) Checks if the peer requested is in the peer list sent by the server.
4) Checks if the total number of connection is more than 3 or not.
5) Checkes if the peer is already connected.
6) After all validations are done, we send a connect request to the peer
7) On getting the success message from the peer, we store the sock fd of the 
   peer in the peer list.
8) On success we displays the success message and on failure displays the 
   appropriate failiure message.
*/
bool client::connectCommand(string command)
{
	if(command.length() <= 0)
		return false;

	stringstream stream;
	if(!mRegistered)
	{
		stream.clear();
		stream<<NOT_REGISTERED;
		printMessage(stream.str());
		return false;	
	}
	
	int connectedCount = 0;
	for(int j = 0; j<mPeerList.size() ;j++)
	{
		if(mPeerList[j].mSockFD > 0)
		{
			connectedCount++;
		}
	}
	
	if(connectedCount >= 3)
	{
		printMessage("");
		printMessage("Client is already connected with 3 other clients");
		printMessage("Cannot connect to more than 3 peers");
		printMessage("");
		return true;	
	}

	vector<string> tokens = splitMessage(command.c_str(),' ');
	

	if(tokens.size() < 3)	
	{
		stream.clear();
		stream<<"Invalid Connect Command\n";
		stream<<"Correct format: CONNECT <destination> <portno>\n";
		stream<<USE_HELP;
		printMessage(stream.str());
		return false;
	}

	string peerIP = tokens[1];
	string peerPort = tokens[2];

	if(!mSocket->validatePort(peerPort))
	{
		printMessage(INVALID_PORT);
		return false;	
	}
	
	//Check if the client aready connected to this peer
	bool exist = false;
	bool isConnected = false;

	vector<peer>::iterator currPeer;
	for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
	{
		if(currPeer->mIP == peerIP || currPeer->mName == peerIP)
		{
			exist = true;
			if(currPeer->mSockFD > 0)
			{
				isConnected = true;
			}
			break;
		}
	}

	if(isConnected)
	{
		printMessage(ALREADY_CONNECTED);
		return true;
	}

	if(!exist)
	{
		stream.clear();
		stream<<UNKNOWN_CLIENT<<"\n";
		stream<<"Please use the \"LIST\" comnand to check for the registered peers";
		printMessage(stream.str());
		return false;
	}

	//So we have a valid client to connect to	
	vector<string> msgVector;
        msgVector.clear();
        msgVector.push_back("CONNECT");
        msgVector.push_back(mMyIP);
        msgVector.push_back(mPort);	

	string message = createMessage(msgVector);
	int sockFD = mSocket->initClientSocket(peerIP, peerPort);
	if(sockFD == ERROR)
		return false;

	bool done = mSocket->sendMessage(message, sockFD);
	if(!done)
		return false;

	//This is the confirmation of the connect
	int recvBytes = 0;
	char msgPacket[MAX_PACKET_LEN];
	memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);

	recvBytes = recv(sockFD, msgPacket, MAX_PACKET_LEN, 0);
	if(recvBytes <= 0)
	{
		stream.clear();
		stream<<"Connect::Unable to get the ACK of "<<peerIP<<"\n";
		stream<<CONNECT_FAILED;
		printMessage(stream.str());
		return false;
	}

	//So we got the ACK of the peer. So now set the sockFD of the newly connected peer
	if(currPeer != mPeerList.begin() || currPeer != mPeerList.end())
	{
		//Include this new peers sockFD in the active set so that 
		//select() will poll on this sockFD also.
	        FD_SET(sockFD, &mActiveSet);
        	if(mMaxFD < sockFD)
                mMaxFD = sockFD;

		currPeer->mSockFD = sockFD;
		stream.clear();
		stream<<"\nSuccessfully connected to "<<peerIP;
		printMessage(stream.str());
		return true;
	}
	else
		return false;
	
}

/*
void listCommand():
input: None
Returns: None
Logic:
This is function executed when the user enters list command
1) Checks if the client is registered with the server or not
2) Checks if this client is connected with any peer or not.
3) After validations we display the list containing the info 
   of the connected peers.
*/

void client::listCommand()
{
	if(!mRegistered)
	{
		printMessage(NOT_REGISTERED);
		return;
	}

	const char *hostname = mServer.mName.c_str();
	const char *ip_addr = mServer.mIP.c_str();
	int port_num = stringToInt(mServer.mPort);
	
	int host_id = 0;
	host_id++;
	printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);

	for(int i = 0; i < mPeerList.size();i++)
	{
		if(mPeerList[i].mSockFD > 0)
		{
			hostname = mPeerList[i].mName.c_str();
			ip_addr = mPeerList[i].mIP.c_str();
			port_num = stringToInt(mPeerList[i].mPort);
			host_id++;
	
			printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
		}
	} 

}

/*
void listCommand(bool)
Input: bool flag
Returns: None.
Logic:
Logic is similar to the listCommand(), except that it displays the peer list
without checking the connection with the peer , i.e. it displays all the peers
present in the system
*/

void client::listCommand(bool flag)
{
        if(!mRegistered)
        {
                printMessage(NOT_REGISTERED);
                return;
        }

	printMessage("\n");

        const char *hostname = mServer.mName.c_str();
        const char *ip_addr = mServer.mIP.c_str();
        int port_num = stringToInt(mServer.mPort);
        
        int host_id = 0;
        host_id++;
        printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);

        for(int i = 0; i < mPeerList.size();i++)
        {
		hostname = mPeerList[i].mName.c_str();
                ip_addr = mPeerList[i].mIP.c_str();
                port_num = stringToInt(mPeerList[i].mPort);
                host_id++;

                printf("%-5d%-35s%-20s%-8d\n", host_id, hostname, ip_addr, port_num);
        }
	
	printMessage("");
}

/*
void terminateCommand()
Input: String having the command entered by the user.
Returns: None
Logic:
1) Check if the command is valid or not.
2) Check if the client is registered with the server or not
3) Check if are connected to any peer or not.
4) Validate the connection id specified in the input. This 
   connectionId must match the connection id displayed in the
   list command. 
5) After all the validations, close the connection with the specified
   peer using the "close(sockFD)" function.
6) On successful termination, set the sockFD of the peer in the mPeerList to 0
7) Display a message specifying the success/failure of the operation.
*/
void client::terminateCommand(string command)
{
	if(command.length() <= 0)
	{
		printMessage("Invalid TERMINATE command");
		printMessage(USE_HELP);
		return;
	}
	
	stringstream stream;
        vector<string> commandList = getCommandList(command, ' ');
        if(commandList.size() != 1)
        {
                stream<<"\nInvalid TERMINATE command";
                stream<<"\nCorrect format: TERMINATE <connection id>\n";
                stream<<USE_HELP;
                printMessage(stream.str());
                return;
        }

	int connectionID = 0;

	stream.clear();
	stream<<commandList[0];
	stream>>connectionID;

	if(connectionID <= 0)
	{
		printMessage("Invalid TERMINATE command");
		printMessage(USE_HELP);
		return;
	}
	
	if(!mRegistered)
	{
		printMessage("No connection to terminate as the client is still not registered");
		return;
	}

	//Connection id 1 is invalid as 1 is for server
	if(connectionID == 1)
	{
		printMessage("Cannot terminate the connection with the server");
		printMessage("To terminate the connection with server and exit please use \"EXIT\" command");
		return;
	}
	
	if(!validateId(connectionID))
	{
		return;
	}

	if(mPeerList.size() == 0)
	{
		printMessage("Nothing to terminate as no connection to any peer exist");
		return;
	}	
	
	if(mPeerList.size() < connectionID -1)
	{
		printMessage("Invalid connection id used");
		printMessage("Please use the list command to check for the correct connection id");
		return;
	}

	//Map this to the vector mPeerList numbering
	connectionID = connectionID - 2; //As the minimum id for the peer is 2

	vector<peer> connectedPeer;
	int posn = 0;
	for(int i = 0; i < mPeerList.size(); i++)
	{
		if(mPeerList[i].mSockFD > 0)
		{
			connectedPeer.push_back(mPeerList[i]);
		}
	}

	int peerSockFD = connectedPeer[connectionID].mSockFD;

	if(peerSockFD <= 0)
	{
		printMessage("No valid connection exist with the specified peer");
		return;
	}

	//Phewww.... all the validations done. Just terminate the connection now.

	/*
		1)Close the socket FD
		2)Remove the socket FD from the fd_set
		3)Remove the entry from the peer list
		3)Reset the mMaxFD in the main run loop
		4)Reset the maxFD in the main run loop.
	*/
	
	close(peerSockFD);
	FD_CLR(peerSockFD, &mActiveSet);

	for(int i =0 ;i<mPeerList.size();i++)
	{
		if(peerSockFD == mPeerList[i].mSockFD)
		{
			mPeerList[i].mSockFD = 0;
			break;
		}
	}

	int maxFD = mMaxFD;
	mMaxFD = 0;
	for(int i=0;i<=maxFD;i++)
	{
		if(FD_ISSET(i, &mActiveSet))
		{
			if(i > mMaxFD)
			{
				mMaxFD = i;
			}
		}	
	}

	//Print the success message
	printMessage("Successfully terminated the connection with the requested peer");
}
        
void client::exitCommand()
{
	//No need to clean up the connections if we are exiting without registering
	if(!mRegistered)
		return;

	mRegistered = false;
	//close(mServer.mSockFD);
	
	vector<peer>::iterator itr;
	for(itr = mPeerList.begin(); itr != mPeerList.end(); itr++)
	{
		if(itr->mSockFD > 0)
			close(itr->mSockFD);
	}

	close(mServer.mSockFD);
}


void client::uploadCommand(string command)
{
	if(!mRegistered)
	{
		printMessage(NOT_REGISTERED);
		return;
	}

	if(command.length() <= 0)
		return;

	stringstream stream;
	vector<string> commandList = getCommandList(command, ' ');
	if(commandList.size() != 2)
	{
		stream<<"\nInvalid UPLOAD command";
		stream<<"\nCorrect format: UPLOAD <connection id> <file name>>\n";
		stream<<USE_HELP;
		printMessage(stream.str());
		return;
	}
	
	stream.clear();
	stream<<commandList[0];
	int connectionID = 0;
	stream>>connectionID;

	string fileName = commandList[1];

		
	//Now validate the connection id;
	if(!validateId(connectionID))
	{
		return;
	}

	ifstream file(fileName.c_str(), ios::in|ios::binary|ios::ate);
  	if(!file.good())
  	{
    		printMessage("File "+fileName+" does not exist");
		printMessage("Please check the name of the file entered");
		file.close();
    		return;
  	}

	if(file.tellg() <= 0)
	{
		printMessage("File: "+fileName+" is empty");
		return;
	}

	long int length = file.tellg();
	//Message format: UPLOAD:IP:fileName:fileLength;	
	stringstream message;
	message<<"UPLOAD:"<<mMyIP<<":"<<fileName<<":"<<file.tellg();

	file.seekg(0, ios::beg);

	vector<peer> connectedPeer;
        for(int i = 0; i < mPeerList.size(); i++)
        {
                if(mPeerList[i].mSockFD > 0)
                {
                        connectedPeer.push_back(mPeerList[i]);
                }
        }

	connectionID = connectionID - 2;
        int peerSockFD = connectedPeer[connectionID].mSockFD;

	//Start the timer for this upload operation
	clientTimer.startTimer();
	bool done = mSocket->sendMessage(message.str(), peerSockFD);
	if(!done)
	{
		if(LOG_LEVEL == HIGH)
			printMessage("Failed to initiate upload with client");

		return;
	}

	//recv the ack message
        int recvBytes = 0;
        char msgPacket[MAX_PACKET_LEN];
        memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);

        recvBytes = recv(peerSockFD, msgPacket, MAX_PACKET_LEN, 0);
        if(recvBytes <= 0)
        {
                if(LOG_LEVEL)
		{
                	printMessage("Failed to receive the UPLOAD ACK From the client");
		}
                return;
        }

	//So we received the ACK, finally start the file transfer
	done = mSocket->sendFile(fileName, peerSockFD);
	if(!done)
	{
		//Oh no... file transfer failed :(
		printMessage("Failed to send the file " + fileName + "to the peer");
		printMessage("Upload command failed");
		return;
	}

	//The file has been uploaded to the requested peer. Display success message.
	//printMessage("File successfully uploaded to the peer "+mPeerList[connectionID-2].mIP);
	clientTimer.stopTimer();
	uint64_t executionTime = clientTimer.getExecutionTime();
	string peerName = connectedPeer[connectionID].mName;
	
	long int transferRate = 0;
	transferRate = (length*8)/executionTime;

	stringstream successMsg;
	successMsg<<"Tx:"<<mMyName<<" -> "<<peerName<<",";
	successMsg<<" File Size: "<<length<<" Bytes, Time Taken: "<< executionTime<<" "<<clientTimer.timeUnit<<", ";
	successMsg<<"Tx Rate: "<<transferRate<<" bits/"<<clientTimer.timeUnit;
	printMessage(successMsg.str());

	//Now create an entry in the stats for this operation
	if(mClientStats.find(peerName) == mClientStats.end())
	{
		stats newStats;
		newStats.mUploadCount++;
		newStats.mUploadSpeed += transferRate;
		newStats.mPeerName = peerName;
		mClientStats[peerName] = newStats;
	}
	else
	{
		mClientStats[peerName].mUploadCount++;
		mClientStats[peerName].mUploadSpeed += transferRate;
	}
}

/*
void downloadCommand(string)
Input: command entered by the user.
Returns:None
Logic: 
1) validate the format of command
2) Check if the client is registerd with the server or not.
3) Check the validity of the connection id entered by the user. Also check if they
   are connected or not
4) Check if more than three files/ connection id specified in the command.
5) After completing all the validations perform a 3-way handshake with the respective peer.
   This funtions performs the handshake with all the valid connection ids.
6) If the peer replies that the file requested is not valid, then display the error message 
   for that file.
7) Add all the valid files that we will be downloading in the mDownloadMap.
8) Set the mDownloadFlag to true. This indicates that the download command is in progress.
9) We will be downloading the files parallely using the select().
*/ 
void client::downloadCommand(string command)
{
	mDownloadFlag = false;
	mDownloadMap.clear();
	
	if(command.length()<=0)
	{
		printMessage("Invalid download command");
		return;
	}

	if(!mRegistered)
	{
		printMessage(NOT_REGISTERED);
		return;
	}	

	stringstream stream;
        vector<string> commandList = getCommandList(command, ' ');
        if(commandList.size() == 0 || commandList.size() > 6 || commandList.size()%2 != 0)
        {
                stream<<"\nInvalid DOWNLOAD command";
                stream<<"\nCorrect format:"<<VALID_DOWNLOAD<<"\n";
                stream<<USE_HELP;
                printMessage(stream.str());
                return;
        }

	//So now check the individual parts of the command
	vector<int> connectionID;
	vector<string> fileName;
	
	int connID;
	int maxID = 0;
	for(int i = 0; i<commandList.size();i++)
	{
		if(i % 2 == 0)
		{
			stream.clear();
			connID = 0;
			stream<<commandList[i];
			stream>>connID;
			if(!validateId(connID))
			{
				printMessage("Connection id "+commandList[i]+" is invalid");
				printMessage(USE_HELP);
				return;
			}
			connectionID.push_back(connID);
			if(connID > maxID)
				maxID = connID;
		}
		else
		{
			fileName.push_back(commandList[i]);
		}
	}

        vector<peer> connectedPeer;
        int posn = 0;
        for(int i = 0; i < mPeerList.size(); i++)
        {
                if(mPeerList[i].mSockFD > 0)
                {
                        connectedPeer.push_back(mPeerList[i]);
                }
        }

	//So now we have the connection id and the file name mapping.
	if(connectionID.size() != fileName.size())
	{
		printMessage("Invalid DOWNLOAD command");
		printMessage(VALID_DOWNLOAD);
		return;
	}

	
	//Start the download of the file one by one
	int peerSockFD = 0;
	string currFile = "";
	for(int i = 0; i < connectionID.size();i++)
	{
		peerSockFD = connectedPeer[connectionID[i]-2].mSockFD;
		currFile = fileName[i];
		if(downloadFile(currFile, peerSockFD))
		{
			mDownloadFlag = true;	
		}
	}
}

/*
void statCommand():
This function provides the functionality of the statistics command 
Checks if the client is registed or not. If not returns an error message.
Checks if we have any stats to be shown on this client.
After all validations, it displays the list in the format described in the PA1 document.
*/

void client::statCommand()
{
	if(!mRegistered)
        {
                printMessage(NOT_REGISTERED);
                return;
        }

	if(mClientStats.size() <= 0)
	{
		printMessage("No Upload/Download operation has been performed on this client");
		return;
	}
	
	//So we have some stats to print.
	printMessage("");
	map<string, stats>::iterator itr;
	printf("%-35s%-20s%-35s%-20s%-20s\n", "Host Name", "Total Uploads", "Average upload speed(bits/ms)", "Total downloads", "Average download speed (bits/ms)");
	fflush(stdout);
	for(itr = mClientStats.begin(); itr!=mClientStats.end(); itr++)
	{
		printf("%-35s%-20ld%-35ld%-20ld%-20ld\n", itr->second.mPeerName.c_str(), itr->second.mUploadCount, itr->second.getAvgUploadSpeed(), itr->second.mDownloadCount, itr->second.getAvgDownloadSpeed());
		fflush(stdout);
	}
}

/*
This is a handler function which handles the peer information sent by the server
in case a new peer registers with the server.

This is to update the peer list of the current client
Here we dont need the sockFD. This sockFD will be set when
this client will explicitly call connect. 
After updating the peer list we will display the updated list.

*/

void client::handleRegister(char *msg, int sockFD)
{
	//This is to update the peer list of the current client
	//Here we dont need the sockFD. This sockFD will be set when
	//this client will explicitly call connect.
	vector<string> tokens = splitMessage(msg, ':');
	
	if(tokens.size() < 3)
	{
		cout<<"\nIncomplete registeration message on client"<<endl;
		return;
	}	

	string peerIP = tokens[1];
	string peerPort = tokens[2];

	peer tempPeer;

	tempPeer.mPort = tokens[2];
        if(isdigit(peerIP[0]))
        {
                tempPeer.mIP = peerIP;
                tempPeer.mName = mSocket->getNameFromIP(peerIP);
        }
        else
        {
                tempPeer.mName = peerIP;
                tempPeer.mIP = mSocket->getIPFromName(peerIP);
        }

	mPeerList.push_back(tempPeer);

	//Display the updated peer list on this client
	printMessage("");
        int id = 0;
        printf("%-5d%-35s%-20s%-8s\n", ++id, mServer.mName.c_str(), mServer.mIP.c_str(), mServer.mPort.c_str());

        for(int i = 0; i < mPeerList.size();i++)
        {
        	printf("%-5d%-35s%-20s%-8s\n", ++id, mPeerList[i].mName.c_str(), mPeerList[i].mIP.c_str(), mPeerList[i].mPort.c_str());
        }
	fflush(stdin);

}

/*
int handleRecv(int recvSockFD)
Input: clients socket fd from which we are receiving.
Returns: An int stating whether we have received valid or invalid data.

Logic: This is the main function which is called from run() when the select() finds that we 
have received a data on any connection.
1) It validates the received packet. 
2) After validation, it calls "checkMessage()" to investigate what the message means. 
   We will do this only if the mDownloadFlag is false (i.e. there is no download in progress)
3) If the download flag is set, that mean we are receiving data from a peer. We will send this 
   received data to the checkDownloadStatus() function to handle the download.
4) If the download flag is not setm then we call the checkMessage() and calls the appropriate handler
   method.
*/

int client::handleRecv(int recvSockFD)
{

        int recvBytes = 1;
        char msgPacket[MAX_PACKET_LEN];
	memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);
        /*while(recvBytes > 0)
        {
                memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);
                recvBytes = recv(recvSockFD, msgPacket, MAX_PACKET_LEN, 0);
                cout<<msgPacket<<endl;
        }*/
        recvBytes = recv(recvSockFD, msgPacket, MAX_PACKET_LEN, 0);

        if(recvBytes <= 0)
	{
		if(mDownloadMap.find(recvSockFD) != mDownloadMap.end())
		{
			if(mDownloadMap[recvSockFD]->file.is_open())
			{
				mDownloadMap[recvSockFD]->file.close();
			}

			delete mDownloadMap[recvSockFD];
			mDownloadMap.erase(recvSockFD);
		}
                return recvBytes;
	}

	if(mDownloadFlag)
	{
		checkDownloadStatus(recvSockFD, msgPacket, recvBytes);	
	}

        char msg = checkMessage(msgPacket);

	clientTimer.reset();
        switch(msg)
        {
                case CONNECT:
                        handleConnect(msgPacket, recvSockFD);
                        break;

		case REGISTER:
			handleRegister(msgPacket, 0);
			break;	

		case UPLOAD:
			handleUpload(msgPacket, recvSockFD);
			break;
		case DOWNLOAD:
			handleDownload(msgPacket, recvSockFD);
			break;
		case EXIT:
			handleExit(msgPacket);
			break;
		case SENDSTATS:
			handleStats(msgPacket);
			break;
                default:
                        break;
        }

	return recvBytes;
}

/*
Handles the "connect" request sent by the peer.
It checks if the requester is a valid peer or not.
Then it sets this sockFD in the mPeerList and replies with a success to 
the requester
*/

void client::handleConnect(char *message, int peerSockFD)
{
	if(!message || strlen(message) <= 0)
		return;

	vector<string> tokens = splitMessage(message, ':');
	if(tokens.size() < 3)
		return;

	string peerIP = tokens[1];
	string peerPort = tokens[2];
	
	//Check if this IP is listed in the peer list
	//If its there then set the sockFD of the client there
	bool isConnected = false;
	bool exist = false;
	vector<peer>::iterator currPeer;
	for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
	{
		if(currPeer->mIP == peerIP || currPeer->mName == peerIP)
		{
			exist = true;
			if(currPeer->mSockFD > 0)
			{
				isConnected = true;
			}
			break;
		}
	}

	if(isConnected && (mPeerList.size() > 0))
	{
		return;
	}	

	if(!exist && (mPeerList.size() > 0))
		return;

	//Now everything has been validated. Time to send the ACK
	string reply = ACK;
	bool sent = mSocket->sendMessage(reply, peerSockFD);
	if(!sent)
		return; 

	currPeer->mSockFD = peerSockFD;
	stringstream stream;
	stream<<"Peer "<<peerIP<<" successfully connected";
	printMessage(stream.str());
}

/*
This function handles when any peer exits from the system.
*/
void client::handleExit(int currSockFD)
{
	if(currSockFD <= 0)
		return;

	//Fail safe check to check if server has closed the system	
        stringstream stream;
        stream.clear();

	if(mServer.mSockFD == currSockFD)
	{
		stream<<"Server "<<mServer.mIP<<" has exited the system\n";
		stream<<"Deregistering this peer from the system";
		stream<<"\nTo enable the peer, please start the server and then register";
		stream<<"\nthis peer to the server again.";
		printMessage(stream.str());
		exitCommand();
		exit(0);
	}

        vector<peer>::iterator currPeer;
        for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
        {
                if(currPeer->mSockFD == currSockFD)
                {
			stream.clear();
                        stream<<"Peer "<<currPeer->mIP<<" has closed this system";
                        stream<<"\nClosing the connection from this peer";
                        printMessage(stream.str());
			currPeer->mSockFD = 0;
                        break;
                }
        }
	
}

/*
This function handles the message sent by the server in case any of the peer exits the system.
On receiving this message we will display a message that the peer has exited the systems
and then we display the peers present in the system.
*/
void client::handleExit(char *msg)
{

	if(!msg || strlen(msg) <= 0)
		return;

	//Format of the message received : EXIT:<PEER_IP>
	vector<string> tokens = splitMessage(msg, ':');	
	if(tokens.size() != 2)
		return;

	string peerIP = tokens[1];

	//Check if this peer is in the list.
	vector<peer>::iterator itr;
	
	int currSockFD = 0;
	for(itr = mPeerList.begin(); itr != mPeerList.end(); itr++)
	{
		if(itr->mIP == peerIP || itr->mName == peerIP)
		{
			currSockFD = itr->mSockFD;
			mPeerList.erase(itr);
			break;
		}
	}

	
	printMessage(peerIP + " has exited the system");

	listCommand(true);
}


/*
void handleUpload():
This method is called in case some peers wants to upload data on me.
1) We will first create a file with the same name as the one being uploaded by the peer.
2) On successfully creating the file, we send the go-ahead message to the peer.
3) Then we receive the file in the chunks till all the data is received.
4) On receiving all the data just close the file and stop the timer.
5) Display the statistics as specified in the PA1 document and record the stats in the 
  mClientStats map.

*/
void client::handleUpload(char *message, int peerSockFD)
{
	//Doubt: Do we need to validate the file at this peer
	if(!message || strlen(message) <= 0)
		return;

	//Format of the message received-> UPLOAD:<PEER_IP>:<FILE_NAME>:<FILE_LENGTH>
        vector<string> tokens = splitMessage(message, ':');
        printMessage(message);

        if(tokens.size() != 4)
        {
                cout<<"\n incomplete registeration message on client"<<endl;
                return;
        }

	//We need this ip for the statistics
        string peerIP = tokens[1];
        string fileName = extractFileName(tokens[2]);
	long int fileSize = stringToInt(tokens[3]);

	vector<peer>::iterator itr;
	for(itr = mPeerList.begin(); itr!=mPeerList.end();itr++)
	{
		if(itr->mIP == peerIP || itr->mName == peerIP)
		{
			peerSockFD = itr->mSockFD;
			break;
		}
	
	}	

	if(peerSockFD == 0)	
		return;

        //Start the timer here 
        clientTimer.startTimer();

	string peerName = itr->mName;

	string replyAck("ACK");
	bool done = mSocket->sendMessage(replyAck, peerSockFD);
	if(!done)
		return;

	done = mSocket->recvFile(fileName, fileSize, peerSockFD);
	if(!done)
	{
		printMessage("Failed to receive file: "+fileName+" from: "+peerIP);
		return;
	}
	
	//File has been downloaded successfully, stop the timer.
	clientTimer.stopTimer(); 
	
	uint64_t executionTime = clientTimer.getExecutionTime();
	long int transferRate = (fileSize*8)/executionTime;

	//Update the Download speed and the Download count for this peer.
        if(mClientStats.find(peerName) == mClientStats.end())
        {
                stats newStats;
		newStats.mPeerName = peerName;
		newStats.mDownloadCount++;
		newStats.mDownloadSpeed += transferRate;
		mClientStats[peerName] = newStats;
        }
        else
        {
                mClientStats[peerName].mDownloadCount++;
                mClientStats[peerName].mDownloadSpeed += transferRate;
        }

	//Display the success message
	stringstream successMsg;
        successMsg<<"Rx:"<<mMyName<<" <-  "<<peerName<<",";
        successMsg<<" File Size: "<<fileSize<<" Bytes, Time Taken: "<< executionTime<<" "<<clientTimer.timeUnit<<", ";
        successMsg<<"Rx Rate: "<<transferRate<<" bits/"<<clientTimer.timeUnit;
        printMessage(successMsg.str());
}	

/*
Handler method if the peer is requesting to download any file from me
1) Validate the file name requested. If invalid send and INVALID message to peer.
2) If valid, sends an ACK to peer.
3) Peer replies with the GOAHEAD message
4) On receiving the GOAHEAD message, it start the transfer of the file to this peer.
5) On finishing the file transfer, it stops the timer and notes the statistics for this
   file transfer operation.
*/
void client::handleDownload(char *message, int peerSockFD)
{
	//Sanity check
	if(!message || strlen(message) <= 0)
		return;

	clientTimer.reset();
	clientTimer.startTimer();
        vector<string> tokens = splitMessage(message, ':');
        if(tokens.size() != 3)
                return;

        //We need this ip for the statistics
        string peerIP = tokens[1];
        string fileName = tokens[2];

	ifstream file(fileName.c_str(), ios::in|ios::binary|ios::ate);
	if(!file.good())
	{
		printMessage("File: "+fileName+" requested by "+peerIP+" does not exist");
		mSocket->sendMessage(INVALID_FILE, peerSockFD);
		file.close();
		return;
	}

	//Get the size of the file.
	int size = file.tellg();
	file.close();

	string replyMessage = "";
	stringstream stream;

	if(size == 0)
	{
		replyMessage = INVALID_FILE;
	}
	else
	{
		stream.clear();
		stream<<ACK<<":"<<size;
		replyMessage = stream.str();
	}

        bool done = mSocket->sendMessage(replyMessage, peerSockFD);
	if(size == 0)
	{
		return;
	}

	if(!done)
	{
		if(LOG_LEVEL == HIGH)
		{
			printMessage("Failed to send the ACK message for the download request");
			return;
		}
	}

	char buff[20];
	memset(buff, 0, sizeof(char)*20);
	int recvBytes =recv(peerSockFD, buff, 20, 0);
	if(recvBytes <= 0)
	{
		printMessage("Failed to establish 3-way handshake");
		printMessage("Aborting file transfer");
		return;
	}

	if(strcmp(buff, "STOP") == 0)
	{
		printMessage("Failed on the 3rd message for 3-way handshake");
		printMessage("Requested failed to create file");
		printMessage("Aborting the file transfer");
		return;
	}

	//So the connection is well setup, now start the file transfer.
	printMessage("Starting transfer of file "+fileName+" to the peer: "+peerIP);
	done = mSocket->sendFile(fileName, peerSockFD);
	if(!done)
	{
		printMessage("Unable to send the file "+fileName+" to the peer "+peerIP);
		return;
	}		

	//Done with the file transfe, Stop the timer.
	clientTimer.stopTimer();
	uint64_t executionTime = clientTimer.getExecutionTime();
	long int transferRate = (size*8)/executionTime;

	//Get the name of the peer
        int idx = 0;
        for(idx = 0; idx < mPeerList.size();idx++)
        {
                if(mPeerList[idx].mSockFD == peerSockFD)
			break;
        }
        string peerName = mPeerList[idx].mName;

        //Time to record the stats for this file download operation
	//Since we are uploading this file to the other peer, we are changing the upload stats
        if(mClientStats.find(peerName) == mClientStats.end())
        {
                stats newStats;
                newStats.mPeerName = peerName;
                newStats.mUploadCount++;
                newStats.mUploadSpeed += transferRate;
                mClientStats[peerName] = newStats;
        }
        else
        {
                mClientStats[peerName].mUploadCount++;
                mClientStats[peerName].mUploadSpeed += transferRate;
        }
	
        stringstream successMsg;
        successMsg<<"Tx:"<<mMyName<<" ->  "<<peerName<<",";
        successMsg<<" File Size: "<<size<<" Bytes, Time Taken: "<< executionTime<<" "<<clientTimer.timeUnit<<", ";
        successMsg<<"Tx Rate: "<<transferRate<<" bits/"<<clientTimer.timeUnit;
        printMessage(successMsg.str());

}

/*
Logic: 
This function performs the start of the download from the specified peer.
we perform the 3-way handshake here. If the handshake is successful, then we
add the information in the mDownloadMap and set the mDownloadFlag (indicator
of download in progress) as true
*/

bool client::downloadFile(string& fileName, int peerSockFD)
{
	if(fileName.length() <= 0 || peerSockFD <= 0)
		return false;

	//Start the timer here
	clientTimer.reset();
	clientTimer.startTimer();

	bool done = false;
	//Format of the message = DOWNLOAD:IP:<FILE_NAME>
	string message =  "DOWNLOAD:" +mMyIP+":"+fileName;
	done = mSocket->sendMessage(message, peerSockFD);
	if(!done)
	{
		return false;
	}
	
        int recvBytes = 0;
        char msgPacket[MAX_PACKET_LEN];
        memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);

        recvBytes = recv(peerSockFD, msgPacket, MAX_PACKET_LEN, 0);
	if(recvBytes <= 0)
	{
		return false;
	}

	vector<string> reply = splitMessage(msgPacket, ':');
	if(strcmp(reply[0].c_str(), ACK) != 0)
	{
		printMessage("Download for the file "+fileName+" failed");
		printMessage("Please check the name of the file");
		return false;
	}

	int fileSize = 0;
	stringstream stream;
	stream<<reply[1];
	stream>>fileSize;

	string origFileName = extractFileName(fileName);
	fileInfo *tempFile = new fileInfo();
	tempFile->file.open(origFileName.c_str(), ios::out);
	if(!tempFile->file.is_open())
	{
		printMessage("Failed to create file "+origFileName+" on this machine");
		printMessage("Download for the file "+origFileName+" failed");
		mSocket->sendMessage("STOP", peerSockFD);
		delete tempFile;
		return false;
	}

	//3-way handshake as the TCP was misbehaving :(	
	mSocket->sendMessage("START", peerSockFD);

	tempFile->fileTimer.startTime = clientTimer.startTime;
	tempFile->fileName = origFileName;
	tempFile->fileSize = fileSize;
	tempFile->origFileSize = fileSize;
	mDownloadMap[peerSockFD] = tempFile;

	return true;
}

/*
Does exactly what the name says. It broadcasts the requested message to all the 
connected peers.
*/
bool client::broadCast(char *msg)
{
	if(!msg || strlen(msg) <= 0)
		return false;

	if(!mRegistered)
		return true;

	bool done = false;
	done = mSocket->sendMessage(msg, mServer.mSockFD);
	
	vector<peer>::iterator currPeer;
	for(currPeer = mPeerList.begin(); currPeer != mPeerList.end(); currPeer++)
	{
		if(currPeer->mSockFD > 0)
		{
			done = mSocket->sendMessage(msg, currPeer->mSockFD);
		}
	}	
	
	return done;
}

/*
It unpacks the message and extracts peers data from the message.
Then it sets the peer in the list.
*/

bool client::setPeerList(char *msg)
{
	if(!msg || strlen(msg) <= 1)
		return false;

	vector<string> tokens = splitMessage(msg, PEER_SEPERATOR);
	if(tokens.size() <= 0)
		return false;
	
	string reply = "";

	//For this reply the first token will always be an "ACK" or Other reply.
	if(tokens.size() >= 1)
	{
		reply = tokens[0];
		//This is the case when i am the first client in the system
		if(reply != ACK)
			return false;
	}

	vector<string> tempPeerList;
	peer tempPeer;
	for(int i=1;i<tokens.size();i++)
	{
		tempPeerList.clear();
		tempPeerList = splitMessage(tokens[i].c_str(), ':');
		if(tempPeerList.size() < 3)
		{
			//Malformed message. Don't process
			return false;
		}
		else
		{
			tempPeer.mIP = tempPeerList[0];
			tempPeer.mName = tempPeerList[1];
			tempPeer.mPort = tempPeerList[2];	
		
			mPeerList.push_back(tempPeer);
		}	
	}

	return true;
}

/*
Validation function that validates the connection id entered by the users in 
the commands at the shell prompt.
*/

bool client::validateId(int connId)
{

	if(connId == 1)
	{
		printMessage("Cannot Upload/download file to the server");
		printMessage(USE_HELP);
		return false;
	}

	if(mPeerList.size() == 0)
	{
		printMessage("Invalid id used as there is no other peer in the system");
		return false;
	}

	connId = connId - 2; //As the first one is always the server
	
	if(mPeerList.size() < connId)
	{
		printMessage("Invalid Id used in the command");
		printMessage("Please use the list command to check the valid connection id's");
		return false;
	}

	//Check that there exist a valid connection between me and the requested peer
        vector<peer> connectedPeer;
        int posn = 0;
        for(int i = 0; i < mPeerList.size(); i++)
        {
                if(mPeerList[i].mSockFD > 0)
                {
                        connectedPeer.push_back(mPeerList[i]);
                }
        }

	if(connId >= connectedPeer.size())
	{
		printMessage("Invalid Id used in the command");
		printMessage("Please use the list command to check the valid connection id's");
		return false;
	}

	if(connectedPeer[connId].mSockFD <= 0)
	{
		printMessage("There exist no valid connection with the requested peer");
		printMessage("Please use the \"CONNECT\" command to establish a valid connection");
		printMessage(USE_HELP);
		return false;
	}

	return true;
}

/*
This is a handler method called when the statistics command is executed on the server.
Here we are just preparing a message with our stats and then we send it to the server.
*/

void client::handleStats(char *msg)
{

	//As we are handling this message only from the server, no need to take the recvSokFD
	int sockFD = mServer.mSockFD;
	
	string reply = "ACK-";
	stringstream stream;
	stream.clear();

	bool done = false;

	//Check the case if we dont have anything to update,
	if(mClientStats.size() <= 0)
	{
		done = mSocket->sendMessage(reply, sockFD);
		return;
	}
	
	stream<<"ACK";
	//So we now have to serialize and send the stats of this server.
	//FORMAT of the message: "ACK-<peer_name>:<uploadspeed>:<uploadcount>:<
	map<string, stats>::iterator itr;
	for(itr = mClientStats.begin(); itr != mClientStats.end(); itr++)
	{
		stream<<PEER_SEPERATOR<<itr->first<<SEPERATOR<<itr->second.mUploadSpeed<<SEPERATOR<<itr->second.mUploadCount;
		stream<<SEPERATOR<<itr->second.mDownloadSpeed<<SEPERATOR<<itr->second.mDownloadCount;
	}
	
	done = mSocket->sendMessage(stream.str(), sockFD);
	if(!done)
	{
		if(LOG_LEVEL == HIGH)
			printMessage("client.handleStats(), failed to send the stats to the server ");
	}
}

/*
This function has the functionality to handle the parralel downloads.
It maps the data received to the relevent peer and writes the data into the file.
After writing, it updates the file data remaining. 
Then it checks if we have received all the data for this file. If all the data has been downloaded
we close this file and removes it from the mDownloadMap. 
In the end we check if the mDownloadMap is empty. If its empty then we are done with downloading the 
files. So we set the mDownloadFlag to false indicating that the download operation is over.
*/
void client::checkDownloadStatus(int recvSockFD, char *packet, int recvBytes)
{

	map<int, fileInfo *>::iterator itr;
        map<int, fileInfo *>::iterator currItr;

	bool found = false;
	for(itr = mDownloadMap.begin(); itr != mDownloadMap.end(); itr++)
	{
		if(itr->first == recvSockFD)
		{
			found  = true;
			fileInfo *tempObj = itr->second;
			tempObj->file.write(packet, recvBytes);
			tempObj->fileSize -= recvBytes;
			if(tempObj->fileSize == 0)
			{
				tempObj->fileTimer.stopTimer();
				string peerName = "";
				for(int i = 0; i<mPeerList.size(); i++)
				{
					if(mPeerList[i].mSockFD == recvSockFD)
						peerName = mPeerList[i].mName;
				}
				
				long int fileSize = tempObj->origFileSize;
			        uint64_t executionTime = tempObj->fileTimer.getExecutionTime();
			        long int transferRate = 0;
			        transferRate = (fileSize*8)/executionTime;

        			stringstream successMsg;
			        successMsg<<"Rx:"<<mMyName<<" <-  "<<peerName<<",";
			        successMsg<<" File Size: "<<fileSize<<" Bytes, Time Taken: "<< executionTime<<" "<<"millisecond, ";
			        successMsg<<"Rx Rate: "<<transferRate<<" bits/millisecond";
			        printMessage(successMsg.str());
			        //get the name of this peer.
        
			        //Time to record the stats for this file download operation
			        if(mClientStats.find(peerName) == mClientStats.end())
        			{
			                stats newStats;
			                newStats.mPeerName = peerName;
			                newStats.mDownloadCount++;
			                newStats.mDownloadSpeed += transferRate;
			                mClientStats[peerName] = newStats;
			        }
			        else
			        {
			                mClientStats[peerName].mDownloadCount++;
			                mClientStats[peerName].mDownloadSpeed += transferRate;
			        }

				tempObj->file.close();
				delete tempObj;
				mDownloadMap.erase(itr);
				if(mDownloadMap.size() == 0)
				{
					break;
				}
			}	
		}
	}

	if(mDownloadMap.size() == 0)
	{
		mDownloadFlag = false;
	}
}

client::~client()
{
	if(mWindow)
	{
		delete mWindow;
	}	
}

