#include "pch.h"
#include <iostream>
#include <queue>
#include <set>
#include <system_error>

#include "primitives.h"
#include "objects.h"



struct Beachline
{
	Beachline()
	{	}
	Beachline(myq* evts, SiteEvent* first): events(evts)
	{	
		tree = new AVL(first, events);
	}

	// несколько верхних точек
	void preprocess(Point prev, SiteEvent* evt)
	{
		Point cur = evt->face->centre;
		// prev.y() == cur.y() == evt->y
		Segment tmp((prev.x() + cur.x()) / 2.0, prev.y(), (prev.x() + cur.x()) / 2.0, prev.y() - 1.0);
		tree->insert_right(tmp, evt);
	}

	void process_site(SiteEvent* evt)
	{
		tree->insert(evt);
	}

	void process_circ(CircEvent* evt)
	{
		tree->del(evt);
	}

	void post_process()
	{
		tree->post_process();
	}

	~Beachline()
	{
		delete tree;
	}


	AVL* tree;
	myq* events;
};



// Fortune algorithm
void set_diagram(Diagram & voronoy)
{
	Comp cmp;
	myq events;
	//adds SiteEvents
	voronoy.get(events);

	Event *cur, *tmp;
	cur = events.top();
	events.pop();
	Beachline* beachline = new Beachline(&events, (SiteEvent*)cur);
	tmp = cur;
	// by now all of the events are SiteEvents
	// processing case of multi upper points
	while (!events.empty() && (cur = (SiteEvent*)events.top())->y == tmp->y)
	{
		// tmp->vertex - previous vertex
		beachline->preprocess(tmp->vertex, (SiteEvent*)cur);
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
			//cur->vertex.print();
			if (cur->tp == site)
			{
				beachline->process_site((SiteEvent*)cur);
			}
			else
			{
				beachline->process_circ((CircEvent*)cur);
			}
		}
		delete cur;
	}

	beachline->post_process();
	delete beachline;
}



int main()
{
	Diagram voronoy;
	set_diagram(voronoy);
	voronoy.print();

	return EXIT_SUCCESS;
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


1 0
3 0
4 0
5 0
8 0
2 -2

0 0
2 2
-2 2
0 4

0 0
4 4
-4 4
0 2

0 0
-2 0
2 0
1 1
-1 1

0 0
2 2
-2 2
0 3
0 5

8 3
5 6
0 2
0 5
7 1
-3 -2
1 4
3 3
-5 2

2 3
1 4
0 -1
-3 -5
0 0
*/
