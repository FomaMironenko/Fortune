#include "pch.h"
#include <iostream>

#include "primitives.h"
#include "objects.h"


//сначала будет идти ev2
bool Comp:: operator() (Event* ev1, Event* ev2) { return *ev1 > *ev2; }

///////////////		PARABOLA	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////	      Arc       \\\\\\\\\\\\\\\

Arc:: Arc(SiteEvent* ev)
{
	face = ev->face;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      EDGE      \\\\\\\\\\\\\\\

Edge:: Edge(Segment guide) : inter(nullptr)
{
	edge = guide;
}
Edge:: Edge(Arc* left, Arc* right)
{

}
void Edge::unvalidate()
{
	if (inter != nullptr)
	{
		inter->valid = false;
	}
}
Edge:: ~Edge()
{
	unvalidate();
}

///////////////	############### \\\\\\\\\\\\\\\




///////////////		  LOCUS	    \\\\\\\\\\\\\\\

void Locus:: add(Point pnt)
{
	vertexes.push_back(pnt);
}
void Locus::print()
{
	centre.print(true);
	cout << ":  \t";
	for (auto it = vertexes.begin(); it != vertexes.end(); it++)
	{
		it->print(true);
	}
	cout << endl;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////     Diagram	    \\\\\\\\\\\\\\\

Diagram:: Diagram(int n): n(n)
{
	faces = vector<Locus*>(n);
	for (int i = 0; i < n; i++)
	{
		faces[i] = new Locus;
		faces[i]->centre.get();
	}
}
void Diagram:: get(myq & Q)
{
	Point tmp;
	while (cin >> tmp)
	{
		Locus* face = new Locus;
		face->centre = tmp;
		faces.push_back(face);  //copies fields
		SiteEvent* nsite = new SiteEvent(tmp.y(), tmp);
		nsite->face = face;
		Q.push(nsite);
		n++;
	}
	cin.clear();
}
void Diagram:: print()
{
	for (auto it = faces.begin(); it < faces.end(); it++)
	{
		(*it)->print();
		cout << endl;
	}
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////		  EVENT	    \\\\\\\\\\\\\\\

bool Event:: operator==(Event & other) { return this->y == other.y && this->vertex == other.vertex; }
bool Event:: operator <(Event & other) { return this->y > other.y || this->y == other.y && this->vertex < other.vertex; }
bool Event:: operator >(Event & other) { return this->y < other.y || this->y == other.y && this->vertex > other.vertex; }
bool Event:: operator>=(Event & other) { return !(*this < other); }
bool Event:: operator<=(Event & other) { return !(*this > other); }
///////////////	############### \\\\\\\\\\\\\\\




///////////////	  SITE EVENT	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////	  CIRC EVENT	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////	      NODE     	\\\\\\\\\\\\\\\


Node::Node(SiteEvent* evt, Node* father, Edge* ledge, Edge* redge):
	ledge(ledge), redge(redge), left(nullptr), right(nullptr), father(father)
{
	Arc* tmp = new Arc(evt);
}
Node:: ~Node()
{
	delete arc;
	delete ledge;
	delete redge;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	       AVL	   \\\\\\\\\\\\\\\

AVL:: AVL(SiteEvent* first, myq* events) : events(events)
{
	Node* tmp = new Node();
}
void AVL::balance(Node* cur)
{

}
Node* AVL::find(MyDouble x)
{

}
void AVL:: insert(Edge* edge, SiteEvent* evt)
{

}
void AVL:: insert(SiteEvent* evt)
{

}
void AVL:: del(CircEvent* evt)
{
	Node* cur = evt->arcnode;

	Node* leftnode = cur->ledge->left;
	Node* rightnode = cur->redge->right;
	Edge* newedge = new Edge(leftnode->arc, rightnode->arc);
	newedge->left = leftnode;
	newedge->right = rightnode;
	leftnode->redge = newedge;
	rightnode->ledge = newedge;

	CircEvent* newevt = new CircEvent(leftnode);
	if (newevt != nullptr)
	{
		events->push(newevt);
	}
	else
	{
		delete newevt;
	}
	newevt = new CircEvent(rightnode);
	if (newevt != nullptr)
	{
		events->push(newevt);
	}
	else
	{
		delete newevt;
	}

	cur->arc->face->add(evt->vertex);
	
	_del(evt);
}
///////////////	############### \\\\\\\\\\\\\\\
