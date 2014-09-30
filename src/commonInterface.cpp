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
 * file Name: commonInterface.cpp
   Description: It is an interface for both client.cpp and server.cpp
   This file has the implementation of all the methods commond to both client.cpp and server.cpp
   It also contains the implementation of some useful utility methods that are useful for both
   client.cpp and server.cpp
 * 
 */


#include "../include/commonInterface.h"


commonInterface::commonInterface()
{
	//empty constructor
}

commonInterface::~commonInterface()
{
	if(!mWindow)
		delete mWindow;
	
	if(!mSocket)
		delete mSocket;
}

/*
Main function that is called from the run() after the users input has been taken from the runShell() method.
It traverses the users input and checks what command has been entered by the user.
Then it calls the appropriate method to handle this input.
*/

int commonInterface::runCommand(string command)
{
	char commandStr[MAX_COMMAND_LEN];
	memset(commandStr,0, MAX_COMMAND_LEN);
	
	sscanf(command.c_str(), "%s", commandStr);
	if(strlen(commandStr)<=0)
		return ERROR;

	string upperCaseCommand = commandStr;
	for(int i=0;i<upperCaseCommand.length();i++)
	{
		upperCaseCommand[i] = toupper(upperCaseCommand[i]);
	}
	
	if(strcmp(upperCaseCommand.c_str(), "CREATOR") == 0)
	{
		mWindow->displayCreator();
	}
	else if(strcmp(upperCaseCommand.c_str(), "HELP") == 0)
	{
		mWindow->displayHelpMenu();
	}
	else if(strcmp(upperCaseCommand.c_str(), "MYIP") == 0)
	{
		mWindow->displayMyIP(mMyIP);
	}
	else if(strcmp(upperCaseCommand.c_str(), "MYPORT") == 0)
	{
		mWindow->displayMyPort(mPort);
	}
	else if(strcmp(upperCaseCommand.c_str(), "REGISTER") == 0)
	{
		if(mMode == SERVER)
		{
			cout<<"\n Register command not available in server mode"<<endl;
			return true;
		}
		registerCommand(command);	
	}
	else if(strcmp(upperCaseCommand.c_str(), "CONNECT") == 0)
	{
		connectCommand(command);
		return 1;	
	}
	else if(strcmp(upperCaseCommand.c_str(), "LIST") == 0)
	{
		listCommand();
	}
	else if(strcmp(upperCaseCommand.c_str(), "TERMINATE") == 0)
	{
        	terminateCommand(command);
	}
	else if(strcmp(upperCaseCommand.c_str(), "EXIT") == 0)
	{
		exitCommand();
		return EXIT_PROGRAM;
	}
	else if(strcmp(upperCaseCommand.c_str(), "UPLOAD") == 0)
	{
		uploadCommand(command);	
	}
	else if(strcmp(upperCaseCommand.c_str(), "DOWNLOAD") == 0)
	{
		downloadCommand(command);
	}
	else if(strcmp(upperCaseCommand.c_str(), "STATISTICS") == 0)
	{
                statCommand();
	}
	else 
	{
		//Wrong command enter
		cout<<"\n Command not found. Please note the commands are case sensetive\n";
		return ERROR;
	}
}

//This is a utility methods that gets the command list from the 
//string "input". It returns the vector<string> having the commands.
vector<string> commonInterface::getCommandList(string input, char delimiter)
{
	stringstream stream(input);
    	string element;

	vector<string> command;
	command.clear();
	int position = 0;
   	while (std::getline(stream, element, delimiter)) {
		if(position != 0)
        		command.push_back(element);
		else 
			position++;
    	}
    	return command;		
}


/*
This is a utility method to create a message from the vector input.
Returns the created message.
*/
string commonInterface::createMessage(vector<string> input)
{
	stringstream stream;
	string message = "";
	if(input.size()<=0)
	{
		return message;
	}

	vector<string>::iterator itr;	
	for(itr=input.begin();itr!=input.end();itr++)
	{
		stream<<*itr<<SEPERATOR;
	}

	stream >> message;
	message = message.substr(0, message.length()-1);
	return message;
}

/*
Utility method to split the string "msg" based on the delimiter specified by 
the "delimited". It returns the vector<string> of the tokens.
*/
vector<string> commonInterface::splitMessage(const char *msg, char delimiter)
{
	vector<string> message;
	message.clear();

	if(!msg || (strlen(msg)<=0))
		return message;

	//Create a local copy of the msg
	string localMsg(msg);
	
	/*char *tokens = strtok(const_cast<char *>(localMsg.c_str()), &delimiter);
  	while (tokens != NULL)
  	{
    		message.push_back(tokens);
   		tokens = strtok (NULL, &delimiter);
  	}*/

        string token = "";
        for(int i=0;i<localMsg.length();i++)
        {
                if(localMsg[i] == delimiter)
                {
			message.push_back(token);
                        token = "";
                }
                else
                {
                        token = token+localMsg[i];
                }
        }
	
	if(token.length() > 0)
	{
		message.push_back(token);
	}
	return message;
}

/*
Method called from handleRecv() of client/server to interpret what 
action needs to be called for this message.
*/
char commonInterface::checkMessage(char *msg)
{
	char retVal = (char)0;
	if(!msg || (strlen(msg) == 0))
		return retVal;

	string localMsg(msg);
	char *command = NULL;
	command = strtok(const_cast<char*>(localMsg.c_str()), ":");
	if(strcmp(command, "REGISTER") == 0)
	{
		return REGISTER;
	}
	else if(strcmp(command, "CONNECT") == 0)
	{
		return CONNECT;
	}
	else if(strcmp(command, "TERMINATE") == 0)
	{
		return TERMINATE;
	}
	else if(strcmp(command, "EXIT") == 0)
	{
		return EXIT;
	}
	else if(strcmp(command, "UPLOAD") == 0)
	{
		return UPLOAD;
	}
	else if(strcmp(command, "DOWNLOAD") == 0)
	{
		return DOWNLOAD;
	}
	else if(strcmp(command, "STATS") == 0) //For handling the Stats messages from server
	{
		return SENDSTATS;
	}
	else
	{
		//Invalid case. Return error; 
		return retVal;
	}
}	

/*
Utility method to print the message. Safe as we 
are using fflush after using the cout.
*/
void commonInterface::printMessage(string message)
{
	cout<<"\n"<<message<<endl;
	fflush(stdout);
}	


/*
Utility method to extract the name of the file from the 
string "name". For e.g. if the input is /home/vsharma7/file.txt
then it returns "file.txt"
*/
string commonInterface::extractFileName(string& name)
{
	string fileName = "";
	if(name.length() <= 0)
		return fileName;

	for(int i = name.length()-1; i>=0 ;i--)
	{
		if(name[i] == '/')
		{
			break;
		}
		else
		{
			fileName = name[i]+fileName;
		}
	}

	return fileName;
}











