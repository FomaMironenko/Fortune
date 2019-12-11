#pragma once
#ifndef OBJECTS
#define OBJECTS

#include "primitives.h"

#include "pch.h"
#include <iostream>
#include <algorithm>
#include <list>

using namespace std;

//компаратор для приоритетной очереди
struct Comp
{
	bool operator()(Event* ev1, Event*ev2)
	{
		return *ev1 < *ev2;
	}
};

typedef priority_queue<Event*, vector<Event*>, Comp> myq;




struct Parabola;
struct Edge;
struct Locus;
struct Event;
struct NewSite;
struct ArcInter;


struct Parabola
{

	Point focus;
	MyDouble* sweep; // y coordinate
};


// Components of beachline //
struct Arc
{

	Parabola curve;
	Point start;
	bool valid;
};

struct Edge
{

	Segment edge;
	Locus* face;
	bool valid;
};
/////////////////////////////


struct Locus
{
	Locus(): centre(Point()), vertexes(list<Point>(0))
	{	
		//an infinity point
		vertexes.push_back(Point(numeric_limits<double>::max(), numeric_limits<double>::max()));
	}
	void add(Point pnt);
	void print();

	Point centre;
	list<Point> vertexes;
};


struct Diagram
{
	Diagram() {    };
	Diagram();
	Diagram(int n);
	void get(myq & Q);
	void print();
	
	int n;
	vector<Locus> faces;
};


enum type{site, inter};

struct Event
{
	Event()
	{	}
	Event(MyDouble y): y(y)
	{	}

	bool operator <(Event & other);
	bool operator >(Event & other);


	type tp;
	MyDouble y;
	bool valid;
};

struct NewSite : Event
{
	NewSite(): Event()
	{	this->tp = site;	}
	NewSite(MyDouble y): Event(y)
	{	this->tp = site;	}

	Edge e1;
	Edge e2;
	Point vertex;
};

struct ArcInter : Event
{
	ArcInter(): Event()
	{	this->tp = inter;	}
	ArcInter(MyDouble y) : Event(y)
	{	this->tp = inter;	}


};


#endif