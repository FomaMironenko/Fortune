#include "pch.h"
#include <iostream>
#include <queue>
#include <set>

#include "primitives.h"
#include "objects.h"


struct Beachline
{


	set<Event> chain;
};



// Fortune algorithm
void set_diagram()
{
	Diagram voronoy;
	Comp cmp;
	myq events;
	voronoy.get(events);

	
}



int main()
{
	Comp cmp;
	priority_queue<Event*, vector<Event*>, Comp> Q;
	NewSite* a = new NewSite(5);
	a->vertex = Point(5, 5);
	Q.push(a);
	Event* tmp = Q.top();
	if (tmp->tp == site)
	{
		cout << "hi";
		((NewSite*)tmp)->vertex.print();
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
