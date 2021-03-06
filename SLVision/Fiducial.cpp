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

#include "Fiducial.h"
#include "Globals.h"

#ifndef M_PI
#define M_PI 3.141592654f
#endif

#define DISTANCE_OFFSET 30
#define AREA_OFFSET 1000
#define REMOVE_OFFSET 500

Fiducial::Fiducial(void):x(0),y(0),area(0),size(0),a(cv::Point2f(0,0)),b(cv::Point2f(0,0)),c(cv::Point2f(0,0)),d(cv::Point2f(0,0)),fiducial_id(-1),orientation(0),is_updated(false),removed_time(-1)
{
	yaw=0;
	pitch=0;
	roll=0;
}

Fiducial::~Fiducial(void)
{
}

Fiducial::Fiducial(const Fiducial &copy):removed_time(-1)
{
	x = copy.x;
	y = copy.y;
	size = copy.size;
	a = cv::Point2f(copy.a.x,copy.a.y);
	b = cv::Point2f(copy.b.x,copy.b.y);
	c = cv::Point2f(copy.c.x,copy.c.y);
	d = cv::Point2f(copy.d.x,copy.d.y);

	fiducial_id = copy.fiducial_id;
	orientation = copy.orientation;
	area = copy.area;
	is_updated = true;

	yaw=copy.yaw;
	pitch=copy.pitch;

	roll=copy.roll;
	r11 = copy.r11;
	r12 = copy.r12;
	r13 = copy.r13;
	r21 = copy.r21;
	r22 = copy.r22;
	r23 = copy.r23;
	r31 = copy.r31;
	r32 = copy.r32;
	r33 = copy.r33;

	xpos = copy.xpos;
	ypos = copy.ypos;
	zpos = copy.zpos;

#ifdef USE_EIGHT_POINTS
	e = cv::Point2f(copy.e.x,copy.e.y);
	f = cv::Point2f(copy.f.x,copy.f.y);
	g = cv::Point2f(copy.g.x,copy.g.y);
	h = cv::Point2f(copy.h.x,copy.h.y);
#endif
	
	rotation_vector = cv::Mat(copy.rotation_vector);
	translation_vector = cv::Mat(copy.translation_vector);
}

void Fiducial::clear()
{
	x = 0;
	y = 0;
	a = cv::Point2f(0,0);
	b = cv::Point2f(0,0);
	c = cv::Point2f(0,0);
	d = cv::Point2f(0,0);
	fiducial_id = -1;
	orientation = 0;
	is_updated = true;
}

void Fiducial::SetId(unsigned int id)
{
	this->fiducial_id = id;
}

void Fiducial::SetSize(float size)
{
	this->size = size;
}

void Fiducial::Update(const Fiducial &copy)
{
	x = copy.x;
	y = copy.y;
	size = copy.size;
	a = cv::Point2f(copy.a.x,copy.a.y);
	b = cv::Point2f(copy.b.x,copy.b.y);
	c = cv::Point2f(copy.c.x,copy.c.y);
	d = cv::Point2f(copy.d.x,copy.d.y);
	orientation = copy.orientation;
	area = copy.area;
	is_updated = true;

	yaw=copy.yaw;
	pitch=copy.pitch;

	roll=copy.roll;
	r11 = copy.r11;
	r12 = copy.r12;
	r13 = copy.r13;
	r21 = copy.r21;
	r22 = copy.r22;
	r23 = copy.r23;
	r31 = copy.r31;
	r32 = copy.r32;
	r33 = copy.r33;

	xpos = copy.xpos;
	ypos = copy.ypos;
	zpos = copy.zpos;

#ifdef USE_EIGHT_POINTS
	e = cv::Point2f(copy.e.x,copy.e.y);
	f = cv::Point2f(copy.f.x,copy.f.y);
	g = cv::Point2f(copy.g.x,copy.g.y);
	h = cv::Point2f(copy.h.x,copy.h.y);
#endif

	rotation_vector = cv::Mat(copy.rotation_vector);
	translation_vector = cv::Mat(copy.translation_vector);
}

void Fiducial::Update(float _x, float _y,cv::Point2f _a,cv::Point2f _b,cv::Point2f _c,cv::Point2f _d, float _area, int orientation)
{
	this->x = _x;
	this->y = _y;
	this->a = cv::Point2f(_a.x, _a.y);
	this->b = cv::Point2f(_b.x, _b.y);
	this->c = cv::Point2f(_c.x, _c.y);
	this->d = cv::Point2f(_d.x, _d.y);
	this->area = fabs(_area);
	is_updated = true;
	this->orientation = orientation;
#ifdef USE_EIGHT_POINTS
	has_inner_corenr = false;
#endif
}

#ifdef USE_EIGHT_POINTS
	void Fiducial::Update(float _x, float _y,cv::Point2f _a,cv::Point2f _b,cv::Point2f _c,cv::Point2f _d,cv::Point2f _e,cv::Point2f _f,cv::Point2f _g,cv::Point2f _h, float _area, int _orientation)
	{
		Update(_x, _y,_a, _b, _c, _d, _area, _orientation);
		this->e = _e;
		this->f = _f;
		this->g = _g;
		this->h = _h;
		has_inner_corenr = true;
	}

	bool Fiducial::HasInnerCorners()
	{
		return has_inner_corenr;
	}
#endif

bool Fiducial::Is_inside(const Fiducial &f)
{
	if( f.area  < area)
	{
		if(!(	IsLeft(a.x, a.y, b.x, b.y, f.x, f.y) < 0 ||
				IsLeft(b.x, b.y, c.x, c.y, f.x, f.y) < 0 ||
				IsLeft(c.x, c.y, d.x, d.y, f.x, f.y) < 0 ||
				IsLeft(d.x, d.y, a.x, a.y, f.x, f.y) < 0 ))
		{
			return true;
		}
	}
	return false;
}


bool Fiducial::CanUpdate(const Fiducial &f, float & minimum_distance)
{
	if ( fabs((float)(f.area-area)) <= AREA_OFFSET )
	{
		float tmp = fabs(fnsqdist(f.x,f.y,x,y));
		if(tmp <= DISTANCE_OFFSET && tmp < minimum_distance)
		{
			minimum_distance = tmp;
			return true;
		}
	}
	return false;
}

int Fiducial::GetOrientation()
{
	return orientation;
}

unsigned int Fiducial::GetFiducialID()
{
	return fiducial_id;
}

float Fiducial::GetX()
{
	return x;
}

float Fiducial::GetY()
{
	return y;
}

float Fiducial::GetSize()
{
	return size;
}

cv::Mat Fiducial::GetRotationVector()
{
	return rotation_vector;
}

cv::Mat Fiducial::GetTranslationVector()
{
	return translation_vector;
}

bool Fiducial::IsUpdated()
{
	if(is_updated)
	{
		is_updated = false;
		return true;
	}
	return false;
}

bool Fiducial::CanBeRemoved(double actual_time)
{
	if(removed_time != -1 && ((actual_time - removed_time)/(cvGetTickFrequency()*1000.) >= REMOVE_OFFSET) )
		return true;
	return false;
}

void Fiducial::RemoveStart(double actual_time)
{
	removed_time = actual_time;
}

void Fiducial::SetOrientation(int o)
{
	orientation = o;
}

cv::Point2f Fiducial::GetCorner(int corner)
{
	switch(corner)
	{
	case 0:
		return a;
		break;
	case 1:
		return b;
		break;
	case 2:
		return c;
		break;
	case 3:
		return d;
		break;
#ifdef USE_EIGHT_POINTS
	case 4:
		return e;
		break;
	case 5:
		return f;
		break;
	case 6:
		return g;
		break;
	case 7:
		return h;
		break;
#endif
	}
	return a;
}

void Fiducial::OritentateCorners()
{
	
	int markerDirection = GetOrientation();
#ifdef USE_EIGHT_POINTS
	cv::Point2f te,tf,tg,th;
	te = cv::Point2f(e); tf= cv::Point2f(f); tg = cv::Point2f(g); th = cv::Point2f(h);
#endif
	cv::Point2f ta,tb,tc,td;
	ta = cv::Point2f(a); tb = cv::Point2f(b); tc = cv::Point2f(c); td = cv::Point2f(d);
	//a0 b1 c2 d3
	switch(markerDirection)
	{
	case 0:
		a = tb;
		b = tc;
		c = td;
		d = ta;
#ifdef USE_EIGHT_POINTS
		e = tf;
		f = tg;
		g = th;
		h = te;
#endif
		break;
	case 1:
		a = tc;
		b = td;
		c = ta;
		d = tb;
#ifdef USE_EIGHT_POINTS
		e = tg;
		f = th;
		g = te;
		h = tf;
#endif
		break;
	case 2:

		break;
	case 3:
		a = td;
		b = ta;
		c = tb;
		d = tc;
#ifdef USE_EIGHT_POINTS
		e = th;
		f = te;
		g = tf;
		h = tg;
#endif
		break;
	}
}

void Fiducial::CalculateIntrinsics()
{
	if(size != 0)
	{
		/******************************************************
		* Define target points
		*******************************************************/
		cv::Mat ObjPoints;
#ifdef USE_EIGHT_POINTS
		if(HasInnerCorners())
			ObjPoints = cv::Mat(8,3,CV_32FC1);
		else
#endif
			ObjPoints = cv::Mat(4,3,CV_32FC1);

		ObjPoints.at<float>(0,0)=0;
		ObjPoints.at<float>(0,1)=size;
		ObjPoints.at<float>(0,2)=0;
		ObjPoints.at<float>(1,0)=size;
		ObjPoints.at<float>(1,1)=size;
		ObjPoints.at<float>(1,2)=0;
		ObjPoints.at<float>(2,0)=size;
		ObjPoints.at<float>(2,1)=0;
		ObjPoints.at<float>(2,2)=0;
		ObjPoints.at<float>(3,0)=0;
		ObjPoints.at<float>(3,1)=0;
		ObjPoints.at<float>(3,2)=0;
#ifdef USE_EIGHT_POINTS
		if(HasInnerCorners())
		{
			ObjPoints.at<float>(4,0)=5;
			ObjPoints.at<float>(4,1)=size-5;
			ObjPoints.at<float>(4,2)=0;
			ObjPoints.at<float>(5,0)=size-5;
			ObjPoints.at<float>(5,1)=size-5;
			ObjPoints.at<float>(5,2)=0;
			ObjPoints.at<float>(6,0)=size-5;
			ObjPoints.at<float>(6,1)=5;
			ObjPoints.at<float>(6,2)=0;
			ObjPoints.at<float>(7,0)=5;
			ObjPoints.at<float>(7,1)=5;
			ObjPoints.at<float>(7,2)=0;
		}
#endif
		/******************************************************
		* Define source points
		*******************************************************/
		cv::Mat ImagePoints;
#ifdef USE_EIGHT_POINTS
		if(HasInnerCorners())
			ImagePoints= cv::Mat(8,2,CV_32FC1);
		else
#endif
		ImagePoints= cv::Mat(4,2,CV_32FC1);

		ImagePoints.at<float>(0,0)=(a.x);
		ImagePoints.at<float>(0,1)=(a.y);
		ImagePoints.at<float>(1,0)=(b.x);
		ImagePoints.at<float>(1,1)=(b.y);
		ImagePoints.at<float>(2,0)=(c.x);
		ImagePoints.at<float>(2,1)=(c.y);
		ImagePoints.at<float>(3,0)=(d.x);
		ImagePoints.at<float>(3,1)=(d.y);
#ifdef USE_EIGHT_POINTS
		if(HasInnerCorners())
		{
			ImagePoints.at<float>(4,0)=(e.x);
			ImagePoints.at<float>(4,1)=(e.y);
			ImagePoints.at<float>(5,0)=(f.x);
			ImagePoints.at<float>(5,1)=(f.y);
			ImagePoints.at<float>(6,0)=(g.x);
			ImagePoints.at<float>(6,1)=(g.y);
			ImagePoints.at<float>(7,0)=(h.x);
			ImagePoints.at<float>(7,1)=(h.y);
		}
#endif
		/******************************************************
		* Pose Estimation
		*******************************************************/
		cv::solvePnP(ObjPoints, ImagePoints, Globals::CameraMatrix, Globals::Distortion,rotation_vector,translation_vector);
		rotation_vector.convertTo(rotation_vector,CV_32F);
		translation_vector.convertTo(translation_vector ,CV_32F);
		/******************************************************
		* Prepare parameters
		*******************************************************/
		cv::Mat R(3,3,CV_32F);
		/*///**** /////
		cv::Mat Rz= cv::Mat::eye(3,3,CV_32F);
		float angle = M_PI;
		float c = cos(angle);
		float s = sin (angle);
		Rz.at<float>(0,0) = c;				Rz.at<float>(0,1) = -s;				Rz.at<float>(0,2) = 0;
		Rz.at<float>(1,0) = s;				Rz.at<float>(1,1) = c;				Rz.at<float>(1,2) = 0;
		Rz.at<float>(2,0) = 0;				Rz.at<float>(2,1) = 0;				Rz.at<float>(2,2) = 1;
		cv::Mat Rx= cv::Mat::eye(3,3,CV_32F);
		Rx.at<float>(0,0) = 1;				Rx.at<float>(0,1) = 0;				Rx.at<float>(0,2) = 0;
		Rx.at<float>(1,0) = 0;				Rx.at<float>(1,1) = c;				Rx.at<float>(1,2) = s;
		Rx.at<float>(2,0) = 0;				Rx.at<float>(2,1) = -s;				Rx.at<float>(2,2) = c;
		R = Rx;

		Rodrigues(R, rotation_vector);
		////****/////
		//std::cout << rotation_vector.ptr<float>(0)[0] << "  "<< rotation_vector.ptr<float>(0)[1] << "  "<< rotation_vector.ptr<float>(0)[2] << std::endl;
		//
		Rodrigues(rotation_vector, R);

		xpos = translation_vector.ptr<float>(0)[0];
		ypos = translation_vector.ptr<float>(0)[1];
		zpos = translation_vector.ptr<float>(0)[2];
		//std::cout << fiducial_map[tmp_ssid]->xpos << "\t" << fiducial_map[tmp_ssid]->ypos << "\t" << fiducial_map[tmp_ssid]->zpos << std::endl;
		//Rotate XAxis

//to uncomment :
		cv::Mat Rx= cv::Mat::eye(3,3,CV_32F);
		float angle = M_PI;
		Rx.at<float>(1,1) = cos(angle);
		Rx.at<float>(1,2) = -sin(angle);
		Rx.at<float>(2,1) = sin(angle);
		Rx.at<float>(2,2) = cos(angle);
		R = R*Rx;

		r11 = -R.ptr<float>(0)[0];
		r12 = R.ptr<float>(0)[1];
		r13 = -R.ptr<float>(0)[2];
		r21 = R.ptr<float>(1)[0];
		r22 = -R.ptr<float>(1)[1];
		r23 = -R.ptr<float>(1)[2];
		r31 = -R.ptr<float>(2)[0];
		r32 = -R.ptr<float>(2)[1];
		r33 = R.ptr<float>(2)[2];
		
		//yaw = atan2(-R.ptr<float>(2)[0],sqrt( R.ptr<float>(2)[1]*R.ptr<float>(2)[1] + R.ptr<float>(2)[2]*R.ptr<float>(2)[2]));
		//pitch = atan2(R.ptr<float>(2)[1],R.ptr<float>(2)[2]);
		//roll = (2.0f*3.141592654f)-atan2(R.ptr<float>(1)[0],R.ptr<float>(0)[0]);
		yaw = atan2(-r31,sqrt( r32*r32 + r33*r33));
		pitch = atan2(r32,r33);
		roll = (2.0f*M_PI)-atan2(r21,r11);

		//std::cout << "ypr " << yaw << "  " << pitch << "  " << roll << std::endl;

	}
}

/****************************************
* general purpose functions
*****************************************/

float nsqdist2(const cv::Point &a, const cv::Point &b)
{
	return insqdist(a.x,a.y,b.x,b.y);
}

float nsqdist(const cv::Point2f &a, const cv::Point2f &b)
{
	return insqdist(a.x,a.y,b.x,b.y);
}

float fnsqdist(float x, float y, float a, float b)
{
	float uu = (float)(a-x);
	float vv = (float)(b-y);
	return sqrt(uu*uu + vv*vv);
}

float insqdist(int x, int y, int a, int b)
{
	float uu = (float)(a-x);
	float vv = (float)(b-y);
	return sqrt(uu*uu + vv*vv);
}

//vector AB  point C
float vect_point_dist(float ax, float ay, float bx, float by, float cx, float cy)
{
	float abx = bx-ax;
	float aby = by-ay;
	float acx = cx-ax;
	float acy = cy-ay;
	return ( ( abx*acy - aby*acx) / sqrt(abx*abx +aby*aby));
}

float ivect_point_dist(float ax, float ay, float bx, float by, float cx, float cy)
{
	float abx = (float)(bx-ax);
	float aby = (float)(by-ay);
	float acx = (float)(cx-ax);
	float acy = (float)(cy-ay);
	return ( ( abx*acy - aby*acx) / sqrt(abx*abx +aby*aby));
}

/// Givven a segment A(ax,ay)--B(bx,by) returns if the point P(px,py) is at the left side of the segment.
double IsLeft(double ax, double ay, double bx, double by, float px, float py)
{
	return ((bx - ax) * (py - ay) - (px - ax) * (by - ay));
}

