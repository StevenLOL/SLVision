/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include "TouchFinder.h"

#define HAND_CENTROID_SIMILARITY 100
class Hand
{
protected:
	unsigned long sessionID;
	
	float area; //blob area
	cv::Point centroid;  //blob centroid
	//cv::Point fingers[5]; //Finger
	std::vector<unsigned int> fingers;
	int numfingers;

	cv::Point center_hand;  //center of the hand (excluding the arm)
	float influence_radius; //hand radius (excluding the arm)

	cv::Point startarm;  // point where the arm starts (use to be a point on a side of the table)
	cv::Point endarm; //point where the hand ends
	
	cv::Point pinch_centre;  //centre of the pinch area
	float pinch_area;  //pinch area radius

	bool is_hand;

	cv::vector<cv::Point> blobPath;
	cv::vector<int> hull;
	std::vector<cv::Vec4i> defects;

	bool is_updated;

	//auxiliar data
	int edge_index;
	int further_index;
	float auxiliar_distance;
	float temp_dist;
	std::vector<int> finger_candidates;
public:
	//Functions
	Hand(void);
	Hand(unsigned long _sessionID, const cv::Point & _centroid, float area);
	bool IsItTheSame( cv::Point &point );
	void UpdateData( cv::Point &point, cv::vector<cv::Point> &path, float area, TouchFinder* tfinder );
	void AddPinch( cv::vector<cv::Point> &path, float area );
	unsigned long GetSID();
	bool IsValid();
	void Draw(TouchFinder* tfinder, bool force = false);

	bool IsUpdated();//{return is_updated;}
	cv::Point GetCentroid(){return centroid;}
	float GetArea(){return area;}
	cv::Point GetStartArm(){return startarm;}
	cv::Point GetEndArm(){return endarm;}
	cv::Point GetHandPoint(){return center_hand;}
	float GetHandInfluence(){return influence_radius;}
	cv::Point GetPinchPoint(){return pinch_centre;}
	float GetPinchInfluence(){return pinch_area;}
	int GetNumFingers(){return numfingers;}
	cv::vector<cv::Point>& GetPath(){return blobPath;}

private:
	void Reset();
	float IsNearEdge( cv::Point & p );
};

