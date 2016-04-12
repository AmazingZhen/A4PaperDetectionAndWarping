#ifndef _HOUGH_H_
#define _HOUGH_H_

#include "CImg.h"
#include "draw2d.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;
using namespace cimg_library;

#define HOUGH_THRESHOLD_FACTOR 0.35;

static bool flag = false;
static float sin_val[360];
static float cos_val[360];

struct HoughPoint {
	float theta;
	float p;
	int vote;
	HoughPoint() {
		theta = 0;
		p = 0;
		vote = 0;
	}

	HoughPoint(const HoughPoint& other) {
		theta = other.theta;
		p = other.p;
		vote = other.vote;
	}

	HoughPoint(float _theta, float _p, int _vote = 0) {
		theta = _theta;
		p = _p;
		vote = _vote;
	}
};

bool operator<(const HoughPoint &x, const HoughPoint &y);

template <class T>
void drawHoughPixel(CImg<T>& image, int x, int y, float value) {
	assert(x >= 0 && x < image.width());
	assert(y >= 0 && y < image.height());
	assert(image.spectrum() == 1);

	float new_value = image(x, image.height() - 1 - y) + value;
	image(x, image.height() - 1 - y) = new_value;
}

template <class T>
void drawHoughPoint(CImg<T>& image, int theta, float p) {
	int p_draw = _round(p);

	if (p_draw < image.height() && p_draw >= 0) {
		drawHoughPixel(image, theta, p_draw, 1);
	}
}

template <class T>
void drawHoughCurve(CImg<T>& image, int x, int y) {
	int theta = 0;

	while (theta < 360) {
		float p = x * cos_val[theta] + y * sin_val[theta];
		drawHoughPoint(image, theta, p);

		theta++;
	}
}

float houghPointDistance(const HoughPoint& a, const HoughPoint& b);
CImg<int> getHoughSpaceFromEdge(const CImg<unsigned char>& edge);
vector<HoughPoint> getHoughParaFromHoughSpace(const CImg<int>& houghSpace);

vector<HoughPoint> getHoughParamFromEdge(const CImg<unsigned char>& edge);

#endif