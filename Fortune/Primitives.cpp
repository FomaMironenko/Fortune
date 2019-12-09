#include "pch.h"
#define PCH


#include "primitives.h"

int GCD(int a, int b)
{
	if (b == 0)
		return a;
	return GCD(b, a%b);
}



///////////////////        RATIONAL       \\\\\\\\\\\\\\\\\\\\

rational:: rational() : nom(0), den(1)
{	}
rational:: rational(int n, int d)
{
	int tmp = GCD(n, d);
	nom = n / tmp;
	den = d / tmp;
	if (den < 0)
	{
		den = -den;
		nom = -nom;
	}
}
void rational:: get_int()
{
	cin >> this->nom;
	this->den = 1;
}

bool rational:: operator==(const rational & other) const
{
	return	(this->nom == other.nom && this->den == other.den) ||
		(this->nom == 0 && other.nom == 0);
}
bool rational:: operator==(int a) { return ((nom == a) && den == 1) || (nom == 0 && a == 0); }
bool rational:: operator!=(const rational & other) { return !(*this == other); }
bool rational:: operator!=(int a) { return !(*this == a); }
rational rational:: operator +(const rational & other) const { return rational(this->nom*other.den + other.nom*this->den, this->den*other.den); }
rational rational:: operator+=(const rational & other) { return *this = *this + other; }
rational rational:: operator -() { return rational(-this->nom, this->den); }
rational rational:: operator -(rational & other) const { return *this + -other; }
rational rational:: operator-=(rational & other) { return *this = *this - other; }
rational rational:: operator *(const rational & other) const { return rational(this->nom*other.nom, this->den*other.den); }
rational rational:: operator *(int a) { return rational(nom * a, den); }
rational rational:: operator*=(const rational & other) { return *this = *this * other; }
rational rational:: operator /(const rational & other) const
{
	if (other.nom == 0)
		return 0;
	return rational(this->nom * other.den, this->den * other.nom);
}
rational rational:: operator/=(const rational & other) { return *this = *this / other; }

void rational:: print()
{
	if (den != 1)
		cout << nom << "/" << den;
	else
		cout << nom;
}
//////////////////// ################### \\\\\\\\\\\\\\\\\\\\




///////////////////       MY DOUBLE      \\\\\\\\\\\\\\\\\\\\

MyDouble:: MyDouble() : value(0)
{	}
MyDouble:: MyDouble(double x)
{
	value = x;
}

MyDouble:: operator double() const { return value; }
MyDouble & MyDouble:: operator =(const MyDouble& other) { this->value = other.value; return *this; }
MyDouble MyDouble:: operator +(double other) { return MyDouble((double)this->value + other); }
MyDouble MyDouble:: operator+=(double other) { return *this = MyDouble(*this + other); }
MyDouble MyDouble:: operator -() { return MyDouble(-(double)*this); }
MyDouble MyDouble:: operator -(double other) { return MyDouble(*this + -other); }
MyDouble MyDouble:: operator-=(double other) { return *this = *this - other; }
bool MyDouble:: operator==(MyDouble & other) { return abs(*this - other) < eps; }
bool MyDouble:: operator<=(MyDouble & other) { return *this < other + eps; }
bool MyDouble:: operator>=(MyDouble & other) { return *this > other - eps; }
bool MyDouble:: operator <(MyDouble & other) { return *this < other - eps; }
bool MyDouble:: operator >(MyDouble & other) { return *this < other + eps; }

istream & operator>>(istream & cin, MyDouble & num)
{
	cin >> num.value;
	return cin;
}
ostream & operator<<(ostream & cout, MyDouble & num)
{
	cout << num.value;
	return cout;
}
//////////////////// ################### \\\\\\\\\\\\\\\\\\\\




////////////////////       SEGMENT        \\\\\\\\\\\\\\\\\\\\

Segment:: Segment() : start(pair<MyDouble, MyDouble>(0, 0)), end(pair<MyDouble, MyDouble>(0, 0))
{	}
Segment:: Segment(MyDouble x1, MyDouble y1, MyDouble x2, MyDouble y2) : start(pair<MyDouble, MyDouble>(x1, y1)), end(pair<MyDouble, MyDouble>(x2, y2))
{	}
Segment:: Segment(pair<MyDouble, MyDouble> start, pair<MyDouble, MyDouble> end) : start(start), end(end)
{	}
Segment:: Segment(Point st, Point en): start(st.end), end(en.end)
{	}
Segment:: Segment(Segment const & other): start(other.start), end(other.end)
{	}
Segment & Segment:: operator=(Segment other)
{
	this->start = other.start;
	this->end = other.end;
	return *this;
}
MyDouble Segment:: x_project() { return end.first - start.first; }
MyDouble Segment:: y_project() { return end.second - start.second; }
Segment Segment:: operator +(Segment other) { return Segment(start, pair<MyDouble, MyDouble>(end.first + other.x_project(), end.second + other.y_project())); }
Segment Segment:: operator+=(Segment other) { return *this = *this + other; }
Segment Segment:: operator -() { return Segment(start, pair<int, int>(start.first - x_project(), start.second - y_project())); }
Segment Segment:: operator -(Segment other) { return *this + -other; }
Segment Segment:: operator-=(Segment other) { return *this = *this - other; }
Segment Segment:: operator *(MyDouble k) { return Segment(start, pair<MyDouble, MyDouble>(start.first + k * x_project(), start.second + k * y_project())); }
Segment Segment:: operator*=(MyDouble k) { return *this = (*this) * k; }
//scalar product
MyDouble Segment:: operator *(Segment other) { return x_project()*other.x_project() + y_project()*other.y_project(); }
//vector product
MyDouble Segment::operator ^(Segment other) { return x_project()*other.y_project() - y_project()*other.x_project(); }

MyDouble Segment:: len()
{
	return MyDouble(sqrt((double)x_project()*(double)x_project() + (double)y_project()*(double)y_project()));
}

void Segment:: print()
{
	cout << "[(" << start.first << ", " << start.second << "),  (" <<
		end.first << ", " << end.second << ")] ";
}

bool Segment:: between(Point pnt)
{
	return ((*this)*(Point(start) - pnt)) <= 0 && ((*this)*(Point(end) - pnt)) >= 0;
}

bool Segment:: contain(Point pnt)
{
	return this->between(pnt) && ((Point(start) - pnt) ^ (Point(end) - pnt)) == 0;
}

bool Segment:: cross(Segment other)
{
	Line l1(other);
	Line l2(*this);
	if (l1.parallel(l2))
	{
		if (l1(l2.pnt) != 0)
		{
			return false;
		}
		return this->contain(Point(other.end)) || this->contain(Point(other.start));
	}
	Point a = (l1.intersection(l2));
	return this->between(a) && other.between(a);
}
//////////////////// ################### \\\\\\\\\\\\\\\\\\\\




////////////////////        POINT        \\\\\\\\\\\\\\\\\\\\

Point:: Point() : Segment()
{	}
Point:: Point(pair<MyDouble, MyDouble> pnt) : Segment(0, 0, pnt.first, pnt.second)
{	}
Point:: Point(MyDouble x1, MyDouble y1) : Segment(0, 0, x1, y1)
{	}
Point:: Point(Segment pnt) : Segment(0, 0, pnt.end.first, pnt.end.second)
{	}
Point & Point:: operator=(Point other)
{
	this->end = other.end;
	return *this;
}
void Point:: get()
{
	cin >> this->end.first;
	cin >> this->end.second;
}
bool Point:: operator==(Point & other) { return (this->end.first == other.end.first && this->end.second == other.end.second); }
bool Point:: operator!=(Point & other) { return !(*this == other); }
bool Point:: operator >(Point & other) { return (this->end.first > other.end.first || (this->end.first == other.end.second && this->end.second > other.end.second)); }
bool Point:: operator <(Point & other) { return (this->end.first < other.end.second || (this->end.first == other.end.second && this->end.second < other.end.second)); }
bool Point:: operator>=(Point & other) { return !(*this < other); }
bool Point:: operator<=(Point & other) { return !(*this > other); }
//builds a vector from the point
Segment Point:: operator >>(Segment other) { return Segment(this->end, pair<MyDouble, MyDouble>(this->end.first + other.x_project(), this->end.second + other.y_project())); }
Segment Point:: operator <<(Segment other) { return Segment(this->end, pair<MyDouble, MyDouble>(this->end.first - other.x_project(), this->end.second - other.y_project())); }
void Point:: print(bool nice)
{
	if (nice)
	{
		cout << "(" << this->end.first << ", " << this->end.second << ") ";
	}
	else
	{
		cout << this->end.first << " " << this->end.second;
	}
}
MyDouble Point:: polar_ang(Point p1, Point p2) { return (p1 - *this) ^ (p2 - *this); }
//////////////////// ################### \\\\\\\\\\\\\\\\\\\\




////////////////////         LINE        \\\\\\\\\\\\\\\\\\\\


Line:: Line(Segment guide)
{
	pnt = Point(guide.start.first, guide.start.second);
	a = guide.y_project();
	b = -guide.x_project();
	c = guide.x_project() * guide.start.second - guide.y_project() * guide.start.first;
}
Line:: Line(Point st, Point en)
{
	pnt = st;
	if (st == en)
	{
		a = b = c = 0;
	}
	else
	{
		*this = Line(Segment(st, en));
	}
}
Line:: Line(Point st, Segment normal)
{
	pnt = st;
	a = normal.x_project();
	b = normal.y_project();
	c = -a * st.end.first - b * st.end.second;
}
MyDouble Line:: operator()(Point pnt) { return a * pnt.end.first + b * pnt.end.second + c; }
Point Line:: intersection(Line other)
{
	MyDouble det = a * other.b - b * other.a;
	if (det == 0)
	{
		if (other(this->pnt) == 0)
		{
			return pnt;
		}
		return Point(numeric_limits<double>::max(), numeric_limits<double>::max());
	}
	return Point(((MyDouble)other.c*b - c * other.b) / det, ((MyDouble)other.a*c - a * other.c) / det);
}
int Line:: parallel(Line other)
{
	return a * other.b - b * other.a == 0;
}
//////////////////// ################### \\\\\\\\\\\\\\\\\\\\




////////////////////       POLYGON       \\\\\\\\\\\\\\\\\\\\

Polygon:: Polygon() : points(vector<Point>(0)), n(0)
{	}
//should be in correct order
Polygon:: Polygon(vector<Point> pnts) : points(pnts), n(pnts.size())
{	}
Polygon:: Polygon(int n) : n(n)
{
	points = vector<Point>(n);
	for (int i = 0; i < n; i++)
	{
		points[i].get();
	}
}

Polygon::Comp::Comp(Point p)
{
	origin = p;
}
bool Polygon::Comp:: operator()(Point & p1, Point & p2)
{
	return origin.polar_ang(p1, p2) > 0 || (origin.polar_ang(p1, p2) == 0 && (p1 - origin)*(p1 - p2) <= 0);
}

void Polygon::convex_hull()
{
	int n;
	cin >> n;
	vector<Point> bucket(n);
	bucket[0].get();
	Point origin;
	origin = bucket[0];
	//also defines the lowest left point
	for (int i = 1; i < n; i++)
	{
		bucket[i].get();
		if (bucket[i] < origin)
		{
			origin = bucket[i];
		}
	}

	points.push_back(origin);
	Comp cmp(origin);
	if (bucket.size() == 1)
	{
		return;
	}
	sort(bucket.begin(), bucket.end(), cmp);
	points.push_back(bucket[1]);
	Point second;
	for (auto it = bucket.begin() + 2; it < bucket.end(); it++)
	{
		second = *(points.end() - 2);
		while (points.size() > 1 && ((*it - second) ^ (points.back() - second)) >= 0)
		{
			points.pop_back();
			if (points.size() == 1)
			{
				break;
			}
			second = *(points.end() - 2);
		}
		points.push_back(*it);
	}
	return;
}

void Polygon::print()
{
	for (auto it = points.begin(); it < points.end(); it++)
	{
		it->print(true);
		cout << endl;
	}
}

//////////////////// ################### \\\\\\\\\\\\\\\\\\\\