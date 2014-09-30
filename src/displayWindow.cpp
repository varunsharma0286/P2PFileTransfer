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
 * File Name: displayWindow.cpp
   Description:
   This file has implementation of all the methods declared in the 
   corresponding header file "displayWindow.h"
 */


#include "../include/displayWindow.h"



displayWindow::displayWindow()
{
	//default constructor
	mMode.clear();
}


displayWindow::displayWindow(const string mode)
{
	mMode.clear();
	mMode = mode;
}

/*
This function displays the main menu for the client/server. 

*/
int displayWindow::displayMainMenu()
{
	int index = 0;
	cout<<"\n "<<++index<<". CREATOR";
	cout<<"\n "<<++index<<". HELP";
	cout<<"\n "<<++index<<". MYIP";
	cout<<"\n "<<++index<<". MYPORT";
	
	if(mMode == CLIENT)
	{
		cout<<"\n "<<++index<<". REGISTER <server IP> <port_no>";
	}
	
	cout<<"\n "<<++index<<". CONNECT <destination> <port no>";
	cout<<"\n "<<++index<<". LIST";
	cout<<"\n "<<++index<<". TERMINATE <connection id.>";
	cout<<"\n "<<++index<<". EXIT";
	cout<<"\n "<<++index<<". UPLOAD <connection id.> <file name>";
	cout<<"\n "<<++index<<". DOWNLOAD <connection id 1> <file1> <connection id 2> <file2> <connection id 3> <file3>";
	cout<<"\n "<<++index<<". STATISTICS"<<endl;
	cout<<"\n\n\n";
	return 0;
}

/*
This method is called in an event the user enters "HELP" command on the shell.
*/
void displayWindow::displayHelpMenu()
{
        int index = 0;
        cout<<"\n "<<++index<<". CREATOR";
	cout<<"\n    Displays the developers information :) ";
	cout<<"\n\n";

        cout<<"\n "<<++index<<". HELP-> Choose this option to know more about the available user interface option.\n";
	cout<<"\n\n";

        cout<<"\n "<<++index<<". MYIP-> Display the IP address of the host on which this process is being running.\n";
	cout<<"\n\n";

        cout<<"\n "<<++index<<". MYPORT-> Displays the port which is being used my this process for accepting the incoming connections.\n";
        cout<<"\n\n";
        
        if(mMode == CLIENT)
        {
                cout<<"\n "<<++index<<". REGISTER <server IP> <port_no>:";
		cout<<"\n    This command is used by the client to register itself with the server and to get the IP and listening port numbers of all the peers currently registered with the server.";
		cout<<"\n    This option takes two arguments: first is the server ip and the other one is the port on which this process is listening.";
		
        }
        cout<<"\n\n";
       
        cout<<"\n "<<++index<<". CONNECT <destination> <port no>";
	{
		cout<<"\n    This command establishes a new TCP connection to the specified <destination> at the specified <port no>. The <destination> can either be an IP address or a";
		cout<<"\n    hostname. (e.g., CONNECT euston.cse.buffalo.edu 3456 or CONNECT 192.168.45.55 3456). The  specified IP address should be a valid IP address and listed in"; 
		cout<<"\n    the Server-IP-List sent to the client by the server. It must be listed by the server in its Server-IP-List else the option will not work";
	}
        cout<<"\n\n";

        cout<<"\n "<<++index<<". LIST";
	{
                cout<<"\n    Display a numbered list of all the connections this process is part of. This numbered list will include connections initiated by this process and connections";
                cout<<"\n    initiated by other processes. The output will be displayed in the format shown in the example below";
		cout<<"\n    id: Hostname			IP address		Port No.";
		cout<<"\n     1: timberalake.cse.buffalo.edu	192.168.21.20		4545";
	}
        cout<<"\n\n";

        cout<<"\n "<<++index<<". TERMINATE <connection id.>";
	{
		cout<<"\n    This command will terminate the connection with the specific connection id. Take the connection id using the LIST command.";

	}
        cout<<"\n\n";

        cout<<"\n "<<++index<<". EXIT";
	cout<<"\n     This will close all the connections and will terminate this process";
        cout<<"\n\n";

        cout<<"\n "<<++index<<". UPLOAD <connection id.> <file name>";
	{
		cout<<"\n    Use this option to upload the specfied file to the target machine. Target machine is specified by the connection id. Take the connection id using the LIST Command";
	}
        cout<<"\n\n";

        cout<<"\n "<<++index<<". DOWNLOAD <connection id 1> <file1> <connection id 2> <file2> <connection id 3> <file3>";
	{
                cout<<"\n    This will download a file from each host specified in the command. Please note that the total number of hosts may be 1, 2 or 3. The hosts will be part of the list";
		cout<<"\n    of hosts displayed by the LIST command.";
	}
	cout<<"\n\n\n";
}

//Handles the case when user enter "CREATOR" command.
void displayWindow::displayCreator()
{
	cout<<"\n\n---------------------------Creators Information----------------------------------------------";
	cout<<"\nFull Name : Varun Sharma";
	cout<<"\nUBIT Name : vsharma7";
	cout<<"\nUB Emai ID: vsharma7@buffalo.edu";
	cout<<"\n\nI have read and understood the course academic integrity policy located at"; 
	cout<<"\nhttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity";
	cout<<"\n----------------------------------------------------------------------------------------------";
	cout<<"\n\n";
}


/*
This methods is called from the run() of server/client to display the shell to the user.
It takes the input from the user at the command line.
Then it trims the string entered by the user to remove unnecessary tabs and spaces.
It returns this trimmed command to the run().
*/
string displayWindow::runShell()
{
	string command = "";
	fflush(stdin);
	char stdinbuf[1000];
	fgets(stdinbuf, 1000, stdin);
	command  = stdinbuf;
	command = command.substr(0,command.length()-1);
	bool whiteSpace = false;
	string cleanCommand = "";

	for(int i=0;i<command.length();i++)
	{
		if(command[i] == TAB || command[i] == SPACE)
		{
			if(!whiteSpace)
			{
				cleanCommand = cleanCommand+" ";
				whiteSpace = true;
			}
		}
		else
		{
			cleanCommand = cleanCommand+command[i];
			whiteSpace = false;
		}
	}

	//cout<<"\n DEBUG::displayWindow::runShell(): command = "<<cleanCommand<<endl;
	return cleanCommand;
}


//Handles the case when user enters the MYIP command
void displayWindow::displayMyIP(string IP)
{
	cout<<"\n IP address of this machine is: "<<IP<<NEWLINE;
}

//Called when the user enters the MYPORT command.
void displayWindow::displayMyPort(string port)
{
	string object="";
	
	if(mMode == CLIENT)
	{
		object = "Client";
	}
	else
		object = "Server";

	cout<<"\n"<<object<<" is listening on port: "<<port<<NEWLINE;
}

displayWindow::~displayWindow()
{
	//default Destructor
}
