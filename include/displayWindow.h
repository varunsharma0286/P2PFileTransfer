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
 * File Name: displayWindow.h
   Description: 
   Header file for the displayWindow class.
 */


#include "configFile.h"

#include<stdio.h>
#include<sstream>
#include<algorithm>
#include<string>
#include<iostream>
using namespace std;


#ifndef _DISPLAYWINDOW_H
#define _DISPLAYWINDOW_H

class displayWindow
{
	string mMode;
public:
	displayWindow();
	displayWindow(const string mode);

	int displayMainMenu();
	void displayHelpMenu();

	void displayMyIP(string ip);
	void displayMyPort(string port);

	string runShell();
	void displayCreator();

	~displayWindow();
};

#endif
