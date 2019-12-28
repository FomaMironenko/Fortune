#include "pch.h"
#include "primitives.h"
#include "objects.h"


// ev2 first
bool Comp:: operator() (Event* ev1, Event* ev2) { return *ev1 > *ev2; }



pair<MyDouble, MyDouble> solve_square(MyDouble a, MyDouble b, MyDouble c)
{
	if (a == NILL)
	{
		// not considered: b == 0
		return pair<MyDouble, MyDouble>(-c / b, -c / b);
	}
	return pair<MyDouble, MyDouble>((-b - sqrt(b*b - 4 * a*c)) / (2 * a), (-b + sqrt(b*b - 4 * a*c)) / (2 * a));
}


///////////////	      Arc       \\\\\\\\\\\\\\\

Arc::Arc(SiteEvent* ev)
{
	face = ev->face;
}
Arc::Arc(Locus* face)
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
MyDouble Arc::left_inter(Arc & parab, MyDouble sweep)
{
	pair<MyDouble, MyDouble> tmp = this->intersection(parab, sweep);
	return tmp.first;
}
MyDouble Arc::right_inter(Arc & parab, MyDouble sweep)
{
	pair<MyDouble, MyDouble> tmp = this->intersection(parab, sweep);
	return tmp.second;
}
MyDouble Arc::of(MyDouble x, MyDouble sweep)
{
	MyDouble yf = face->centre.y();
	MyDouble xf = face->centre.x();
	if (yf == sweep)
	{
		return sweep;
	}
	return (x - xf)*(x - xf) / (2 * (yf - sweep)) + (yf + sweep) / 2.0;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      EDGE      \\\\\\\\\\\\\\\

Edge::Edge(Segment guide) : inters(vector<CircEvent*>(0)), left(nullptr), right(nullptr)
{
	edge = guide;
}
Edge::Edge(Arc* left, Arc* right, CircEvent* evt) : inters(vector<CircEvent*>(0)), left(nullptr), right(nullptr)
{
	MyDouble sweep = evt->y;
	Point pnt;
	// they can't lay on the sweep both
	// evt->vertex is actually an intersection point
	if (left->face->centre.y() != sweep)
	{
		pnt = Point(evt->vertex.x(), left->of(evt->vertex.x(), sweep));
	}
	else
	{
		pnt = Point(evt->vertex.x(), right->of(evt->vertex.x(), sweep));
	}
	// a perpendicular line
	Line perp(pnt, Segment(left->face->centre, right->face->centre));
	if (perp.a == NILL)
		// case when points corresponding to *left and *right lays on same vertical
	{
		if (pnt.x() > evt->arcnode->arc->face->centre.x())
			// it's obveous after drawing a picture
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
	Line checker(Point(pnt.x(), pnt.y() - 1.0), Point(pnt.x() + 1.0, pnt.y() - 1.0));
	edge = Segment(pnt, perp.intersection(checker));
}
void Edge::unvalidate()
{
	for (auto it = inters.begin(); it < inters.end(); it++)
	{
		(*it)->valid = false;
	}
}
void Edge::reset_event(Node* oevt, Node* nevt)
{
	for (auto it = inters.begin(); it < inters.end(); it++)
	{
		if ((*it)->arcnode == oevt)
		{
			(*it)->arcnode = nevt;
		}
	}
}
Edge:: ~Edge()
{
	unvalidate();
}

///////////////	############### \\\\\\\\\\\\\\\




///////////////		  LOCUS	    \\\\\\\\\\\\\\\

void Locus::add(Point pnt)
{
	// the only case when same point can be added more than
	// once is when processing a CircEvent for more than
	// three points. So it's enough to check just only the 
	// last point in vertexes for coinsiding
	if (vertexes.size() == 0 || vertexes.back() != pnt)
	{
		vertexes.push_back(pnt);
	}
}
void Locus::print()
{
	centre.print(true);
	cout << ":  \t";
	for (auto it = vertexes.begin(); it != vertexes.end(); it++)
	{
		it->print(true);
		cout << " ";
	}
	cout << endl;
}
Locus:: ~Locus()
{
	vertexes.clear();
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////     Diagram	    \\\\\\\\\\\\\\\

Diagram::Diagram(int n) : n(n)
{
	faces = vector<Locus*>(n);
	for (int i = 0; i < n; i++)
	{
		faces[i] = new Locus;
		faces[i]->centre.get();
	}
}
void Diagram::get(myq & Q)
{
	cout << "Enter the points. In the end of input enter ctrl+z\n\n";
	Point tmp;
	while (cin >> tmp)
	{
		Locus* face = new Locus;
		face->centre = tmp;
		faces.push_back(face);
		SiteEvent* nsite = new SiteEvent(face);
		Q.push(nsite);
		n++;
	}
	cin.clear();
}
void Diagram::add(myq & Q, Point tmp)
{
	Locus* face = new Locus;
	face->centre = tmp;
	faces.push_back(face);
	SiteEvent* nsite = new SiteEvent(face);
	Q.push(nsite);
	n++;
}
void Diagram::print()
{
	for (auto it = faces.begin(); it < faces.end(); it++)
	{
		(*it)->print();
		cout << endl;
	}
}
Diagram:: ~Diagram()
{
	for (auto it = faces.begin(); it < faces.end(); it++)
	{
		delete *it;
	}
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////		  EVENT	    \\\\\\\\\\\\\\\

bool Event:: operator==(Event & other) { return this->y == other.y && this->vertex == other.vertex && this->tp == other.tp; }

bool Event:: operator <(Event & other) { return this->y > other.y || (this->y == other.y && this->tp > other.tp) || (this->y == other.y && this->vertex.x() < other.vertex.x()); }
bool Event:: operator >(Event & other) { return this->y < other.y || (this->y == other.y && this->tp < other.tp) || (this->y == other.y && this->vertex.x() > other.vertex.x()); }
bool Event:: operator>=(Event & other) { return !(*this < other); }
bool Event:: operator<=(Event & other) { return !(*this > other); }
bool Event:: operator <(Node* cur)
{
	if (cur->ledge == nullptr)
	{
		return false;
	}
	return this->vertex.x() < cur->left_end(y);
}
bool Event:: operator >(Node* cur)
{
	if (cur->redge == nullptr)
	{
		return false;
	}
	return this->vertex.x() > cur->right_end(y);
}
bool Event:: operator==(Node* cur) { return !(*this < cur) && !(*this > cur); }
///////////////	############### \\\\\\\\\\\\\\\




///////////////	  SITE EVENT	\\\\\\\\\\\\\\\

SiteEvent::SiteEvent(Locus* nf) : Event(nf->centre.y(), nf->centre)
{
	this->tp = site;
	this->face = nf;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	  CIRC EVENT	\\\\\\\\\\\\\\\

CircEvent::CircEvent(Node* arcnd) : Event()
{
	this->tp = circ;
	if (!arcnd->ledge || !arcnd->redge)
	{
		this->valid = false;
		return;
	}
	Segment eleft = arcnd->ledge->edge;
	Segment eright = arcnd->redge->edge;
	Line lleft(eleft);
	Line lright(eright);
	// they can't coinside
	if (lleft.parallel(lright))
	{
		this->valid = false;
		return;
	}
	Point inter = lleft.intersection(lright);
	Segment tmp(arcnd->arc->face->centre, inter);
	y = inter.y() - tmp.len();
	MyDouble xl = arcnd->left_end(y);
	MyDouble xr = arcnd->right_end(y);
	// defering vectors from ends of the arc
	eleft = Point(xl, arcnd->arc->of(xl, y)) >> eleft;
	eright = Point(xr, arcnd->arc->of(xr, y)) >> eright;
	Segment ldir(eleft.start, inter);
	Segment rdir(eright.start, inter);
	// when their starts coinsides it's better understanding when drawing a picture
	if (eleft*ldir < NILL || eright * rdir < NILL || eleft.start == eright.start && (eleft ^ eright) >= NILL)
	{
		valid = false;
		return;
	}
	vertex = inter;
	arcnode = arcnd;
	arcnode->ledge->inters.push_back(this);
	arcnode->redge->inters.push_back(this);
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	      NODE     	\\\\\\\\\\\\\\\

Node::Node(SiteEvent* evt, Edge* ledge, Edge* redge) :
	ledge(ledge), redge(redge), left(nullptr), right(nullptr), parent(nullptr), height(1)
{
	arc = new Arc(evt);
}
Node::Node(Locus* face, Edge* ledge, Edge* redge) :
	ledge(ledge), redge(redge), left(nullptr), right(nullptr), parent(nullptr), height(1)
{
	arc = new Arc(face);
}
Node:: ~Node()
{
	delete arc;
	if (ledge) { delete ledge; }
	if (redge) { delete redge; }
}
int Node::get_height()
{
	return this != NULL ? this->height : 0;
}
int Node::leftH()
{
	return this->left->get_height();
}
int Node::rightH()
{
	return this->right->get_height();
}
int Node::corr_height()
{
	return this->height = 1 + ::max(this->left->get_height(), this->right->get_height());
}
void Node::set_parent(Node* npar)
{
	if (this != nullptr)
	{
		this->parent = npar;
	}
}
// a tangent vector (oriented "to the right") to this->arc in point x
Segment Node::tangent(MyDouble x, MyDouble sweep)
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
	//return ans.normalise();
	return ans;
}
MyDouble Node::left_end(MyDouble sweep)
{
	Node* lft = this->ledge->left;
	if (this->arc->face->centre.y() == sweep)
	{
		return this->arc->face->centre.x();
	}
	if (lft->arc->face->centre.y() == sweep)
	{
		return lft->arc->face->centre.x();
	}

	pair<MyDouble, MyDouble> inters = this->arc->intersection(*lft->arc, sweep);
	Segment tanths = this->tangent(inters.first, sweep);
	Segment tanlft = lft->tangent(inters.first, sweep);
	if ((tanths ^ tanlft) >= NILL)
	{
		return inters.first;
	}
	return inters.second;
}
MyDouble Node::right_end(MyDouble sweep)
{
	Node* rht = this->redge->right;
	if (this->arc->face->centre.y() == sweep)
	{
		return this->arc->face->centre.x();
	}
	if (rht->arc->face->centre.y() == sweep)
	{
		return rht->arc->face->centre.x();
	}
	pair<MyDouble, MyDouble> inters = this->arc->intersection(*rht->arc, sweep);
	Segment tanths = this->tangent(inters.second, sweep);
	Segment tanrht = rht->tangent(inters.second, sweep);
	if ((tanths ^ tanrht) <= NILL)
	{
		return inters.second;
	}
	return inters.first;
}
///////////////	############### \\\\\\\\\\\\\\\




///////////////	       AVL	   \\\\\\\\\\\\\\\

AVL::AVL(SiteEvent* first, myq* events) : events(events)
{
	root = new Node*;
	*root = new Node(first);
}
// returns parent's pointer to cur or a root pointer if cur is the root
Node** AVL::get_upper(Node* cur)
{
	if (cur == *root) { return root; }
	else
	{
		if (cur == cur->parent->left) { return &(cur->parent->left); }
		else { return &(cur->parent->right); }
	}
}
Node* AVL::left_turn(Node* cur)
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
Node* AVL::right_turn(Node* cur)
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
void AVL::up_balance(Node* cur)
{
	if (cur == nullptr) { return; }
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
void AVL::swap_nodes(Node* cur, Node* rightv)
// to swap:  (1) ->left, (2) ->right, (3) ->parent, (4) ->left->parent, (5) ->right->parent, (6) *upper 
{
	if (cur->right == rightv)
	{
		Node** curupper = get_upper(cur);
		cur->left->set_parent(rightv);		//1.4
		cur->right->set_parent(cur->parent);//1.5 2.3
		rightv->left->set_parent(cur);		//2.4
		rightv->right->set_parent(cur);		//2.5 
		cur->set_parent(rightv);			//1.3
		swap(cur->left, rightv->left);		//1.1 2.1
		cur->right = rightv->right;			//1.2
		rightv->right = cur;				//2.2 1.6
		*curupper = rightv;					//2.6
	}
	else
	{
		Node** curupper = get_upper(cur);
		Node** rgtupper = get_upper(rightv);
		cur->left->set_parent(rightv);		//1.4
		cur->right->set_parent(rightv);		//1.5
		rightv->left->set_parent(cur);		//2.4
		rightv->right->set_parent(cur);		//2.5
		swap(cur->left, rightv->left);		//1.1 2.1
		swap(cur->right, rightv->right);	//1.2 2.2
		swap(cur->parent, rightv->parent);	//1.3 2.3
		swap(*curupper, *rgtupper);			//1.6 2.6
	}
}

void AVL::insert_right(Segment & edg, SiteEvent* evt)
{
	_insert_right(*root, edg, evt);
}
void AVL::_insert_right(Node* cur, Segment & edg, SiteEvent* evt)
{
	if (!cur->right)
	{
		// tree
		Edge* edge = new Edge(edg);
		cur->right = new Node(evt);
		cur->right->parent = cur;
		// list
		cur->redge = edge;
		cur->right->ledge = edge;
		edge->left = cur;
		edge->right = cur->right;
		// add
		cur->arc->face->add(((-edge->edge) * 1000).end); // an "infinity" point
		cur->right->arc->face->add(((-edge->edge) * 1000).end);
		up_balance(cur->right);
	}
	else
	{
		_insert_right(cur->right, edg, evt);
	}
}
void AVL::insert(SiteEvent* evt)
{
	_insert(*root, evt);
}
void AVL::_insert(Node* cur, SiteEvent* evt)
{
	// inserting: three new nodes instear of the ol one (old + 2)
	// according to beachline properties there will be an ultimate node
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
		// a tangent vector to the arc
		Segment vr = cur->tangent(evt->vertex.x(), evt->y);
		Segment vl = -vr;
		// list
		Edge* el = new Edge(vl);
		Edge* er = new Edge(vr);
		// *cur = al											// left part of the divided edge
		Node* ao = new Node(evt, el, er);						// new (central) edge
		Node* ar = new Node(cur->arc->face, er, cur->redge);	// right part of the divided edge
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
		if (cur->right == nullptr)
		{
			cur->right = ar;
			ar->parent = cur;
			up_balance(cur);
		}
		else
			// means that curs left subtree is nonempty, so ar->redge->right 
			// is nonempty and is placed in curs right subtree 
			// but ar->redge->right->left is empty
		{
			Node* tmp = ar->redge->right; // used to be cur->redge->right
			tmp->left = ar;
			ar->parent = tmp;
			up_balance(tmp);
		}

		// now cur and ar are neighbours in the tree projection
		// and ar is from the right side of cur, so it's obveous 
		// that   either   ar->left == 0   or   cur->right == 0  
		// otherwise there's something between them and it's a contradiction
		if (ar->left == nullptr)
		{
			ar->left = ao;
			ao->parent = ar;
			up_balance(ar);
		}
		else
		{
			cur->right = ao;
			ao->parent = cur;
			up_balance(cur);
		}
		CircEvent* newevt = new CircEvent(ar);
		if (newevt->valid) { events->push(newevt); }
		else { delete newevt; }
		newevt = new CircEvent(cur);
		if (newevt->valid) { events->push(newevt); }
		else { delete newevt; }
	}
}
void AVL::del(CircEvent* evt)
{
	/* according to the beachline and Events structure,
	   cur->ledge != 0 && cur->redge != 0 */
	Node* cur = evt->arcnode;
	Node* al = cur->ledge->left;
	Node* ar = cur->redge->right;
	// pointer to the pointer to cur
	Edge*  nedge = new Edge(al->arc, ar->arc, evt);
	// list
	al->redge = nedge;
	nedge->left = al;
	ar->ledge = nedge;
	nedge->right = ar;
	// adding the vertex to the diagram
	cur->arc->face->add(evt->vertex);
	al->arc->face->add(evt->vertex);
	ar->arc->face->add(evt->vertex);
	// new events
	CircEvent* newevt = new CircEvent(ar);
	if (newevt->valid) { events->push(newevt); }
	else { delete newevt; }
	newevt = new CircEvent(al);
	if (newevt->valid) { events->push(newevt); }
	else { delete newevt; }
	//tree
	_del(get_upper(cur));
}
void AVL::_del(Node** cur)
{
	Node* tmp = *cur;
	if ((*cur)->left == nullptr && (*cur)->right == nullptr)
	{
		*cur = nullptr;		   // change parent's pointer
		up_balance(tmp->parent);
		delete tmp;
	}
	else if ((*cur)->left != nullptr && (*cur)->right == nullptr)
	{
		(*cur)->left->parent = (*cur)->parent;
		(*cur) = (*cur)->left; // change parent's pointer; also works if *cur is root
		delete tmp;
		up_balance(*cur);
	}
	else if ((*cur)->left == nullptr && (*cur)->right != nullptr)
	{
		(*cur)->right->parent = (*cur)->parent;
		(*cur) = (*cur)->right; // change parent's pointer
		delete tmp;
		up_balance(*cur);
	}
	else
	{
		// (*cur)->right != 0 so rightv is placed in the subtree of cur
		Node* rightv = (*cur)->redge->right;
		Node* todel = *cur;
		//(*cur)->swap_list_fields(rightv);
		//Node** tmpup = get_upper(rightv);
		//_del(tmpup);
		swap_nodes(todel, rightv);
		_del(get_upper(todel));
	}
}
void AVL::post_process()
{
	Node* cur = *root;
	while (cur->left)
	{
		cur = cur->left;
	}
	// now cur->ledge == nullptr
	if (!cur->redge)
		// unit arc
	{
		return;
	}
	Edge* edg = cur->redge;
	cur->arc->face->add(((edg->edge) * 1000).end);
	// also considering a case of just two arcs
	cur = cur->redge->right;
	while (cur->redge)
	{
		edg = cur->ledge;
		cur->arc->face->add(((edg->edge) * 1000).end);
		edg = cur->redge;
		cur->arc->face->add(((edg->edge) * 1000).end);
		cur = cur->redge->right;
	}
	// now cur->redge == nullptr
	edg = cur->ledge;
	cur->arc->face->add(((edg->edge) * 1000).end);
}
AVL:: ~AVL()
{
	Node* cur = *root;
	while (cur->left)
	{
		cur = cur->left;
	}
	// now cur->ledge == 0
	if (!cur->redge)
	{
		delete cur;
		return;
	}
	Node* tmp = cur;
	while (cur->redge)
	{
		cur = cur->redge->right;
		cur->ledge = nullptr; // cur->ledge == tmp->redge
		delete tmp;
		tmp = cur;
	}
	delete tmp;
}
///////////////	############### \\\\\\\\\\\\\\\
