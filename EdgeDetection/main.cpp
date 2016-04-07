// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CImg.h"
#include "Canny.h"
#include "draw2d.h"
#include "Hough.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cimg_library;

string getString(const int n){
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

template <class T>
CImg<T> getGray(const CImg<T>& src_img) {
	assert(src_img.spectrum() == 3);
	int dst_width = src_img.width();
	int dst_height = src_img.height();

	CImg<T> res(dst_width, dst_height, 1, 1, 0);

	for (int col = 1; col < dst_width - 1; col++) {
		for (int row = 1; row < dst_height - 1; row++) {
			res(col, row) = 0.299 * src_img(col, row, 0) + 0.587 * src_img(col, row, 1) + 0.114 * src_img(col, row, 2);
		}
	}

	return res;
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < 6; i++) {
		string s1("img/");
		string s2(".jpg");

		CImg<float> image((s1 + getString(i) + s2).c_str());
		CImg<float> gray = getGray(image);

		CImg<unsigned char> final_res = cannyEdgeDection(gray);

		// CImg<unsigned char> edge = cannyEdgeDection(gray);
		//cout << "edge" << endl;

		//string k1("res/canny_");
		//string k2(".jpg");

		//edge.save((k1 + getString(i) + k2).c_str());

		fineLineFromHough(image, final_res, 4);
		image.display();
		//string t1("res4/final_");
		//string t2(".jpg");
		//image.save((t1 + getString(i) + t2).c_str());

		//cout << i << endl;
	}

	int t;
	cin >> t;

	return 0;
}

