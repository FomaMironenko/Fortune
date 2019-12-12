#pragma once
#ifndef OBJECTS
#define OBJECTS

#include "primitives.h"

#include "pch.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <list>

using namespace std;


struct Parabola;
struct BL;
struct Arc;
struct Edge;
struct Locus;
struct Diagram;
struct Event;
struct NewSite;
struct ArcInter;

//компаратор для приоритетной очереди
struct Comp
{
	bool operator()(Event*, Event*);
};
struct BComp
{
	bool operator()(BL*, BL*);
};
typedef priority_queue<Event*, vector<Event*>, Comp> myq;


struct Parabola
{

	Point focus;
};


// Components of beachline //
enum btype {arc, edg};
struct BL
{
	BL(): valid(true)
	{	}
	BL(MyDouble x): x(x)
	{	}
	bool operator <(BL & other);

	bool valid;
	MyDouble x;
	btype tp;
};


struct Arc: BL
{
	Arc(): BL()
	{ this->tp = arc; }
	Arc(MyDouble x): BL(x)
	{ this->tp = arc; }

	Parabola curve;
	Point start;
};

struct Edge: BL
{
	Edge() : BL()
	{ this->tp = edg; }
	Edge(MyDouble x) : BL(x)
	{ this->tp = edg; }

	Segment edge;
	Locus* face;
};
/////////////////////////////


struct Locus
{
	Locus(): centre(Point(0, 0))
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
	Diagram(): n(0) {    };
	Diagram(int n);
	void get(myq & Q);
	void print();
	
	int n;
	vector<Locus*> faces;
};


enum type{site, inter};

struct Event
{
	Event(): valid(true)
	{	}
	Event(MyDouble y, Point pnt): y(y), valid(true)
	{	
		vertex = Point(pnt.x(), pnt.y());
	}
	bool operator==(Event & other);
	bool operator <(Event & other);
	bool operator >(Event & other);
	bool operator>=(Event & other);
	bool operator<=(Event & other);


	type tp;
	Point vertex;
	MyDouble y;
	bool valid;
};

struct NewSite : Event
{
	NewSite(): Event()
	{ this->tp = site; }
	NewSite(MyDouble y, Point pnt): Event(y, pnt)
	{ this->tp = site; }

	Locus* face;
};

struct ArcInter : Event
{
	ArcInter(): Event()
	{	this->tp = inter;	}
	ArcInter(MyDouble y, Point pnt) : Event(y, pnt)
	{	this->tp = inter;	}

	Edge e1;
	Edge e2;
};


#endif
