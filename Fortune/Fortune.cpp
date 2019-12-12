#include "pch.h"
#include <iostream>
#include <queue>
#include <set>

#include "primitives.h"
#include "objects.h"


struct Beachline
{
	Beachline()
	{	}



	set<BL*> chain;
};



// Fortune algorithm
void set_diagram()
{
	Diagram voronoy;
	Comp cmp;
	myq events;
	voronoy.get(events);

	Beachline front;


	cout << "\n";
	voronoy.print();
}



int main()
{
	//set_diagram();
	set<BL*, BComp> Q;
	Arc* t1 = new Arc(4.5);
	Arc* t2 = new Arc(2.6);
	Arc* t3 = new Arc(7);
	Q.insert(t1);
	Q.insert(t2);
	Q.insert(t3);
	for (auto it = Q.begin(); it != Q.end(); it++)
	{
		if ((*it)->tp == arc)
		{
			cout << (*it)->x << " ";
		}
	}


	// CHECK THIS OUT
	//Point a(1, 2);
	//Point b(3, 4);
	//Segment s1(a, b);
	//Segment s2(0, 0, 1.1, 2);
	//cout << s1.cross(s2);
}

/*
1) При добавлении NewSite события если х координата точки совпадает с
х координатой пересечения каих-то двух параболических дуг,
то эта точка пересечения - новая вершина диаграмы. Можно не выделять этот 
случай в отдельный

2) Направление нового ребра (2х рёбер) - касательная к
параболе в точке дуги с х координатой новой точки 
Segment(point, dir)

3) События-удаления вычисляются во время события-вставки. y координата события-
удаления = у координата точки пересечения(новой вершины диаграммы)


*/
