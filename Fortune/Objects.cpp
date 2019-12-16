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

//((x - x1)^2) / (2*(y1 - yd)) + (y1 + yd)/2  =  ((x - x2)^2) / (2*(y2 - yd)) + (y2 + yd)/2
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
MyDouble Arc:: of(MyDouble x, MyDouble sweep)
{
	MyDouble yf = face->centre.y();
	MyDouble xf = face->centre.x();
	if (yf == sweep)
	{
		return sweep;
	}
	return (x - xf)*(x - xf)/(2*(yf - sweep)) + (yf + sweep)/2;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      EDGE      \\\\\\\\\\\\\\\

Edge:: Edge(Segment guide) : inter(nullptr), left(nullptr), right(nullptr)
{
	edge = guide;
}
Edge:: Edge(Arc* left, Arc* right, CircEvent* evt): inter(nullptr), left(nullptr), right(nullptr)
{
	MyDouble sweep = evt->y;
	pair<MyDouble, MyDouble> options = left->intersection(*right, sweep);
	Point pnt;
	if (evt->vertex.x() == options.first)
	{
		pnt = Point(options.first, left->of(options.first, sweep));
	}
	else
	{
		pnt = Point(options.first, left->of(options.first, sweep));
	}
	// a perpendicular line
	Line perp(pnt, Segment(left->face->centre, right->face->centre));
	if (perp.a == 0)
	{
		if (pnt.x() > left->face->centre.x())
		{
			this->edge = Segment(pnt.x(), pnt.y(), pnt.x() + 1.0, pnt.y());
			return;
		}
		else
		{
			this->edge = Segment(pnt.x(), pnt.y(), pnt.x() - 1.0, pnt.y());
			return;
		}
	}
	// a horisontal line
	Line checker(Point(pnt.x(), pnt.y() - 1.0), Point(pnt.x() + 1.0, pnt.y() - 0.1));
	edge = Segment(pnt, perp.intersection(checker));
	edge = edge.normalise();
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
bool Event:: operator <(Node* cur) { return cur->left != nullptr || this->vertex.x() < cur->arc->left_inter(*cur->left->arc, this->y); }
bool Event:: operator >(Node* cur) { return cur->right != nullptr || this->vertex.x() > cur->arc->right_inter(*cur->right->arc, this->y); }
bool Event:: operator==(Node* cur) { return !(*this < cur) && !(*this > cur); }
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
void Node::swap_list_fields(Node* other)
{
	Arc* tmparc = this->arc;
	this->arc = other->arc;
	other->arc = tmparc;

	Edge* tmpedg = this->ledge;
	this->ledge = other->ledge;
	other->ledge = tmpedg;

	tmpedg = this->redge;
	this->redge = other->redge;
	other->redge = tmpedg;
}
// a tangent vector (oriented "to the left") to this->arc in point x
Segment Node:: tangent(MyDouble x, MyDouble sweep)
{
	MyDouble xf = arc->face->centre.x();
	MyDouble yf = arc->face->centre.y();
	if (sweep == yf)
	{
		return Segment(x, sweep, x, sweep - 1.0);
	}
	Point   pnt = Point(x, arc->of(x, sweep));
	// plusing the derivative vector
	Segment ans = Segment(pnt.x(), pnt.y(), pnt.x() + 1.0, pnt.y() + (x - xf) / (yf - sweep));
	return ans.normalise();
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	       AVL	   \\\\\\\\\\\\\\\

AVL:: AVL(SiteEvent* first, myq* events) : events(events)
{
	Node* tmp = new Node(first);
}
// returns parent's pointer to cur or a root pointer if cur is the root
Node** AVL:: get_upper(Node* cur)
{
	if (cur == root) { return &root; }
	else
	{
		if (cur == cur->parent->left) { return &(cur->parent->left); }
		else { return &(cur->parent->right); }
	}
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
	if (!cur) { return; }
	Node** upper = get_upper(cur);

	if (cur->rightH() - cur->leftH() == 2)
	{
		if (cur->right->leftH() > cur->right->rightH())
		{
			cur->right = right_turn(cur->right);
		}
		*upper = left_turn(cur);
	}
	if (cur->leftH() - cur->rightH() == 2)
	{
		if (cur->left->rightH() > cur->left->leftH())
		{
			cur->left = left_turn(cur->left);
		}
		*upper = right_turn(cur);
	}
	(*upper)->height = (*upper)->corr_height();
	up_balance((*upper)->parent);
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
void AVL:: insert(SiteEvent* evt)
{
	_insert(root, evt);
}
void AVL:: _insert(Node* cur, SiteEvent* evt)
{
	// вставка: содание двух новых узлов вместо старого
	// исходя из свойств beachline кревой узел найдётся
	if (*evt < cur)
	{
		_insert(cur->left, evt);
	}
	else if (*evt > cur)
	{
		_insert(cur->right, evt);
	}
	else
	{
		// касательный вектор
		Segment vr = cur->tangent(evt->vertex.x(), evt->y);
		Segment vl = -vr;
		// may be need to swap
		// list
		Edge* el = new Edge(vl);
		Edge* er = new Edge(vr);
		// *cur = al						// left part of the divided edge
		Node* ao = new Node(evt, el, er);	// new (central) edge
		Node* ar = new Node(cur->arc->face, er, cur->redge);  // right part of the divided edge
		if (cur->redge != nullptr)
		{
			cur->redge->left = ar;
		}
		cur->redge = el;
		el->left = cur;
		el->right = ao;
		er->left = ao;
		er->right = ar;
		// tree
		if (ar->redge == nullptr) // used to be cur->redge
			// means that cur used to be the rightest arc
		{
			cur->right = ao;
			ao->parent = cur;
			up_balance(ao);    // ao is still a leaf

			ao->right = ar;
			ar->parent = ao;
			up_balance(ar);
		}
		else
			// means that ar->redge->right is not empty
		{
			Node* tmp = ar->redge->right;
			tmp->left = ar;
			ar->parent = tmp;
			up_balance(ar);

			ar->left = ao;
			ao->parent = ar;
			up_balance(ao);
		}
		ADD CircEvent HERE
	}
}
void AVL:: del(CircEvent* evt)
{
	/* according to the beachline and Events structure, 
	   cur->ledge != 0 && cur->redge != 0 */
	Node*  cur = evt->arcnode;
	Node** upper = get_upper(cur);
	Edge*  nedge = new Edge(cur->ledge->left->arc, cur->redge->right->arc, evt);
	// list
	cur->ledge->left->redge = nedge;
	nedge->left = cur->ledge->left;
	cur->redge->right->ledge = nedge;
	nedge->right = cur->redge->right;

	ADD POINT HERE
	ADD CircEvent HERE
	//tree
	_del(upper);
}
void AVL:: _del(Node** cur)
{
	Node* tmp = *cur;
	if ((*cur)->left == nullptr && (*cur)->right == nullptr)
	{
		*cur = nullptr;
		up_balance(tmp->parent);
		delete tmp;
	}
	else if ((*cur)->left != nullptr && (*cur)->right == nullptr)
	{
		(*cur)->left->parent = (*cur)->parent;
		(*cur) = (*cur)->left; // меняем указатель отца cur
		delete tmp;
		up_balance(*cur);
	}
	else if ((*cur)->left == nullptr && (*cur)->right != nullptr)
	{
		(*cur)->right->parent = (*cur)->parent;
		(*cur) = (*cur)->right; // меняем указатель отца cur
		delete tmp;
		up_balance(*cur);
	}
	else
	{
		// (*cur)->right != 0 so leftv is placed in the subtree of cur
		Node* leftv = (*cur)->redge->right; // left node from *cur
		(*cur)->swap_list_fields(leftv);
		Node** tmpup = get_upper(leftv);
		// leftv doesn't have the left child, so case 3 will rise
		_del(tmpup); // makes up_balance
	}
}
///////////////	############### \\\\\\\\\\\\\\\
