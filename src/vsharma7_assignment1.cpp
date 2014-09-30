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
 *
 * This contains the main function. Add further description here....
 */



#include <cstdlib>
#include "../include/global.h"

#include "../include/configFile.h"
#include "../include/client.h"
#include "../include/server.h"

#include<sstream>
#include<iostream>
using namespace std;

void displayCorrectUsage()
{
	cout<<"\n Correct Usage:";
        cout<<"\n For Server Mode:./"<<PROG_NAME<<" "<<SERVER<<"  <port_number>";
	cout<<"\n For Client Mode:./"<<PROG_NAME<<" "<<CLIENT<<"  <port_number>\n";
}

int main(int argc, char **argv)
{
	//If the number of argument is not correct return
	if(argc != 3)
	{
		cout<<"\n Wrong number of arguments";
		displayCorrectUsage();
		return 0;		
	}

	int x;	

	//Run the client or the server depending on the argument passed

	//using stringstream to convert string to int
	stringstream stream(argv[2]);
	long int portNumber;
	stream>>portNumber;

	//server and client pointers

	string mode = argv[1];
	if(portNumber > 65534)
	{
		cout<<"\n port number must be in the range (0,65534)";
	}
	if(mode == SERVER || argv[1] == "S")
	{
		//Run the SERVER here

		server serverObj(portNumber);
		serverObj.run();
	}
	else if(mode == CLIENT || argv[1] == "C")
	{
		//RUN the client here
		client clientObj(portNumber);
		clientObj.run();
	}
	else
	{
		cout<<"\n Incorrect Usage:";
		displayCorrectUsage();
	}

	return 0;
}
