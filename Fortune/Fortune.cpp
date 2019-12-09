#include "pch.h"
#include <iostream>

#include "primitives.h"

int main()
{
	Point a(1, 2);
	Point b(3, 4);
	Segment s1(a, b);
	Segment s2(0, 0, 1.1, 2);
	cout << s1.cross(s2);
}