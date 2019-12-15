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

// компаратор для приоритетной очереди
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

	bool operator <(Node* parab);
	bool operator >(Node* parab);
	bool operator==(Node* parab);



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
	Node(): parent(nullptr), ledge(nullptr), redge(nullptr), left(nullptr), right(nullptr), height(1)
	{	}
	Node(SiteEvent* arc, Edge* ledge = nullptr, Edge* redge = nullptr);
	Node(Locus* face, Edge* ledge = nullptr, Edge* redge = nullptr);
	void rebind(CircEvent*);
	int get_height();
	int leftH();
	int rightH();
	int corr_height();
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


//переписать через parent
struct AVL
{
	AVL()
	{	}
	AVL(SiteEvent* first, myq*);


	Node* left_turn(Node*);
	Node* right_turn(Node*);
	void balance(Node**);
	void up_balance(Node*);

	void insert_right(Segment &, SiteEvent*);
	void _insert_right(Node**, Segment &, SiteEvent*);
	void insert(SiteEvent* evt)
	{
		_insert(&root, evt);
	}
	// Должен создавать CircEvent
	void _insert(Node** cur, SiteEvent* evt)
	{
		// вставка: содание двух новых узлов вместо старого
		// исходя из свойств beachline кревой узел найдётся
		if (*evt < *cur)
		{
			_insert(&((*cur)->left), evt);
		}
		else if (*evt > *cur)
		{
			_insert(&((*cur)->right), evt);
		}
		// ADD HERE
		else
		{
			// касательный вектор
			Segment v1 = (*cur)->tanget(evt->vertex.x());
			Segment v2 = -v1;
			// may be need to swap
			// list
			Edge* el = new Edge(v1);
			Edge* er = new Edge(v2);
			// *cur = al						// left part of the divided edge
			Node* ao = new Node(evt, el, er);	// new (central) edge
			Node* ar = new Node((*cur)->arc->face, er, (*cur)->redge);  // right part of the divided edge
			if ((*cur)->redge != nullptr)
			{
				(*cur)->redge->left = ar;
			}
			(*cur)->redge = el;
			el->left = (*cur);
			el->right = ao;
			er->left = ao;
			er->right = ar;
			// tree
			if ((*cur)->left == nullptr) // used to be (*cur)->redge
				// means that cur used to be the rightest arc
			{
				(*cur)->right = ao;
				ao->parent = *cur;
				up_balance(ao);    // ao is still a leaf
				ao->right = ar;
				ar->parent = ao;
				up_balance(ar);
			}
			else
				// means that ar->redge->right is not empty
			{
				Node* tmp = ar->redge->right;
				tmp->left = ao;
				ao->parent = tmp;
				up_balance(ao);
				ao->right = ar;
				ar->parent = ao;
				up_balance(ar);
			}
			ADD EVENT HERE
		}
	}

	// в CircEvent есть указатель на узел, но нужно делать рекурсивно для балансировки
	void del(CircEvent* evt)
	{
		find(&root, evt);
	}
	void find(Node** cur, CircEvent* evt)
	{
		if (*cur == nullptr)
		{
			return;
		}
		// DEL HERE
		else if (*cur == evt->arcnode)
		{

		}
		else if (*evt < *cur)
		{
			find(&((*cur)->left), evt);
			balance(cur);
		}
		else if (*evt > *cur)
		{
			find(&((*cur)->right), evt);
			balance(cur);
		}
		// в случае если сразу несколько дуг схлопываются, нужно проверять всех потомков
		else
		{
			find(&((*cur)->left), evt);
			find((&(*cur)->right), evt);
			balance(cur);
		}
	}

	void __del__(Node** cur)
	{
		//изменение
		if ((*cur)->left && (*cur)->right)
		{
			get_left(&((*cur)->right), &((*cur)->key));
			balance(cur);
			return;
		}
		// меняются рёбра
		if (!(*cur)->left && (*cur)->right)
		{
			Node* tmp = *cur;
			*cur = (*cur)->right;
			delete tmp;
			(*cur)->height = (*cur)->corr_height();
			return;
		}
		if ((*cur)->left && !(*cur)->right)
		{
			Node* tmp = *cur;
			*cur = (*cur)->left;
			delete tmp;
			(*cur)->height = (*cur)->corr_height();
			return;
		}
		if (!(*cur)->left && !(*cur)->right)
		{
			Node* tmp = *cur;
			*cur = 0;
			delete tmp;
			return;
		}
	}
	// через swap
	void get_left(Node** cur, T* write_to)
	{
		if (!(*cur)->left)
		{
			*write_to = (*cur)->key;
			__del__(cur);
			balance(cur);
			return;
		}
		get_left(&((*cur)->left), write_to);
		balance(cur);
	}


	Node* root;
	myq* events;
};


#endif
