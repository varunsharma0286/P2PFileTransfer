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
   File Name: "socketClass.cpp"
   Description:
   This file has implementation of all the methods declared in the
   socketClass.h header file.
   This class provides the functionality of the sockets like socket creation,
   socket closing, send() and recv().
   We can consider it to be the socket layer of the program
 */



#include "../include/socketClass.h"

/*
Reference:
The code for creating the server and clients sockets has been referenced from the beej's guide.
*/




//Default Constructor.
socketClass::socketClass()
{
	mSocketFD = -1;	
	//mClientIP = new string("");
	memset(mClientIP, 0, sizeof(char)*MAX_IP_LEN);
}


//Parameterized constructor.
socketClass::socketClass(const string mode)
{
	mSocketFD = -1;
	mMode.clear();
	mMode = mode;
        memset(mClientIP, 0, sizeof(char)*MAX_IP_LEN);
}

/*
Displays the ip address of the local machine
Right now it displays only the IPv4 address
Can add to display both teh v4 and v6 
*/
string socketClass::getMyIP()
{

	string myIP = "";
	int retVal = 0;
	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof(struct addrinfo));

	int err = 0;
	string ip = "8.8.8.8";
	string port = "53";
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        hints.ai_protocol = IPPROTO_TCP;

	retVal = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
	if(retVal != 0)
	{
		if(LOG_LEVEL == HIGH)
			cout<<"\nerror in getMyIP().getaddrinfo() "<<gai_strerror(retVal);
		return "";
	}
	
	struct addrinfo *tempNode; //Iterator for the list pointed to by servinfo
	tempNode = servinfo;
	bool done = false;

	char buffer[128];
	int socketFD;
	while(tempNode)
	{
		socketFD = socket(tempNode->ai_family, tempNode->ai_socktype, tempNode->ai_protocol);
		if(socketFD == ERROR)
		{
			tempNode = tempNode->ai_next;
			continue;
		}

                if (connect(socketFD, tempNode->ai_addr, tempNode->ai_addrlen) == -1) {
                        close(socketFD);
			if(LOG_LEVEL == HIGH)
	                        cout<<"\n Error in the sendMessage().connect()"<<endl;

                        tempNode = tempNode->ai_next;
                        continue;
                }

	    	sockaddr_in name;
    		socklen_t namelen = sizeof(name);
    		err = getsockname(socketFD, (sockaddr*) &name, &namelen);

    		const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 128);
		myIP = p;
                done = true;
                break;
	}
	return myIP;
}

//Function to send the "msg" at the specified "ip" and "port"
bool socketClass::sendMessage(const string& msg, string ip, string port)
{
	if(msg.length() <= 0 || ip.length()<=0 || port.length()<=0)
	{
		return false;
	}

	int sockFD = 0;
	if(mSocketList.end() != mSocketList.find(ip+port))
	{
		//We already have a socket open for this connection.
		sockFD = mSocketList[ip+port];
	}	
	else
	{
		//New connection. Open a new socket for the same.
		sockFD = initClientSocket(ip, port);
		if(sockFD <= 0)
		{
			cout<<"\n Error creating socket for the ip: "<<ip;
			return false;
		}
		
		mSocketList[ip+port] = sockFD;
	}
	//Step 5:Now send the actual message using send(). Check for the appropriate flags at MAN page	
	int sentLen = send(sockFD, msg.c_str(), strlen(msg.c_str()), 0);
	cout<<"\n msg sent: "<<msg;
	if(sentLen <= 0)
	{
		cout<<"\n Error while sending the message: "<<msg;
		close(sockFD);
		return false;
	}
	else if(sentLen < msg.length())
	{
		cout<<"\n full data not sent";
    	}
	
	cout<<"\n msg sent2: "<<msg<<endl;
	//Should not close the sockFD here
    	//close(sockFD);
    	return true;
}


//Overloaded sendMessage()
//Just sends the message specified by "msg", to the speicified sockFD.
bool socketClass::sendMessage(const string& msg, int sockFD)
{
	int sentLen = send(sockFD, msg.c_str(), msg.length(), 0);
	if(sentLen <= 0)
	{
		cout<<"\n Error while sending the message: "<<endl;
		return false;
	}
	
	if(sentLen < msg.length())
	{
		cout<<"\n incomplete data sent"<<endl;
	}
	
	return true;
}


bool socketClass::initSocket(string port, string ip)
{
	bool retVal = false;
	//stringstream tempObj;
	//tempObj<<port;
	//string strPort;
	//tempObj>>strPort;
	
	if(mMode == CLIENT)
	{
		retVal = initClientSocket(port, ip);
	}
	else if(mMode == SERVER)
	{
		retVal = initServerSocket(port);	
	}
	return true;	
}


//Function to initiate a client socket.
//Ordering of the calls
//getaddrinfo() -> socket() -> connect()

int socketClass::initClientSocket(string ip, string port)
{
	if(port.length() <= 0)
	{
		cout<<"\n socketClass::initClient()->port is empty"<<endl;
		return ERROR;
	}
	else if(ip.length() <= 0)
	{
		cout<<"\n socketClass::initClient()->ip is empty"<<endl;
		return ERROR;
	}
	
	int retVal = 0;
	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof(struct addrinfo));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        hints.ai_protocol = IPPROTO_TCP;

	retVal = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
	if(retVal != 0)
	{
		if(LOG_LEVEL == HIGH)
			cout<<"\n error in initClient.getaddrinfo() "<<gai_strerror(retVal);
		return ERROR;
	}
	
	struct addrinfo *tempNode; //Iterator for the list pointed to by servinfo
	tempNode = servinfo;
	bool done = false;

	int socketFD;
	while(tempNode)
	{
		socketFD = socket(tempNode->ai_family, tempNode->ai_socktype, tempNode->ai_protocol);
		if(socketFD == ERROR)
		{
			tempNode = tempNode->ai_next;
			continue;
		}

                if (connect(socketFD, tempNode->ai_addr, tempNode->ai_addrlen) == -1) {
                        close(socketFD);
			if(LOG_LEVEL == HIGH)
	                        cout<<"\n Error in the sendMessage().connect()"<<endl;

                        tempNode = tempNode->ai_next;
                        continue;
                }

                done = true;
                break;
	}

        if(!done)
        {
                cout<<"\nFailed to connect to "<<ip<<endl;
                return ERROR;
        }

	return socketFD;
}

/*
Function to create a listening socket specified by the "port"
Order for the calls here.
getaddrinfo() -> socket() ->setsocketopt() -> bind() -> listen().
accept() will be called inside the run() of client/server.
*/
bool socketClass::initServerSocket(string port)
{
	int retVal = -1;
	
	//Step1: Call getaddrinfo() to get the list of the available connections on this host.
	struct addrinfo hints, *servinfo;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	retVal = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);
	if(retVal != 0)
	{
		if(LOG_LEVEL == HIGH)
			cout<<"\n error in initServer.getaddrinfo() "<<gai_strerror(retVal);
		return false;
	}

	struct addrinfo *tempNode = servinfo;
	bool done = false;
	while(tempNode != NULL)
	{
		//Step2: Create a socket on the specified port using socket();
		mSocketFD = socket(tempNode->ai_family, tempNode->ai_socktype, tempNode->ai_protocol);
		if(mSocketFD == ERROR)
		{
			tempNode = tempNode->ai_next;
			continue;
		}

		//Step3: Use the setsocketopt() to make sure that we can avail this port
		int option_value = 1;
		retVal = setsockopt(mSocketFD, SOL_SOCKET, SO_REUSEADDR, &option_value,sizeof(int));
		if(retVal == ERROR)
		{
			if(LOG_LEVEL == HIGH)
				cout<<"\n error in startServer.setsocketopt "<<endl;
			mSocketFD = -1;
			return false;
		}
	
		//Step4: Bind the created socket to the port using the bind()
		retVal = bind(mSocketFD, tempNode->ai_addr, tempNode->ai_addrlen);
		if(retVal == ERROR)
		{
			close(mSocketFD);
			mSocketFD = -1;
			tempNode = tempNode->ai_next;
			continue;
		}

		//If here then we have created a socket and have bind the socket to the specified port;
		done = true;
		break;	
	}

	if(!done)
	{
		if(LOG_LEVEL == HIGH)
			cout<<"\n startServer():: Failed to create the server socket of the port: "<<port;
		return false;
	}

	freeaddrinfo(servinfo);

	retVal = listen(mSocketFD, BACKLOG);
	if(retVal == ERROR)
	{
		if(LOG_LEVEL == HIGH)
			cout<<"\n Error: startServer()::listen(): Failed to call using the backlog: "<<BACKLOG;
		close(mSocketFD);
		return false;
	}	

	return true;
}

//Wrapper for the accept() call.
int socketClass::acceptConnection()
{
	struct sockaddr_storage clientAddr;
	socklen_t sin_size;
	sin_size = sizeof(clientAddr);
	int clientSockFD  = -1;

	char recvBuff[512];
	//mClientIP->clear();
        memset(mClientIP, 0, sizeof(char)*MAX_IP_LEN);

	clientSockFD = accept(mSocketFD, (struct sockaddr *)&clientAddr, &sin_size);
	
	if(clientSockFD != ERROR)
	{
		char clientIP[INET6_ADDRSTRLEN];
		memset(clientIP, 0, sizeof(char)*INET6_ADDRSTRLEN);
		inet_ntop(clientAddr.ss_family, get_in_addr((struct sockaddr *)&clientAddr),clientIP, sizeof clientIP);
		//mClientIP->append(clientIP);
		//int numBytes = recv(clientSockFD, recvBuff, 511, 0);
		strcpy(mClientIP, clientIP);	
	}
	else
	{
		cout<<"\n socketClass::acceptConnections()->clientSockFD = -1"<<endl;
		perror("accept");
	}

	
	return clientSockFD;
}

/*
This function sends the file specified by "fileName" to the peer specified by the "sockFD"
It send the file in the chunks of size MAX_PACKET_LEN,
*/
bool socketClass::sendFile(string fileName, int sockFD)
{
        ifstream file (fileName.c_str(), ios::in|ios::ate);
	if(!file.good())
		return false;

	stringstream stream;
	long int length  = file.tellg();
        char packet[MAX_PACKET_LEN]; 
        file.seekg(0,ios::beg);

	long int read = 0;
	long int remaining = length;
        while(!file.eof())
        {
                memset(packet,0, sizeof(char)*MAX_PACKET_LEN);
		

		if(remaining >= MAX_PACKET_LEN)
		{
	                file.read(packet, MAX_PACKET_LEN);
			read = read + MAX_PACKET_LEN;
			remaining = length - read;

	                send(sockFD, packet, MAX_PACKET_LEN, 0);
		}
		else if(remaining == 0)
		{
			break;
		}
		else
		{
			file.read(packet, remaining);
                	send(sockFD, packet, remaining, 0);
			read = read + remaining;
			break;
		}
		//packet[MAX_PACKET_LEN-1] = '\0';
                //cout<<packet;
        }
	file.close();
	if(LOG_LEVEL == HIGH)
	{
		stream<<"Actual Length of file: "<<length;
		printMessage(stream.str());
		stream.clear();
		stream<<"Actual bytes sent: "<<read;
		printMessage(stream.str());
	}

    	return true;
}


//This function just needs the file name, file Length and socket descriptor to receive the file
//Every input to this function is assumed to be validated.
bool socketClass::recvFile(string fileName, long int fileSize, long int peerSockFD)
{
	ofstream file(fileName.c_str(), ios::out);
	if(!file.is_open())
        {
                printMessage("Failed to create file "+fileName+" on this machine");
                printMessage("Download for the file "+fileName+" failed");
                return false;
        }
	
        int read = 0;
        int remaining = fileSize;
	char msgPacket[MAX_PACKET_LEN];
	int recvBytes = 0;
        while(read < fileSize)
        {
                memset(msgPacket, 0, sizeof(char)*MAX_PACKET_LEN);
                recvBytes = recv(peerSockFD, msgPacket, MAX_PACKET_LEN, 0);
		if(recvBytes > 0)
		{
                	file.write(msgPacket, recvBytes);
	                read = read + recvBytes;
		}
		else
			break;
        }
	
	//cout<<"\n fileName: "<<fileName<<" -> Received bytes: "<<read<<endl;
	file.close();

	if(read > 0)
		return true;
	else
		return false;
}



//Start of all the utility functions
void *socketClass::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Validates the IP specified by "ip"
bool socketClass::validateIP(string ip)
{
	struct sockaddr_in sa;
	char str[INET_ADDRSTRLEN];

	// using the inet_pton to validate the IP
	//It returns -1 in case of error and 0 in case of invalid ip
	int retVal = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
	if(retVal <= 0)
	{
		cout<<"\n Invalid IP Entered"<<endl;
		return false;
	}
	else
		return true;
}

//Validates the port specified by "port"
bool socketClass::validatePort(string port)
{
	int portNumber = 0;
	stringstream stream;
	stream<<port;
	stream>>portNumber;
	if(portNumber <= 0 || portNumber > MAX_PORT_NUM)
	{
		cout<<"\n Invalid port Entered"<<endl;
		return false;
	}
	else
		return true;	
}

/*
Method to get the name of the peer based on the ip given
Reference: The code has been referenced from beej's guide
*/
string socketClass::getNameFromIP(string ip)
{
	if(ip.length() <= 0)
		return "";

        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);

        char name[MAX_IP_LEN];
	memset(name, 0, sizeof(char)*MAX_IP_LEN);

        int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), name, sizeof(name), NULL, 0, 0);
        if (res)
        {
                perror("getnameinfo");
                return "";
        }
	
	string strName = name;
	return strName;
}

/*
This methods returns the ip of the machine based on name given 
Reference:: This code has been referenced from the beej's guide.
*/
string socketClass::getIPFromName(string name)
{
        struct hostent *he;
        struct in_addr **addr_list;

         if ((he = gethostbyname(name.c_str())) == NULL) {  // get the host info
                herror("gethostbyname");
                return "";
        }

        // print information about this host:
	string ipAddress = "";
        addr_list = (struct in_addr **)he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
                ipAddress = inet_ntoa(*addr_list[i]);
        }

	return ipAddress;	
}


void socketClass::printMessage(string message)
{
	cout<<"\n"<<message<<endl;
	fflush(stdout);
}

//Function to close the socket.
//Sort of wrapper over close() system call.
void socketClass::closeSocket()
{
	if(mSocketFD > 0)
		close(mSocketFD);

	mSocketFD = -1;
	cout<<"\n closed the socket successfully";
}


socketClass::~socketClass()
{
	closeSocket();
}



