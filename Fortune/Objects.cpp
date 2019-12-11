#include "pch.h"
#include <iostream>

#include "primitives.h"
#include "objects.h"


///////////////		PARABOLA	\\\\\\\\\\\\\\\

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

Diagram:: Diagram(int n)
{
	faces = vector<Locus>(n);
	for (int i = 0; i < n; i++)
	{
		faces[i].centre.get();
	}
}
void Diagram:: get(myq & Q)
{
	Point tmp;
	Locus face;
	NewSite* Nsite;
	int n = 0;
	while (cin >> tmp)
	{
		face.centre = tmp;
		//copies fields
		faces.push_back(face);
		Nsite = new NewSite(tmp.x());
		Q.push();
	}
	cin.clear();
	this->n = n;
}
void Diagram:: print()
{
	for (auto it = faces.begin(); it < faces.end(); it++)
	{
		it->print();
	}
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////		  EVENT	    \\\\\\\\\\\\\\\

bool Event:: operator<(Event & other) { return this->y > other.y; }
bool Event:: operator>(Event & other) { return this->y < other.y; }
///////////////	############### \\\\\\\\\\\\\\\




///////////////		NEW SITE	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////		ARC INTER	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\