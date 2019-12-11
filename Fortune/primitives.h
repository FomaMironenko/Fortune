#ifndef PRIMS
#define PRIMS


#include "pch.h"
#include <iostream>
#include <algorithm>
#include <vector>


using namespace std;


struct Segment;
struct Point;
struct Line;
struct Polygon;


int GCD(int, int);

struct rational
{
	rational();
	rational(int, int den = 1);
	void get_int();
	bool operator==(const rational &) const;
	bool operator==(int);
	bool operator!=(const rational &);
	bool operator!=(int);

	rational operator +(const rational &) const;
	rational operator+=(const rational &);
	rational operator -();
	rational operator - (rational &) const;
	rational operator-=(rational &);
	rational operator *(const rational &) const;
	rational operator *(int);
	rational operator*=(const rational &);
	rational operator /(const rational &) const;
	rational operator/=(const rational &);

	void print();
	int nom;
	int den;
};

struct MyDouble
{
	MyDouble();
	MyDouble(double);
	operator double() const;
	MyDouble &operator =(const MyDouble&);
	MyDouble operator +(double);
	MyDouble operator+=(double);
	MyDouble operator -();
	MyDouble operator -(double);
	MyDouble operator-=(double);
	bool operator==(MyDouble &);
	bool operator<=(MyDouble &);
	bool operator>=(MyDouble &);
	bool operator <(MyDouble &);
	bool operator >(MyDouble &);
	friend istream& operator>>(istream&, MyDouble&);
	friend ostream& operator<<(ostream&, MyDouble&);
	const double eps = 1e-6;
	double value;
};


struct Segment
{
	Segment();
	Segment(MyDouble, MyDouble, MyDouble, MyDouble);
	Segment(pair<MyDouble, MyDouble>, pair<MyDouble, MyDouble>);
	Segment(Point, Point);
	Segment(Segment const &);
	Segment & operator=(Segment);
	MyDouble x_project();
	MyDouble y_project();
	Segment operator +(Segment);
	Segment operator+=(Segment);
	Segment operator -();
	Segment operator -(Segment);
	Segment operator-=(Segment);
	Segment operator *(MyDouble);
	Segment operator*=(MyDouble);
	//scalar product
	MyDouble operator *(Segment);
	//vector product
	MyDouble operator ^(Segment);

	MyDouble len();
	void print();
	bool between(Point);
	bool contain(Point);
	//ïåðåñå÷åíèå îòðåçêîâ
	bool cross(Segment);

	pair<MyDouble, MyDouble> start;
	pair<MyDouble, MyDouble> end;
};


struct Point : Segment
{
	Point();
	Point(pair<MyDouble, MyDouble>);
	Point(MyDouble, MyDouble);
	Point(Segment);
	Point &operator=(Point);
	void get();
	bool operator==(Point &);
	bool operator!=(Point &); 
	bool operator >(Point &); 
	bool operator <(Point &); 
	bool operator>=(Point &); 
	bool operator<=(Point &);
	Segment operator >>(Segment);
	Segment operator <<(Segment);
	void print(bool nice = true);
	MyDouble polar_ang(Point, Point);
};

struct Line
{
	Line(Segment);
	Line(Point, Point);
	Line(Point, Segment);
	MyDouble operator()(Point);
	Point intersection(Line);
	int parallel(Line);
	MyDouble a;
	MyDouble b;
	MyDouble c;
	Point pnt;
};

struct Polygon
{
	Polygon();
	Polygon(vector<Point>);
	Polygon(int);
	struct Comp
	{
		Comp(Point);
		bool operator()(Point &, Point &);
		Point origin;
	};
	void convex_hull();
	void print();
	int n;
	vector<Point> points;
};

#endif
