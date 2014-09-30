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
 * File Name: peer.cpp
   Description:
   Implementation of the methods defined in the header "peer.h"
 */



#include "peer.h"



peer::peer()
{	
	mID = 0;
	mIP = "";
	mPort = "";
	mSockFD = 0;
	mName = "";
}


peer::peer(int id, string ip, string port, int sockFD)
{

	mID     = id;
	mIP     = ip;
	mPort   = port;
	mSockFD = sockFD;

}

void peer::clear()
{
        mID = 0;
        mIP = "";
        mPort = "";
        mSockFD = 0;
        mName = "";
}

peer::~peer()
{
	//empty destructor
}
