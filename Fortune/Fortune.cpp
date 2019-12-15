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
	Beachline(myq* evts, SiteEvent* first): events(evts)
	{	
		tree = AVL(first, events);
	}

	// несколько верхних точек
	void preprocess(Point prev, SiteEvent* evt)
	{
		Point cur = evt->face->centre;
		// prev.y() == cur.y() == evt->y
		Segment tmp((prev.x() + cur.x()) / 2, prev.y(), (prev.x() + cur.x()) / 2, prev.y() - 1.0);
		tree.insert_right(tmp, evt);
	}

	void process_site(SiteEvent* evt)
	{
		tree.insert(evt);
	}

	void process_circ(CircEvent* evt)
	{
		tree.del(evt);
	}


	AVL tree;
	myq* events;
};



// Fortune algorithm
void set_diagram()
{
	Diagram voronoy;
	Comp cmp;
	myq events;
	//adds SiteEvents
	voronoy.get(events);

	Event *cur, *tmp;
	cur = events.top();
	events.pop();
	Beachline front(&events, (SiteEvent*)cur);
	tmp = cur;
	// пока что все события - SiteEvent для точек
	// обработка случая: несколько верхних точек
	while (!events.empty() && (cur = (SiteEvent*)events.top())->y == tmp->y)
	{
		// tmp->vertex - предыдущая вершина
		front.preprocess(tmp->vertex, (SiteEvent*)cur);
		events.pop();
		delete tmp;
		tmp = cur;
	}
	delete tmp;

	while (!events.empty())
	{
		cur = events.top();
		events.pop();
		if (cur->valid)
		{
			if (cur->tp == site)
			{
				front.process_site((SiteEvent*)cur);
			}
			else
			{
				front.process_circ((CircEvent*)cur);
			}
		}
		delete cur;
	}

	cout << "\n";
	voronoy.print();
}



int main()
{
	//set_diagram();

	// CHECK THIS OUT
	//Point a(1, 2);
	//Point b(3, 4);
	//Segment s1(a, b);
	//Segment s2(0, 0, 1.1, 2);
	//cout << s1.cross(s2);
}

/*
1) При добавлении SiteEvent события если х координата точки совпадает с
х координатой пересечения каих-то двух параболических дуг,
то эта точка пересечения - новая вершина диаграмы. Можно не выделять этот 
случай в отдельный

2) Направление нового ребра (2х рёбер) - касательная к
параболе в точке дуги с х координатой новой точки 
Segment(point, dir)

3) События-удаления вычисляются во время события-вставки. y координата события-
удаления = у координата точки пересечения(новой вершины диаграммы)


*/
