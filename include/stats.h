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
 * File Name: stats.h
   Description: 
   Header file for the stats class.
 */


#include<stdio.h>
#include<iostream>
using namespace std;


#ifndef _STATS_H
#define _STATS_H
class stats
{
public:
	stats()
	{
		mPeerName = "";
		mUploadSpeed = 0;
		mUploadCount = 0;
		mDownloadSpeed = 0;
		mDownloadCount = 0;
		mTimeUnit = "millisecond";
	}

	string mPeerName;
	long int mUploadSpeed;
	long int mDownloadSpeed;
	
	long int mUploadCount;
	long int mDownloadCount;
	string mTimeUnit;

	inline long int getAvgUploadSpeed()
	{
		if(mUploadCount <= 0)
			return 0;

		long int average = mUploadSpeed/mUploadCount;
		return average;
	}

	inline long int getAvgDownloadSpeed()
	{
		if(mDownloadCount <= 0)
		{
			return 0;
		}

		long int average = mDownloadSpeed/mDownloadCount;
		return average;
	}
};
#endif
