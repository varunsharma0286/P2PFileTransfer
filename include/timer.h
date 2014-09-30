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
 * File Name: timer.h
   Description: 
   Header file for the timer class.
 */


#include<stdint.h>
#include <sys/time.h>
#include <ctime>
#include<stdio.h>
#include<string>
#include<iostream>
using namespace std;

#ifndef _TIMER_H
#define _TIMER_H
class timer
{

public:
	uint64_t startTime;
	uint64_t endTime ;
	string timeUnit ;
	timer();
	uint64_t getTime();
	void startTimer();
	uint64_t stopTimer();
	
	//Return time in seconds
	/*inline uint64_t getExecutionTime()
	{
		uint64_t execTime =  (endTime - startTime)/1000;
		if(execTime <= 0)
		{
			execTime = getExecutionTimeMS();
			timeUnit = "milliseconds";
		}
		return execTime;
	}*/
	
	//Clears/Reset the timer.
	inline void reset()
	{
		startTime = 0;
		endTime = 0;
		timeUnit = "millisecond";	
	}
	
	//Return time in milliseconds
	inline uint64_t getExecutionTime()
	{
		return (endTime - startTime);
	}
};

#endif
