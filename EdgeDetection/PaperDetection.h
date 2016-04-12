#ifndef _PAPERDETECTION_H_
#define _PAPERDETECTION_H_

#include "CImg.h"
#include "Hough.h"
#include "Canny.h"
#include "Segmentation.h"

#define PI 3.14159265
#define TAN_10 0.17632698
#define TAN_80 5.67128181

#define PAPER_WIDTH 1190
#define PAPER_HEIGHT 1684

using namespace std;

struct Point {
	int x;
	int y;
	Point(int _x = 0, int _y = 0) {
		x = _x;
		y = _y;
	}
};

vector<HoughPoint> getLinesFromHoughPara(const vector<HoughPoint> &houghPara, int num);
vector<Point> getVertexFromLineParam(vector<HoughPoint>& lines_param);

void drawVertex(CImg<float>& src_img, CImg<float> &gray);
CImg<float> warp(const CImg<float>& src_img, CImg<float> &gray);

#endif