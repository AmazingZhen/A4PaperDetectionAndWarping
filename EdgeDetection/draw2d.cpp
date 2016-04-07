#include "stdafx.h"
#include "draw2d.h"

int _round(double x) {
	return floor(x + 0.5);
}

double ipart(double x) {
	return x - floor(x);
}

void swap(double & a, double & b) {
	double t = a;
	a = b;
	b = t;
}

double random(int min, int max) {
	return rand() % (max - min + 1) + min;
}