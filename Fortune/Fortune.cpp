#include "pch.h"

#include "primitives.h"
#include "objects.h"
#include <GL/glut.h>


struct Beachline
{
	Beachline()
	{	}
	Beachline(myq* evts, SiteEvent* first) : events(evts)
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
void set_diagram(Diagram & voronoy, myq & events)
{
	//adds SiteEvents
	//voronoy.get(events);

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





Diagram voronoy;
myq events;

const float windowSize = 600.0;
int X = -20;
int Y = -20;





//////////// Graphics ////////////

void Initialize()
{
	glClearColor(0, 0, 0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, windowSize, windowSize, 0.0, 0.0, 1.0);
	glFlush();
}

void renderBitmapString(float x, float y, float z, void *font, const char *string)
{

	const char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

void set_head()
{
	glColor3f(1, 1, 1);
	glRectf(0, 0, windowSize, 50);
	// frame
	glColor3f(0.8, 0.8, 0.8);
	glRectf(0, 0, windowSize, 5);
	glRectf(0, 0, 5, 50);
	glRectf(0, 45, windowSize, 50);
	glRectf(windowSize - 5, 0, windowSize, 50);

	glColor3f(0, 0, 0);
	renderBitmapString(150, 30, 0, GLUT_BITMAP_HELVETICA_18, "Click. When finish, press 'Enter'");
}

// Process the diagram when 'enter' key pressed
void ext(unsigned char key, int x, int y)
{
	if ((int)key == 13)
	{
		set_diagram(voronoy, events);
		voronoy.print();
		for (auto it = voronoy.faces.begin(); it < voronoy.faces.end(); it++)
		{
			Polygon tmp;
			tmp.convex_hull((*it)->vertexes);
			glColor3f(0.4, 0, 0.9);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < tmp.n; i++)
			{
				//if ((abs(tmp.points[i].x()) > 8000 || abs(tmp.points[i].y()) > 8000) &&
				//	((abs(tmp.points[(i + 1) % tmp.n].x()) > 8000 || abs(tmp.points[(i + 1) % tmp.n].y()) > 8000)))
				//{
				//	continue;
				//}
				//glVertex3d(tmp.points[i].x(), tmp.points[i].y(), 0);
				//glVertex3d(tmp.points[(i + 1) % tmp.n].x(), tmp.points[(i + 1) % tmp.n].y(), 0);
				glVertex3d(tmp.points[i].x(), tmp.points[i].y(), 0);
			}
			glEnd();
		}
		set_head();

		glFlush();
	}
}

void get_points(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		X = x;
		Y = y;
	}
	glutPostRedisplay();
}

void Draw()
{
	// first point: (-20, -20)
	if (X < 0 || Y < 0)
	{
		set_head();
		glFlush();
	}
	if (Y <= 50)
	{
		return;
	}

	glColor3f(0.4, 0, 0.9);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3i(X, Y, 0);
	glEnd();

	Point pnt(X, Y);
	if (X < 0 || Y < 0)
	{
		return;
	}
	if (voronoy.faces.empty() || pnt != voronoy.faces.back()->centre)
	{
		voronoy.add(events, pnt);
	}

	glFlush();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(windowSize, windowSize);		//Указываем размер окна
	glutInitWindowPosition(100, 100);	//Позиция окна
	glutCreateWindow("Fortune");

	Initialize();
	glutDisplayFunc(Draw);
	glutKeyboardFunc(ext);
	glutMouseFunc(get_points);

	glutMainLoop();

	//Diagram voronoy;
	//set_diagram(voronoy);
	//voronoy.print();

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
