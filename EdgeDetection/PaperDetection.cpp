#include "stdafx.h"
#include "PaperDetection.h"
#include "interpolation.h"
#include <iostream>
#include <queue>
#include <cassert>

bool containCurPoint(const vector<HoughPoint>& center_points, const HoughPoint& new_center_point) {
	if (center_points.size() == 0)
		return false;

	for (int i = 0; i < center_points.size(); i++) {
		if (houghPointDistance(center_points[i], new_center_point) < 100) {
			return true;
		}
	}

	return false;
}

vector<HoughPoint> getLinesFromHoughPara(const vector<HoughPoint> &houghPara, int num) {
	assert(num >= 1);

	int count = 1;
	vector<HoughPoint> candidate_line(num);
	candidate_line[0] = houghPara[0];

	for (int i = 1; i < houghPara.size(); i++) {
		if (!containCurPoint(candidate_line, houghPara[i])) {
			int delta_theta = candidate_line[0].theta - houghPara[i].theta;
			float tan_val = abs(abs(tan(delta_theta * PI / 180.0)));

			if (tan_val < TAN_10 || tan_val > TAN_80) {
				candidate_line[count] = houghPara[i];
				count++;
			}

			if (count == num) {
				for (int i = 0; i < candidate_line.size(); i++) {
					cout << candidate_line[i].theta << "," << candidate_line[i].p << "," << candidate_line[i].vote << endl;
				}

				return candidate_line;
			}
		}
	}

	return candidate_line;
}

float getDistance(const Point &a, const Point &b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

vector<Point> getVertexFromLineParam(vector<HoughPoint>& lines_param) {
	assert(lines_param.size() == 4);

	vector<HoughPoint> parallel_line_param;
	vector<HoughPoint> vertical_line_param;

	parallel_line_param.push_back(lines_param[0]);

	// Find parallel and vertical lines.
	for (int i = 1; i < lines_param.size(); i++) {
		int delta_theta = parallel_line_param[0].theta - lines_param[i].theta;
		float tan_val = abs(abs(tan(delta_theta * PI / 180.0)));

		if (tan_val < TAN_10) {
			parallel_line_param.push_back(lines_param[i]);
		}
		else if (tan_val > TAN_80) {
			vertical_line_param.push_back(lines_param[i]);
		}
	}

	vector<Point> vertexs;

	for (int i = 0; i < parallel_line_param.size(); i++) {
		for (int j = 0; j < vertical_line_param.size(); j++) {
			float theta1 = parallel_line_param[i].theta;
			float theta2 = vertical_line_param[j].theta;
			float p1 = parallel_line_param[i].p;
			float p2 = vertical_line_param[j].p;

			int x_pos = (p2 * sin(theta1 * PI / 180.0) - p1 * sin(theta2 * PI / 180.0)) / sin((theta1 - theta2) * PI / 180.0);
			int y_pos = (p2 * cos(theta1 * PI / 180.0) - p1 * cos(theta2 * PI / 180.0)) / sin((theta2 - theta1) * PI / 180.0);

			Point new_vertex(x_pos, y_pos);
			vertexs.push_back(new_vertex);
		}
	}
	Point origin(0, 0);

	float min_dis = getDistance(vertexs[0], origin);

	for (int i = 1; i < vertexs.size(); i++) {
		if (getDistance(vertexs[i], origin) < min_dis) {
			min_dis = getDistance(vertexs[i], origin);
			Point temp = vertexs[0];
			vertexs[0] = vertexs[i];
			vertexs[i] = temp;
		}
	}

	int i = 1;

	while (i < vertexs.size()) {
		float min_dis = getDistance(vertexs[0], vertexs[i]);

		for (int j = i + 1; j < vertexs.size(); j++) {
			if (getDistance(vertexs[0], vertexs[j]) < min_dis) {
				min_dis = getDistance(vertexs[j], origin);
				Point temp = vertexs[i];
				vertexs[i] = vertexs[j];
				vertexs[j] = temp;
			}
		}
		i++;
	}

	if (vertexs[1].y > vertexs[2].y) {
		Point temp = vertexs[0];
		vertexs[0] = vertexs[1];
		vertexs[1] = temp;

		temp = vertexs[2];
		vertexs[2] = vertexs[3];
		vertexs[3] = temp;
	}

	cout << "Four vertexs are:" << endl;
	for (int i = 0; i < vertexs.size(); i++) {
		cout << vertexs[i].x << ", " << vertexs[i].y << endl;
	}

	return vertexs;
}

CImg<float> warp(const CImg<float> &src_img, CImg<float> &gray) {
	CImg<unsigned char> edge = getEdgeBySegmentation(gray);
	vector<HoughPoint> hough_param = getHoughParamFromEdge(edge);
	vector<HoughPoint> lines_param = getLinesFromHoughPara(hough_param, 4);
	vector<Point> vertexs = getVertexFromLineParam(lines_param);

	float u0 = 0, v0 = 0;
	float u1 = PAPER_WIDTH, v1 = 0;
	float u2 = 0, v2 = PAPER_HEIGHT;
	float u3 = PAPER_WIDTH, v3 = PAPER_HEIGHT;

	float x0 = vertexs[0].x, y0 = vertexs[0].y;
	float x1 = vertexs[1].x, y1 = vertexs[1].y;
	float x2 = vertexs[2].x, y2 = vertexs[2].y;
	float x3 = vertexs[3].x, y3 = vertexs[3].y;

	float c1, c2, c3, c4, c5, c6, c7, c8;

	c1 = -(u0*v0*v1*x2 - u0*v0*v2*x1 - u0*v0*v1*x3 + u0*v0*v3*x1 - u1*v0*v1*x2 + u1*v1*v2*x0 + u0*v0*v2*x3 - u0*v0*v3*x2 + u1*v0*v1*x3 - u1*v1*v3*x0 + u2*v0*v2*x1 - u2*v1*v2*x0
		- u1*v1*v2*x3 + u1*v1*v3*x2 - u2*v0*v2*x3 + u2*v2*v3*x0 - u3*v0*v3*x1 + u3*v1*v3*x0 + u2*v1*v2*x3 - u2*v2*v3*x1 + u3*v0*v3*x2 - u3*v2*v3*x0 - u3*v1*v3*x2 + u3*v2*v3*x1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c2 = (u0*u1*v0*x2 - u0*u2*v0*x1 - u0*u1*v0*x3 - u0*u1*v1*x2 + u0*u3*v0*x1 + u1*u2*v1*x0 + u0*u1*v1*x3 + u0*u2*v0*x3 + u0*u2*v2*x1 - u0*u3*v0*x2 - u1*u2*v2*x0 - u1*u3*v1*x0
		- u0*u2*v2*x3 - u0*u3*v3*x1 - u1*u2*v1*x3 + u1*u3*v1*x2 + u1*u3*v3*x0 + u2*u3*v2*x0 + u0*u3*v3*x2 + u1*u2*v2*x3 - u2*u3*v2*x1 - u2*u3*v3*x0 - u1*u3*v3*x2 + u2*u3*v3*x1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c3 = (u0*v1*x2 - u0*v2*x1 - u1*v0*x2 + u1*v2*x0 + u2*v0*x1 - u2*v1*x0 - u0*v1*x3 + u0*v3*x1 + u1*v0*x3 - u1*v3*x0 - u3*v0*x1 + u3*v1*x0
		+ u0*v2*x3 - u0*v3*x2 - u2*v0*x3 + u2*v3*x0 + u3*v0*x2 - u3*v2*x0 - u1*v2*x3 + u1*v3*x2 + u2*v1*x3 - u2*v3*x1 - u3*v1*x2 + u3*v2*x1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c4 = (u0*u1*v0*v2*x3 - u0*u1*v0*v3*x2 - u0*u2*v0*v1*x3 + u0*u2*v0*v3*x1 + u0*u3*v0*v1*x2 - u0*u3*v0*v2*x1 - u0*u1*v1*v2*x3 + u0*u1*v1*v3*x2 + u1*u2*v0*v1*x3 - u1*u2*v1*v3*x0 - u1*u3*v0*v1*x2 + u1*u3*v1*v2*x0
		+ u0*u2*v1*v2*x3 - u0*u2*v2*v3*x1 - u1*u2*v0*v2*x3 + u1*u2*v2*v3*x0 + u2*u3*v0*v2*x1 - u2*u3*v1*v2*x0 - u0*u3*v1*v3*x2 + u0*u3*v2*v3*x1 + u1*u3*v0*v3*x2 - u1*u3*v2*v3*x0 - u2*u3*v0*v3*x1 + u2*u3*v1*v3*x0)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c5 = -(u0*v0*v1*y2 - u0*v0*v2*y1 - u0*v0*v1*y3 + u0*v0*v3*y1 - u1*v0*v1*y2 + u1*v1*v2*y0 + u0*v0*v2*y3 - u0*v0*v3*y2 + u1*v0*v1*y3 - u1*v1*v3*y0 + u2*v0*v2*y1 - u2*v1*v2*y0
		- u1*v1*v2*y3 + u1*v1*v3*y2 - u2*v0*v2*y3 + u2*v2*v3*y0 - u3*v0*v3*y1 + u3*v1*v3*y0 + u2*v1*v2*y3 - u2*v2*v3*y1 + u3*v0*v3*y2 - u3*v2*v3*y0 - u3*v1*v3*y2 + u3*v2*v3*y1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c6 = (u0*u1*v0*y2 - u0*u2*v0*y1 - u0*u1*v0*y3 - u0*u1*v1*y2 + u0*u3*v0*y1 + u1*u2*v1*y0 + u0*u1*v1*y3 + u0*u2*v0*y3 + u0*u2*v2*y1 - u0*u3*v0*y2 - u1*u2*v2*y0 - u1*u3*v1*y0
		- u0*u2*v2*y3 - u0*u3*v3*y1 - u1*u2*v1*y3 + u1*u3*v1*y2 + u1*u3*v3*y0 + u2*u3*v2*y0 + u0*u3*v3*y2 + u1*u2*v2*y3 - u2*u3*v2*y1 - u2*u3*v3*y0 - u1*u3*v3*y2 + u2*u3*v3*y1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c7 = (u0*v1*y2 - u0*v2*y1 - u1*v0*y2 + u1*v2*y0 + u2*v0*y1 - u2*v1*y0 - u0*v1*y3 + u0*v3*y1 + u1*v0*y3 - u1*v3*y0 - u3*v0*y1 + u3*v1*y0
		+ u0*v2*y3 - u0*v3*y2 - u2*v0*y3 + u2*v3*y0 + u3*v0*y2 - u3*v2*y0 - u1*v2*y3 + u1*v3*y2 + u2*v1*y3 - u2*v3*y1 - u3*v1*y2 + u3*v2*y1)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	c8 = (u0*u1*v0*v2*y3 - u0*u1*v0*v3*y2 - u0*u2*v0*v1*y3 + u0*u2*v0*v3*y1 + u0*u3*v0*v1*y2 - u0*u3*v0*v2*y1 - u0*u1*v1*v2*y3 + u0*u1*v1*v3*y2 + u1*u2*v0*v1*y3 - u1*u2*v1*v3*y0 - u1*u3*v0*v1*y2 + u1*u3*v1*v2*y0
		+ u0*u2*v1*v2*y3 - u0*u2*v2*v3*y1 - u1*u2*v0*v2*y3 + u1*u2*v2*v3*y0 + u2*u3*v0*v2*y1 - u2*u3*v1*v2*y0 - u0*u3*v1*v3*y2 + u0*u3*v2*v3*y1 + u1*u3*v0*v3*y2 - u1*u3*v2*v3*y0 - u2*u3*v0*v3*y1 + u2*u3*v1*v3*y0)
		/ (u0*u1*v0*v2 - u0*u2*v0*v1 - u0*u1*v0*v3 - u0*u1*v1*v2 + u0*u3*v0*v1 + u1*u2*v0*v1 + u0*u1*v1*v3 + u0*u2*v0*v3 + u0*u2*v1*v2 - u0*u3*v0*v2 - u1*u2*v0*v2 - u1*u3*v0*v1
		- u0*u2*v2*v3 - u0*u3*v1*v3 - u1*u2*v1*v3 + u1*u3*v0*v3 + u1*u3*v1*v2 + u2*u3*v0*v2 + u0*u3*v2*v3 + u1*u2*v2*v3 - u2*u3*v0*v3 - u2*u3*v1*v2 - u1*u3*v2*v3 + u2*u3*v1*v3);

	CImg<float> res(PAPER_WIDTH, PAPER_HEIGHT, 1, src_img.spectrum(), 0);

	for (int cur_x = 0; cur_x < PAPER_WIDTH; cur_x++) {
		for (int cur_y = 0; cur_y < PAPER_HEIGHT; cur_y++) {
			double src_x = c1 * cur_x + c2 * cur_y + c3 * cur_x * cur_y + c4;
			double src_y = c5 * cur_x + c6 * cur_y + c7 * cur_x * cur_y + c8;

			double src_col = src_x, src_row = src_img.height() - src_y - 1;
			int cur_col = cur_x, cur_row = PAPER_HEIGHT - cur_y - 1;

			for (int channel = 0; channel < src_img.spectrum(); channel++) {
				res(cur_col, cur_row, channel) = bilinear_interpolation(src_img, src_col, src_row, channel);
			}

		}
	}
	
	return res;
}



void drawVertex(CImg<float>& src_img, CImg<float> &gray) {
	CImg<unsigned char> edge = getEdgeBySegmentation(gray);
	// CImg<unsigned char> edge("res4/edge_5.jpg");

	vector<HoughPoint> hough_param = getHoughParamFromEdge(edge);

	vector<HoughPoint> lines_param = getLinesFromHoughPara(hough_param, 4);

	vector<Point> vertexs = getVertexFromLineParam(lines_param);

	for (int i = 0; i < vertexs.size(); i++) {
		int x_pos = vertexs[i].x;
		int y_pos = vertexs[i].y;

		for (int j = x_pos - 10; j <= x_pos + 10; j++) {
			if (j < 0 || j >= src_img.width()) {
				continue;
			}

			for (int k = y_pos - 10; k <= y_pos + 10; k++) {
				if (k < 0 || k >= src_img.height()) {
					continue;
				}

				src_img(j, src_img.height() - 1 - k, 0) = 255;
				src_img(j, src_img.height() - 1 - k, 1) = 0;
				src_img(j, src_img.height() - 1 - k, 2) = 0;
			}
		}
	}

}
