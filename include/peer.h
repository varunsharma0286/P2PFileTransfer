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
 * File Name: peer.h
   Description: 
   Header file for the peer.h class.
 */


#include<stdio.h>
#include<iostream>
using namespace std;

#ifndef _PEER_H
#define _PEER_H

class peer
{
public:
	int mID;
	string mIP;
	string mPort;
	int mSockFD;
	string mName;
	
	void clear();
	peer();
	peer(int id, string ip, string port, int mSockFD);
	~peer();	
};


#endif
