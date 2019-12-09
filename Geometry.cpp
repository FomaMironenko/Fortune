#include "pch.h"
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;


int GCD(int a, int b)
{
	if (b == 0)
		return a;
	return GCD(b, a%b);
}

struct rational
{
	rational() : nom(0), den(1)
	{	}
	rational(int n, int d = 1)
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
	void get_int()
	{
		cin >> this->nom;
		this->den = 1;
	}

	bool operator==(const rational & other) const
	{
		return	(this->nom == other.nom && this->den == other.den) ||
			(this->nom == 0 && other.nom == 0);
	}
	bool operator==(int a) { return ((nom == a) && den == 1) || (nom == 0 && a == 0); }
	bool operator!=(const rational & other) { return !(*this == other); }
	bool operator!=(int a) { return !(*this == a); }

	rational operator +(const rational & other) const { return rational(this->nom*other.den + other.nom*this->den, this->den*other.den); }
	rational operator+=(const rational & other) { return *this = *this + other; }
	rational operator -() { return rational(-this->nom, this->den); }
	rational operator -(rational & other) const { return *this + -other; }
	rational operator-=(rational & other) { return *this = *this - other; }
	rational operator *(const rational & other) const { return rational(this->nom*other.nom, this->den*other.den); }
	rational operator *(int a) { return rational(nom * a, den); }
	rational operator*=(const rational & other) { return *this = *this * other; }
	rational operator /(const rational & other) const
	{
		if (other.nom == 0)
			return 0;
		return rational(this->nom * other.den, this->den * other.nom);
	}
	rational operator/=(const rational & other) { return *this = *this / other; }

	void print()
	{
		if (den != 1)
			cout << nom << "/" << den;
		else
			cout << nom;
	}

	int nom;
	int den;
};

struct MyDouble
{
	MyDouble() : value(0)
	{	}
	MyDouble(double x)
	{
		value = x;
	}

	operator double() { return value; }
	MyDouble &operator =(const MyDouble& other) { this->value = other.value; return *this; }
	MyDouble operator +(double other) { return MyDouble((double)this->value + other); }
	MyDouble operator+=(double other) { return *this = MyDouble(*this + other); }
	MyDouble operator -() { return MyDouble(-(double)*this); }
	MyDouble operator -(double other) { return MyDouble(*this + -other); }
	MyDouble operator-=(double other) { return *this = *this - other; }
	bool operator==(MyDouble & other) { return abs(*this - other) < eps; }
	bool operator<=(MyDouble & other) { return *this < other + eps; }
	bool operator>=(MyDouble & other) { return *this > other - eps; }
	bool operator <(MyDouble & other) { return *this < other - eps; }
	bool operator >(MyDouble & other) { return *this < other + eps; }

	friend istream& operator>>(istream& cin, MyDouble& num);

	const double eps = 1e-10;
	double value;
};

istream& operator>>(istream & cin, MyDouble & num)
{
	cin >> num.value;
	return cin;
}


template<class T>
struct Segment;
template<class T>
struct Point;
template<class T>
struct Line;
template<class T>
struct Polygon;


template <class T>
//actually a vector
struct Segment
{
	Segment()
	{	}
	Segment(T x1, T y1, T x2, T y2) : start(pair<T, T>(x1, y1)), end(pair<T, T>(x2, y2))
	{	}
	Segment(pair<T, T> start, pair<T, T> end) : start(start), end(end)
	{	}
	Segment(Point<T> st, Point<T> en)
	{
		start = st.end;
		end = en.end;
	}
	T x_project() { return end.first - start.first; }
	T y_project() { return end.second - start.second; }
	Segment operator +(Segment other) { return Segment(start, pair<T, T>(end.first + other.x_project(), end.second + other.y_project())); }
	Segment operator+=(Segment other) { return *this = *this + other; }
	Segment operator -() { return Segment(start, pair<T, T>(start.first - x_project(), start.second - y_project())); }
	Segment operator -(Segment other) { return *this + -other; }
	Segment operator-=(Segment other) { return *this = *this - other; }
	Segment operator *(T k) { return Segment(start, pair<T, T>(start.first + k * x_project(), start.second + k * y_project())); }
	Segment operator*=(T k) { return *this = (*this) * k; }
	//scalar product
	T operator *(Segment<T> other) { return x_project()*other.x_project() + y_project()*other.y_project(); }
	//vector product
	T operator ^(Segment<T> other) { return x_project()*other.y_project() - y_project()*other.x_project(); }
	operator Point<T>() const { return Point<T>(end.first, end.second); }

	MyDouble len()
	{
		return MyDouble(sqrt((double)x_project()*(double)x_project() + (double)y_project()*(double)y_project()));
	}
	void print()
	{
		cout << "[(" << start.first << ", " << start.second << "),  (" <<
			end.first << ", " << end.second << ")] ";
	}

	bool between(Point<MyDouble> pnt)
	{
		Segment<MyDouble> tmp(this->start, this->end);
		return (tmp*(Point<MyDouble>(start) - pnt)) <= 0 && (tmp*(Point<MyDouble>(end) - pnt)) >= 0;
	}
	bool contain(Point<MyDouble> pnt)
	{
		return this->between(pnt) && ((Point<MyDouble>(start) - pnt) ^ (Point<MyDouble>(end) - pnt)) == 0;
	}
	//пересечение отрезков
	bool cross(Segment<T> other)
	{
		Line<T> l1(other);
		Line<T> l2(*this);
		if (l1.parallel(l2))
		{
			if (l1(l2.pnt) != 0)
			{
				return false;
			}
			return this->contain(Point<MyDouble>(other.end)) || this->contain(Point<MyDouble>(other.start));
		}
		Point<MyDouble> a = (l1.intersection(l2));
		return this->between(a) && other.between(a);
	}

	pair<T, T> start;
	pair<T, T> end;
};

template <class T>
struct Point : Segment<T>
{
	Point() : Segment<T>(0, 0, 0, 0)
	{
		Segment<T>();
	}
	Point(pair<T, T> pnt): Segment<T>(pair<T, T>(0, 0), pnt)
	{
		this->end = pnt;
	}
	Point(T x1, T y1) : Segment<T>(0, 0, x1, y1)
	{
		this->start.first = this->start.second = 0;
		this->end.first = x1;
		this->end.second = y1;
	}
	void get()
	{
		cin >> this->end.first;
		cin >> this->end.second;
	}
	Point &operator=(Point<T> other)
	{
		this->end = other.end;
		return *this;
	}
	bool operator==(Point<T> & other) { return (this->end.first == other.end.first && this->end.second == other.end.second); }
	bool operator!=(Point<T> & other) { return !(*this == other); }
	bool operator >(Point<T> & other) { return (this->end.first > other.end.first || (this->end.first == other.end.second && this->end.second > other.end.second)); }
	bool operator <(Point<T> & other) { return (this->end.first < other.end.second || (this->end.first == other.end.second && this->end.second < other.end.second)); }
	bool operator>=(Point<T> & other) { return !(*this < other); }
	bool operator<=(Point<T> & other) { return !(*this > other); }
	//builds a vector from the point
	Segment<T> operator >>(Segment<T> other) { return Segment<T>(this->end, pair<T, T>(this->end.first + other.x_project(), this->end.second + other.y_project())); }
	Segment<T> operator <<(Segment<T> other) { return Segment<T>(this->end, pair<T, T>(this->end.first - other.x_project(), this->end.second - other.y_project())); }

	void print(bool nice = false)
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
	T polar_ang(Point<T> p1, Point<T> p2) { return (p1 - *this) ^ (p2 - *this); }
};

template <class T>
struct Line
{
	Line(Segment<T> guide)
	{
		pnt = Point<T>(guide.start.first, guide.start.second);
		a = guide.y_project();
		b = -guide.x_project();
		c = guide.x_project() * guide.start.second - guide.y_project() * guide.start.first;
	}
	Line(Point<T> st, Point<T> en)
	{
		pnt = st;
		if (st == en)
		{
			a = b = c = 0;
		}
		else
		{
			*this = Line(Segment<T>(st, en));
		}
	}
	Line(Point<T> st, Segment<T> normal)
	{
		pnt = st;
		a = normal.x_project();
		b = normal.y_project();
		c = -a * st.end.first - b * st.end.second;
	}

	T operator()(Point<T> pnt) { return a * pnt.end.first + b * pnt.end.second + c; }

	Point<MyDouble> intersection(Line<T> other)
	{
		T det = a * other.b - b * other.a;
		if (det == 0)
		{
			if (other(this->pnt) == 0)
			{
				return Point<MyDouble>(pnt.end.first, pnt.end.second);
			}
			return Point<MyDouble>(numeric_limits<double>::max(), numeric_limits<double>::max());
		}
		//attention!!! remake for rational and float
		return Point<MyDouble>(((MyDouble)other.c*b - c * other.b) / det, ((MyDouble)other.a*c - a * other.c) / det);
	}
	int parallel(Line<T> other)
	{
		return a * other.b - b * other.a == 0;
	}

	// ax + by + c = 0
	T a;
	T b;
	T c;
	Point<T> pnt;
};

template <class T>
struct Polygon
{
	Polygon() : points(vector<Point<T>>(0)), n(0)
	{	}
	//should be in correct order
	Polygon(vector<Point<T>> pnts) : points(pnts), n(pnts.size())
	{	}
	Polygon(int n) : n(n)
	{
		points = vector<Point<T>>(n);
		for (int i = 0; i < n; i++)
		{
			points[i].get();
		}
	}

	struct Comp
	{
		Comp(Point<T> p)
		{
			origin = p;
		}
		bool operator()(Point<T> &p1, Point<T> &p2)
		{
			return origin.polar_ang(p1, p2) > 0 || (origin.polar_ang(p1, p2) == 0 && (p1 - origin)*(p1 - p2) <= 0);
		}
		Point<T> origin;
	};

	void convex_hull()
	{
		int n;
		cin >> n;
		vector<Point<T>> bucket(n);
		bucket[0].get();
		Point<T> origin;
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
		Point<T> second;
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

	void print()
	{
		for (auto it = points.begin(); it < points.end(); it++)
		{
			it->print(true);
			cout << endl;
		}
	}

	int n;
	vector<Point<T>> points;
};


int main()
{

	return EXIT_SUCCESS;
}