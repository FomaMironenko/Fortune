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


struct Arc;
struct Edge;
struct Locus;
struct Diagram;
struct Event;
struct SiteEvent;
struct CircEvent;
struct Node;
struct AVL;

// компаратор для приоритетной очереди
struct Comp
{
	bool operator()(Event*, Event*);
};
typedef priority_queue<Event*, vector<Event*>, Comp> myq;



// Components of beachline //
// arc with parabola interface
struct Arc
{
	Arc()
	{    }
	Arc(SiteEvent* ev);

	Locus* face;
};

struct Edge
{
	Edge(): inter(nullptr)
	{	 }
	Edge(Segment guide);
	//Edge(Arc* left, Arc* right);

	// сначала удаляюися рёбра - потом соотвествующее им событие
	void unvalidate();
	~Edge();

	Segment edge;
	//!! случай если пересечений несколько
	// пересечение с другим ребром
	CircEvent* inter;
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


	type tp;
	Point vertex; // точка где происходит событие. Для CircEvent её координаты могут  отличаться от координаты sweepline
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


	Node* arcnode;
};


// beachline struct //
struct Node
{
	Node(): ledge(nullptr), redge(nullptr), left(nullptr), right(nullptr), father(nullptr)
	{	}
	Node(SiteEvent* arc, Node* father = nullptr, Edge* ledge = nullptr, Edge* redge = nullptr);

	~Node();
	Arc* arc;
	// list structure
	Edge* ledge;
	Edge* redge;
	// tree structure
	Node* left;
	Node* right;
	Node* father;
	int rw;
	int lw;

	myq* events;
};

struct AVL
{
	AVL()
	{	}
	AVL(SiteEvent* first, myq* events);

	void balance(Node* cur);
	Node* find(MyDouble x);
	// для Beachline:: preprocess. Вставляет дугу как самый правый узел
	void insert(Edge* edge, SiteEvent* evt);
	// должен созавать два ребра и добавлять в очередь события их пересечения с соседними
	// создаёт CircEvent
	void insert(SiteEvent* evt);
	void del(CircEvent* evt);

	Node* root;
	myq* events;
};


#endif
