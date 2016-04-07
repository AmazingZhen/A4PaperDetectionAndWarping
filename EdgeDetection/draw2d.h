#ifndef _DRAW2D_H_
#define _DRAW2D_H_

#include "CImg.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>

#define GRAY_LEVEL 256

using namespace cimg_library;
using namespace std;

double random(int min, int max);

int _round(double x);

double ipart(double x);

void swap(double & a, double & b);

// Mapping the coordinate system x, y to col, row, which is pixel's actual coordinate.
template <class T>
void draw2d_pixel(CImg<T>& image, int x, int y, unsigned char gray) {
	assert(x >= 0 && x < image.width());
	assert(y >= 0 && y < image.height());

	for (int i = 0; i < image.spectrum(); i++) {
		image(x, image.height() - 1 - y, i) = (T)gray;
	}
}

// Xiaolin Wu's algorithm.
template <class T>
void draw2d_line(CImg<T>& image, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;

    bool flag = false;
    if (abs(dx) < abs(dy)) {
        swap(x1, y1);
        swap(x2, y2);
        swap(dx, dy);

        // Because x and y are swaped to satisfy algorithm.
        // Need to be swaped again when rasterizing.
        flag = true;
    }

    if (x2 < x1) {
        swap(x1, x2);
        swap(y1, y2);
    }

    double gradient = dy / dx;

    // Draw the start point
    int x1_draw = _round(x1);
    double y1_true = y1 + gradient * (x1_draw - x1);
    int y1_draw = floor(y1_true);
    draw2d_pixel(image, x1_draw, y1_draw, ipart(y1_true) * (GRAY_LEVEL - 1), flag);
    draw2d_pixel(image, x1_draw, y1_draw + 1, (1 - ipart(y1_true)) * (GRAY_LEVEL - 1), flag);

    double y_next = y1_true + gradient;

    // Draw the end point.
    int x2_draw = _round(x2);
    double y2_true = y2 + gradient * (x2_draw - x2);
    int y2_draw = floor(y2_true);
    draw2d_pixel(image, x2_draw, y2_draw, ipart(y2_true) * (GRAY_LEVEL - 1), flag);
    draw2d_pixel(image, x2_draw, y2_draw + 1, (1 - ipart(y2_true)) * (GRAY_LEVEL - 1), flag);

    for (int x = x1_draw + 1; x < x2_draw; x++) {
        int y = floor(y_next);

        draw2d_pixel(image, x, y, ipart(y_next) * (GRAY_LEVEL - 1), flag);
        draw2d_pixel(image, x, y + 1, (1 - ipart(y_next)) * (GRAY_LEVEL - 1), flag);

        y_next = y_next + gradient;
    }
}

#endif