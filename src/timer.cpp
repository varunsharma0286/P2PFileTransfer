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
   File Name: timer.cpp
   Description:
   Implements all the methods declared in the header file timer.h
   Provides the functionality of timer like startTimer(), stopTimer()
   to the program
 *
 */




#include "timer.h"


timer::timer()
{
	startTime = 0;
	endTime = 0;
	timeUnit = "milliseconds";
}


//Gets the time in millisecond.
//Reference: http://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-c
uint64_t timer::getTime()
{
	struct timeval tv;
	 gettimeofday(&tv, NULL);

	 uint64_t ret = tv.tv_usec;
	 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
	 ret /= 1000;

	 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 	ret += (tv.tv_sec * 1000);
	return ret;
}

void timer::startTimer()
{
	startTime = 0;
	endTime = 0;
	timeUnit = "milliseconds";
	startTime = getTime();
}


uint64_t timer::stopTimer()
{
	endTime = getTime();
}


