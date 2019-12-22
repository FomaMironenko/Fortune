#pragma once
#ifndef OBJECTS
#define OBJECTS

#include "primitives.h"

#include "pch.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <list>
#include <math.h>

using namespace std;


struct Arc;
struct Edge;
struct Locus;
struct Diagram;
struct Event;
struct SiteEvent;
struct CircEvent;
struct Node;
struct AVL;

// A comparator for the priority queue myq
struct Comp
{
	bool operator()(Event*, Event*);
};
typedef priority_queue<Event*, vector<Event*>, Comp> myq;
 

pair<MyDouble, MyDouble> solve_square(MyDouble, MyDouble, MyDouble);

// Components of beachline //
// arc with parabola interface
struct Arc
{
	Arc()
	{    }
	Arc(SiteEvent* ev);
	Arc(Locus* face);

	pair<MyDouble, MyDouble> intersection(Arc &, MyDouble);
	MyDouble left_inter(Arc &, MyDouble);
	MyDouble right_inter(Arc &, MyDouble);
	// y coordinate
	MyDouble of(MyDouble, MyDouble);

	Locus* face;
	/*
	if  yd  is the directris (sweepline) y coordinate
		yf  is the focus y coordinate (face->centre.y())
		xf  is the focus x coordinate (face->centre.x())
	then the parabola equasion is

	y = ((x - xf)^2) / (2*(yf - yd)) + (yf + yd)/2

	*/
};

struct Edge
{
	Edge(): inters(vector<CircEvent*>(0))
	{	 }
	Edge(Segment guide);
	Edge(Arc* left, Arc* right, CircEvent* evt);

	// edges are deleted furst then - corresponding events
	void unvalidate();
	//needed for Node::swap_list_fields
	void reset_event(Node*, Node*);
	~Edge();

	Segment edge;
	//!! случай если пересечений несколько
	// пересечение с другим ребром
	vector<CircEvent*> inters;
	Node* left;
	Node* right;
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

	bool operator <(Node* parab);
	bool operator >(Node* parab);
	bool operator==(Node* parab);

	type tp;
	Point vertex; // An event point. For CircEvent vertex.y() != y (sweep line's)
	MyDouble y;   // sweep line
	bool valid;
};

struct SiteEvent : Event
{
	SiteEvent(): Event()
	{ this->tp = site; }
	SiteEvent(MyDouble y, Point pnt): Event(y, pnt)
	{ this->tp = site; }
	SiteEvent(Locus*);

	Locus* face;
};

struct CircEvent : Event
{
	CircEvent(): Event()
	{	this->tp = circ;	}
	CircEvent(MyDouble y, Point pnt) : Event(y, pnt)
	{	this->tp = circ;	}
	CircEvent(Node*);

	Node* arcnode;
};


// beachline struct //
struct Node
{
	Node(): parent(nullptr), ledge(nullptr), redge(nullptr), left(nullptr), right(nullptr), height(1)
	{	}
	Node(SiteEvent* arc, Edge* ledge = nullptr, Edge* redge = nullptr);
	Node(Locus* face, Edge* ledge = nullptr, Edge* redge = nullptr);
	int get_height();
	int leftH();
	int rightH();
	int corr_height();
	void swap_list_fields(Node*);
	void set_parent(Node*);
	Segment tangent(MyDouble, MyDouble);
	~Node();

	Arc* arc;
	// list structure
	Edge* ledge;
	Edge* redge;
	// tree structure
	Node* left;
	Node* right;
	Node* parent;
	int height;
};


struct AVL
{
	AVL()
	{	}
	AVL(SiteEvent*, myq*);

	Node** get_upper(Node*);
	Node* left_turn(Node*);
	Node* right_turn(Node*);
	void up_balance(Node*);
	void swap_nodes(Node**, Node**);
	void swap_nodes2(Node*, Node*);

	// case of more then one upper points
	void insert_right(Segment &, SiteEvent*);
	void _insert_right(Node*, Segment &, SiteEvent*);
	void insert(SiteEvent*);
	// Create CircEvent s
	void _insert(Node*, SiteEvent*);
	// Create a CircEvent and add points to the diagram
	void del(CircEvent*);
	// Changes only the three structure
	void _del(Node**);

	Node** root;
	myq* events;
};
#endif
