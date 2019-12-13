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
struct SiteEvent;
struct CircEvent;

// компаратор для приоритетной очереди
struct Comp
{
	bool operator()(Event*, Event*);
};
// компаратор для береговой линии
struct BComp
{
	bool operator()(BL*, BL*);
};
typedef priority_queue<Event*, vector<Event*>, Comp> myq;




struct Parabola
{
	Parabola()
	{	}
	Parabola(Locus* focus): focus(focus)
	{	}


	//the face with the focus
	Locus* focus;
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
};

struct Edge: BL
{
	Edge() : BL(), inter(nullptr)
	{ this->tp = edg; }
	Edge(MyDouble x) : BL(x), inter(nullptr)
	{ this->tp = edg; }

	void unvalidate();
	~Edge();

	// пересечение с другим ребром
	CircEvent* inter;
	Segment edge;
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


enum type{site, circ};

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

struct SiteEvent : Event
{
	SiteEvent(): Event()
	{ this->tp = site; }
	SiteEvent(MyDouble y, Point pnt): Event(y, pnt)
	{ this->tp = site; }

	Locus* face;
};

struct CircEvent : Event
{
	CircEvent(): Event()
	{	this->tp = circ;	}
	CircEvent(MyDouble y, Point pnt) : Event(y, pnt)
	{	this->tp = circ;	}

	Edge* eleft;
	Edge* eright;
};


#endif
