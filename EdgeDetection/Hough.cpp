#include "stdafx.h"
#include "Hough.h"
#include "draw2d.h"
#include "Canny.h"
#include <cmath>

void initSinAndCosValue() {
	if (flag)
		return;

	float PI = 3.1415926;
	for (int i = 0; i < 360; i++) {
		sin_val[i] = sin(i * PI / 180);
		cos_val[i] = cos(i * PI / 180);
	}
	flag = true;
}

bool operator<(const HoughPoint &x, const HoughPoint &y) {
	return x.vote > y.vote;
}

float houghPointDistance(const HoughPoint& a, const HoughPoint& b) {
	return abs(a.theta - b.theta) + abs(a.p - b.p);
}

HoughPoint getNewCenter(const vector<HoughPoint>& cluster_points) {
	assert(cluster_points.size() > 0);

	int sum_of_theta = 0;
	int sum_of_p = 0;
	int sum_of_weight = 0;

	for (int i = 0; i < cluster_points.size(); i++) {
		int weight = cluster_points[i].vote;

		sum_of_theta += (cluster_points[i]).theta * weight;
		sum_of_p += (cluster_points[i]).p * weight;
		sum_of_weight += weight;
	}

	HoughPoint new_center;
	new_center.theta = (float)sum_of_theta / sum_of_weight;
	new_center.p = (float)sum_of_p / sum_of_weight;

	return new_center;
}

bool equal(const vector<HoughPoint>& old_center, const vector<HoughPoint>& new_center) {
	if (new_center.size() != old_center.size()) {
		return false;
	}

	for (int i = 0; i < new_center.size(); i++) {
		if (abs((new_center[i]).theta - (old_center[i]).theta) > 0.01 || abs((new_center[i]).p != (old_center[i]).p) > 0.01) {
			return false;
		}
	}

	return true;
}

CImg<int> getHoughSpace(const CImg<unsigned char>& edge) {
	int dst_height = ceil(sqrt(edge.width() * edge.width() + edge.height() * edge.height()));
	CImg<int> res(360, dst_height, 1, 1, 0);

	int src_width = edge.width();
	int src_height = edge.height();

	initSinAndCosValue();
	for (int i = 0; i < src_width; i++) {
		for (int j = 0; j < src_height; j++) {
			if (edge(i, j) == 255) {
				drawHoughCurve(res, i, src_height - 1 - j);
			}
		}
	}

	return res;
}

bool validCenter(const vector<HoughPoint>& center_points, const HoughPoint& new_center_point) {
	for (int i = 0; i < center_points.size(); i++) {
		if (houghPointDistance(center_points[i], new_center_point) < 70) {
			return false;
		}
	}

	return true;
}

// Try to find by geometry
// Failed

//vector<HoughPoint> getHoughPara(const CImg<float>& houghSpace, int num) {
//	srand(time(0));
//
//	int src_width = houghSpace.width();
//	int src_height = houghSpace.height();
//
//	int max = 0;
//	for (int i = 0; i < src_width; i++) {
//		for (int j = 0; j < src_height; j++) {
//			if (houghSpace(i, j) > max) {
//				max = houghSpace(i, j);
//			}
//		}
//	}
//
//	float threshold = max * 0.2;
//
//	// Find all none zero points to avoid duplicate traversal.
//	vector<HoughPoint> non_zero_points;
//	for (int i = 0; i < src_width; i++) {
//		for (int j = 0; j < src_height; j++) {
//			if (houghSpace(i, j) > threshold) {
//				HoughPoint new_point;
//				new_point.theta = i;
//				new_point.p = src_height - 1 - j;
//				new_point.vote = houghSpace(i, j);
//
//				non_zero_points.push_back(new_point);
//			}
//		}
//	}
//
//	sort(non_zero_points.begin(), non_zero_points.end());
//
//	vector<HoughPoint> para_points;
//	for (int first = 0; first < non_zero_points.size() - 3; first++) {
//		para_points.clear();
//
//		// Divide non zero points into two groups.
//		vector<HoughPoint> candidate_parallel_para_points;
//		vector<HoughPoint> candidate_vertical_para_points;
//
//		// Suppose that the first point must be the first edge of the rectange.
//		HoughPoint first_para_point = non_zero_points[first];
//
//		// cout << first_para_point.theta << "," << first_para_point.p << endl;
//
//		para_points.push_back(first_para_point);
//
//		for (int i = first + 1; i < non_zero_points.size(); i++) {
//			HoughPoint candidate_para = non_zero_points[i];
//
//			// If the second line is parallel to the first line.
//			if (abs(candidate_para.theta - first_para_point.theta) < 10) {
//				// Use the distance between two lines as new vote value.
//				if (abs(candidate_para.p - first_para_point.p) > 50) {
//					candidate_para.vote = abs(first_para_point.p - candidate_para.p);
//					candidate_parallel_para_points.push_back(candidate_para);
//				}
//			}
//			else if ((abs(candidate_para.theta - first_para_point.theta) > 85
//				&& abs(candidate_para.theta - first_para_point.theta) < 95)
//				|| (abs(candidate_para.theta - first_para_point.theta) > 265
//				&& abs(candidate_para.theta - first_para_point.theta) < 275))
//			{
//				candidate_vertical_para_points.push_back(candidate_para);
//			}
//		}
//
//		if (candidate_parallel_para_points.size() > 0) {
//			sort(candidate_parallel_para_points.begin(), candidate_parallel_para_points.end());
//
//			HoughPoint second_para_point = candidate_parallel_para_points[0];
//			if (second_para_point.vote != 0) {
//				para_points.push_back(second_para_point);
//			}
//		}
//
//		if (candidate_vertical_para_points.size() > 0) {
//			HoughPoint third_para_point = candidate_vertical_para_points[0];
//
//			para_points.push_back(third_para_point);
//			// cout << third_para_point.theta << "," << third_para_point.p << endl;
//			for (int i = 1; i < candidate_vertical_para_points.size(); i++) {
//				HoughPoint candidate_para = candidate_vertical_para_points[i];
//
//				// Use the distance between two lines as new vote value.
//				if (abs(candidate_para.theta - third_para_point.theta) < 10) {
//					if (abs(candidate_para.p - third_para_point.p) > 50) {
//						candidate_vertical_para_points[i].vote = abs(third_para_point.p - candidate_vertical_para_points[i].p);
//					}
//					else {
//						candidate_vertical_para_points[i].vote = 0;
//					}
//				}
//				else {
//					candidate_vertical_para_points[i].vote = 0;
//				}
//			}
//
//			sort(candidate_vertical_para_points.begin() + 1, candidate_vertical_para_points.end());
//
//			if (candidate_vertical_para_points.size() > 1) {
//				HoughPoint fourth_para_point = candidate_vertical_para_points[1];
//				if (fourth_para_point.vote != 0) {
//					// cout << fourth_para_point.theta << "," << fourth_para_point.p << endl;
//					para_points.push_back(fourth_para_point);
//				}
//			}
//		}
//
//		if (para_points.size() == 4) {
//			return para_points;
//		}
//	}
//
//	return para_points;
//}


// K-means
//vector<HoughPoint> getHoughPara(const CImg<int>& houghSpace, int num) {
//	srand(time(0));
//
//	int src_width = houghSpace.width();
//	int src_height = houghSpace.height();
//
//	int max = 0;
//	for (int i = 0; i < src_width; i++) {
//		for (int j = 0; j < src_height; j++) {
//			if (houghSpace(i, j) > max) {
//				max = houghSpace(i, j);
//			}
//		}
//	}
//
//	int threshold = max * HOUGH_THRESHOLD_FACTOR;
//
//	// Find all none zero points to avoid duplicate traversal.
//	vector<HoughPoint> non_zero_points;
//	for (int i = 0; i < src_width; i++) {
//		for (int j = 0; j < src_height; j++) {
//			if (houghSpace(i, j) > threshold) {
//				HoughPoint new_point;
//				new_point.theta = i;
//				new_point.p = src_height - 1 - j;
//				new_point.vote = houghSpace(i, j);
//
//				non_zero_points.push_back(new_point);
//			}
//		}
//	}
//
//	sort(non_zero_points.begin(), non_zero_points.end());
//
//	// Initialize center points.
//	vector<HoughPoint> center_points(num);
//	vector<vector<HoughPoint>> cluster_points(num);
//
//	for (int i = 0; i < num; i++) {
//		HoughPoint random_start_center(random(0, src_width - 1), random(0, src_height - 1));
//
//		center_points[i] = random_start_center;
//	}
//
//	vector<HoughPoint> old_center_points;
//	int count = 0;
//	while (count < 100 && !equal(center_points, old_center_points)) {
//		for (int i = 0; i < non_zero_points.size(); i++) {
//			HoughPoint cur_point = non_zero_points[i];
//			HoughPoint cur_center = center_points[0];
//			float min_distance = houghPointDistance(cur_point, cur_center);
//			int closet_center_num = 0;
//
//			for (int j = 1; j < num; j++) {
//				cur_center = center_points[j];
//				if (houghPointDistance(cur_point, cur_center) < min_distance) {
//					min_distance = houghPointDistance(cur_point, cur_center);
//					closet_center_num = j;
//				}
//			}
//
//			(cluster_points[closet_center_num]).push_back(cur_point);
//		}
//
//		old_center_points = center_points;
//
//		// Update center points.
//		for (int i = 0; i < num; i++) {
//			if (cluster_points[i].size() > 0) {
//				center_points[i] = getNewCenter(cluster_points[i]);
//			}
//			else {
//				HoughPoint new_center_point;
//				new_center_point.theta = random(0, src_width - 1);
//				new_center_point.p = random(0, src_height - 1);
//
//				center_points[i] = new_center_point;
//			}
//			(cluster_points[i]).clear();
//		}
//
//		count++;
//	}
//
//	if (count > 100) {
//		cout << "Failed" << endl;
//		center_points.clear();
//	}
//
//	for (int i = 0; i < center_points.size(); i++) {
//		cout << center_points[i].theta << "," << center_points[i].p << endl;
//	}
//
//	return center_points;
//}

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

vector<HoughPoint> getHoughPara(const CImg<int>& houghSpace, int num) {
	srand(time(0));

	int src_width = houghSpace.width();
	int src_height = houghSpace.height();

	int max = 0;
	for (int i = 0; i < src_width; i++) {
		for (int j = 0; j < src_height; j++) {
			if (houghSpace(i, j) > max) {
				max = houghSpace(i, j);
			}
		}
	}

	int threshold = max * HOUGH_THRESHOLD_FACTOR;

	// Find all none zero points to avoid duplicate traversal.
	vector<HoughPoint> non_zero_points;
	for (int i = 0; i < src_width; i++) {
		for (int j = 0; j < src_height; j++) {
			if (houghSpace(i, j) > threshold) {
				HoughPoint new_point;
				new_point.theta = i;
				new_point.p = src_height - 1 - j;
				new_point.vote = houghSpace(i, j);

				non_zero_points.push_back(new_point);
			}
		}
	}

	sort(non_zero_points.begin(), non_zero_points.end());
	for (int i = 0; i < non_zero_points.size(); i++) {
		cout << non_zero_points[i].theta << "," << non_zero_points[i].p << "," << non_zero_points[i].vote << endl;
	}

	// Initialize center points.

	int count = 0;
	vector<HoughPoint> center_points(num);
	for (int i = 0; i < non_zero_points.size(); i++) {
		if (!containCurPoint(center_points, non_zero_points[i])) {
			center_points[count] = non_zero_points[i];
			count++;

			if (count == 4) {
				for (int i = 0; i < center_points.size(); i++) {
					cout << center_points[i].theta << "," << center_points[i].p << "," << center_points[i].vote << endl;
				}

				return center_points;
			}
		}
	}

	for (int i = 0; i < center_points.size(); i++) {
		cout << center_points[i].theta << "," << center_points[i].p << "," << center_points[i].vote << endl;
	}

	return center_points;
}


void drawLineAndCornerFromHoughPara(CImg<float>& image, const vector<HoughPoint>& hough_para) {
	initSinAndCosValue();

	int src_width = image.width();
	int src_height = image.height();

	for (int i = 0; i < src_width; i++) {
		for (int j = 0; j < src_height; j++) {
			int x = i;
			int y = src_height - 1 - j;

			int count = 0;
			for (int k = 0; k < hough_para.size(); k++) {
				int cur_theta = (hough_para[k]).theta;
				int cur_p = (hough_para[k]).p;

				float res = x * cos_val[cur_theta] + y * sin_val[cur_theta];

				if (abs(res - cur_p) < 3) {
					image(i, j, 0) = 0;
					image(i, j, 1) = 255;
					image(i, j, 2) = 0;

					count++;
				}
			}

			if (count > 1) {
				image(i, j, 0) = 255;
				image(i, j, 1) = 0;
				image(i, j, 2) = 0;


			}
		}
	}
}

void fineLineFromHough(CImg<float>& src_img, CImg<unsigned char>& edge, int line_num) {
	CImg<float> hough_space = getHoughSpace(edge);
	vector<HoughPoint> hough_para = getHoughPara(hough_space, line_num);

	drawLineAndCornerFromHoughPara(src_img, hough_para);
}