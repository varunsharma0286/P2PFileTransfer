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
   File Name: configFile.h
   Description:
 This file has all the configuration parameters for our application.
 The key config parameters are the "MAX_PACKET_LEN" which defines 
 the packet size used for the send/recv operations.
 Another important parameter is the "LOG_LEVEL". It can be used 
 to generate debug logs by setting its value to "H" (i.e. high)
*/


#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H


//Log level parameters
#define HIGH 'H'
#define LOW  'L'
#define LOG_LEVEL 'L'

//comman configuration parameters
#define ERROR -1
#define MAX_COMMAND_LEN 50
#define MAX_IP_LEN 100
#define EXIT_PROGRAM 25
#define SUCCESS 1
#define SEPERATOR ":"
#define CHAR_SEPERATOR ':'
#define SPACE ' '
#define TAB '\t'
#define PRESS_ENTER "Press Enter To Continue....."
#define PEER_SEPERATOR '-'
#define ACK "ACK"
#define INVALID_FILE "INVALID"


//Start of the Main file config params
#define PROG_NAME "prog_name"
#define SERVER "s"
#define CLIENT "c"
#define MAX_PORTS 65534

//displayWindow file config params
#define CREATOR "CREATOR"
#define HELP "HELP"
#define MYIP "MYIP"
#define MYPORT "MYPORT" 
#define REGISTER 'R'
#define CONNECT 'C'
#define LIST "LIST"
#define TERMINATE 'T'
#define EXIT 'E'
#define UPLOAD 'U'
#define DOWNLOAD 'D'
#define SENDSTATS 'S'
#define STATS "STATISTICS"

#define SHELL_SYMBOL "[PA1]$"
#define NEWLINE "\n\n"


//socketclass config params
#define BACKLOG 10
#define MSG_LEN 512
#define MAX_PORT_NUM 65534
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define MAX_PACKET_LEN 1000
#define START_UPLOAD "UPLOAD"
#define END "END"


//Error display message used for printing
#define USE_HELP "Please use \"HELP\" command for more details"
#define INVALID_PORT "Invalid port number entered\n.Please check the port number and try again"
#define NOT_REGISTERED "Client is not registered with server\n.Please use the \"Register\" command to register the client with server first"

#define ALREADY_CONNECTED "Client is alread connected to this peer"
#define UNKNOWN_CLIENT "Request host/ip is not a know peer"
#define CONNECT_FAILED "CONNECT command failed\n Please retry"

#define VALID_DOWNLOAD "DOWNLOAD <connection id 1> <file1> <connection id 2> <file2> <connection id 3> <file3>"

 
//End of the config parameters




#endif

