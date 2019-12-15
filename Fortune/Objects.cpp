#include "pch.h"
#include <iostream>

#include "primitives.h"
#include "objects.h"


//сначала будет идти ev2
bool Comp:: operator() (Event* ev1, Event* ev2) { return *ev1 > *ev2; }



pair<MyDouble, MyDouble> solve_square(MyDouble a, MyDouble b, MyDouble c)
{
	if (a == 0)
	{
		// not considered: b == 0
		return pair<MyDouble, MyDouble>(-c/b, -c/b);
	}
	return pair<MyDouble, MyDouble>((-b - sqrt(b*b - 4*a*c))/2, (-b + sqrt(b*b - 4*a*c))/2);
}


///////////////	      Arc       \\\\\\\\\\\\\\\

Arc:: Arc(SiteEvent* ev)
{
	face = ev->face;
}
Arc:: Arc(Locus* face)
{
	this->face = face;
}
pair<MyDouble, MyDouble> Arc::intersection(Arc & parab, MyDouble sweep)
{
	Point pnt1 = this->face->centre;
	Point pnt2 = parab.face->centre;
	if (pnt1.y() == sweep)
	{
		return pair<MyDouble, MyDouble>(pnt1.x(), pnt1.x());
	}
	if (pnt2.y() == sweep)
	{
		return pair<MyDouble, MyDouble>(pnt2.x(), pnt2.x());
	}
	MyDouble a = (pnt2.y() - pnt1.y()) / ((pnt1.y() - sweep)*(pnt2.y() - sweep));
	MyDouble b = 2 * (pnt2.x() / (pnt2.y() - sweep) - pnt1.x() / (pnt1.y() - sweep));
	MyDouble c = pnt1.y() - pnt2.y() + pnt1.x()*pnt1.x() / (pnt1.y() - sweep) - pnt2.x()*pnt2.x() / (pnt2.y() - sweep);
	pair<MyDouble, MyDouble> tmp = solve_square(a, b, c);
	return tmp;
}
MyDouble Arc:: left_inter(Arc & parab, MyDouble sweep)
{
	pair<MyDouble, MyDouble> tmp = this->intersection(parab, sweep);
	return tmp.first;
}
MyDouble Arc:: right_inter(Arc & parab, MyDouble sweep)
{
	pair<MyDouble, MyDouble> tmp = this->intersection(parab, sweep);
	return tmp.second;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      EDGE      \\\\\\\\\\\\\\\

Edge:: Edge(Segment guide) : inter(nullptr), left(nullptr), right(nullptr)
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
// check case when cur is the ultimate arc
bool Event:: operator <(Node* cur)
{
	return cur->left != nullptr || this->vertex.x() < cur->arc->left_inter(*cur->left->arc, this->y);
}
bool Event:: operator >(Node* cur)
{
	return cur->right != nullptr || this->vertex.x() > cur->arc->right_inter(*cur->right->arc, this->y);
}
bool Event:: operator==(Node* cur)
{
	return !(*this < cur) && !(*this > cur);
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	  SITE EVENT	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////	  CIRC EVENT	\\\\\\\\\\\\\\\

///////////////	############### \\\\\\\\\\\\\\\




///////////////	      NODE     	\\\\\\\\\\\\\\\


Node::Node(SiteEvent* evt, Edge* ledge, Edge* redge):
	ledge(ledge), redge(redge), left(nullptr), right(nullptr), parent(nullptr), height(1)
{
	Arc* tmp = new Arc(evt);
}
Node::Node(Locus* face, Edge* ledge, Edge* redge) :
	ledge(ledge), redge(redge), left(nullptr), right(nullptr), parent(nullptr), height(1)
{
	Arc* tmp = new Arc(face);
}
Node:: ~Node()
{
	delete arc;
	delete ledge;
	delete redge;
}
int Node:: get_height()
{
	return this != NULL ? this->height : 0;
}
int Node:: leftH()
{
	return this->left->get_height();
}
int Node:: rightH()
{
	return this->right->get_height();
}
int Node:: corr_height()
{
	return this->height = 1 + ::max(this->left->get_height(), this->right->get_height());
}

void Node:: rebind(CircEvent* evt)
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
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	       AVL	   \\\\\\\\\\\\\\\

AVL:: AVL(SiteEvent* first, myq* events) : events(events)
{
	Node* tmp = new Node(first);
}
Node* AVL:: left_turn(Node* cur)
{
	//cur->right - cur->left = 2
	Node* tmp = cur->right;
	tmp->parent = cur->parent;
	cur->parent = tmp;
	if (tmp->left != nullptr)
	{
		tmp->left->parent = cur;
	}
	cur->right = tmp->left;
	tmp->left = cur;
	cur->height = cur->corr_height();
	tmp->height = tmp->corr_height();
	return tmp;
}
Node* AVL:: right_turn(Node* cur)
{
	//cur->right - cur->left = -2
	Node* tmp = cur->left;
	tmp->parent = cur->parent;
	cur->parent = tmp;
	if (tmp->right != nullptr)
	{
		tmp->right->parent = cur;
	}
	cur->left = tmp->right;
	tmp->right = cur;
	cur->height = cur->corr_height();
	tmp->height = tmp->corr_height();
	return tmp;
}
void AVL:: balance(Node** cur)
{
	//left turn
	if (!*cur)
		return;
	if ((*cur)->rightH() - (*cur)->leftH() == 2)
	{
		if ((*cur)->right->leftH() > (*cur)->right->rightH())
		{
			(*cur)->right = right_turn((*cur)->right);
		}
		(*cur) = left_turn(*cur);
	}

	if ((*cur)->leftH() - (*cur)->rightH() == 2)
	{
		if ((*cur)->left->rightH() > (*cur)->left->leftH())
		{
			(*cur)->left = left_turn((*cur)->left);
		}
		(*cur) = right_turn(*cur);
	}
	(*cur)->height = (*cur)->corr_height();
}
void AVL:: up_balance(Node* cur)
{
	if (!cur)
	{
		return;
	}
	Node** tmp;
	if (cur == root)
	{
		*tmp = root;
	}
	else
	{
		if (cur == cur->parent->left) { tmp = &(cur->parent->left); }
		else { tmp = &(cur->parent->right); }
	}

	if (cur->rightH() - cur->leftH() == 2)
	{
		if (cur->right->leftH() > cur->right->rightH())
		{
			cur->right = right_turn(cur->right);
		}
		*tmp = left_turn(cur);
	}
	if (cur->leftH() - cur->rightH() == 2)
	{
		if (cur->left->rightH() > cur->left->leftH())
		{
			cur->left = left_turn(cur->left);
		}
		*tmp = right_turn(cur);
	}
	(*tmp)->height = (*tmp)->corr_height();
	up_balance((*tmp)->parent);
}
void AVL:: insert_right(Segment & edg, SiteEvent* evt)
{
	_insert_right(&root, edg, evt);
}
void AVL:: _insert_right(Node** cur, Segment & edg, SiteEvent* evt)
{
	if (!(*cur)->right)
	{
		// tree
		Edge* edge = new Edge(edg);
		(*cur)->right = new Node(evt);
		(*cur)->right->parent = *cur;
		// list
		(*cur)->redge = edge;
		(*cur)->right->ledge = edge;
		edge->left = *cur;
		edge->right = (*cur)->right;
	}
	else
	{
		_insert_right(&((*cur)->right), edg, evt);
	}
	balance(cur);
}
///////////////	############### \\\\\\\\\\\\\\\
