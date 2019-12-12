#include "pch.h"
#include <iostream>

#include "primitives.h"
#include "objects.h"


//сначала будет идти ev2
bool Comp:: operator() (Event* ev1, Event* ev2) { return *ev1 > *ev2; }
bool BComp::operator() (BL* bl1, BL* bl2) { return *bl1 < *bl2; }

///////////////		PARABOLA	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////		  BL	   \\\\\\\\\\\\\\\

bool BL::operator <(BL & other)
{
	return this->x < other.x;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      EDGE      \\\\\\\\\\\\\\\

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
		NewSite* nsite = new NewSite(tmp.y(), tmp);
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




///////////////		NEW SITE	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////		ARC INTER	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\
