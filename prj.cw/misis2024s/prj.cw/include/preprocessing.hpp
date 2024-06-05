#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <fstream>

using namespace cv;
using namespace std;


vector<vector<int>> myRect;
vector<vector<int>> idealRect;
vector<vector<double>> programRezult;
int idealIndx = 0;
int myIndx = 0;
int hight = 600;
int width = 300;

void readJSON(Mat img, int k) {

	FileStorage fs("C:/Users/megre/OneDrive/Desktop/pr/dataset/resource/parsed_data.json", 0);
	auto itBegin = fs.root().begin();
	//auto itEnd = fs.root().end();
	while (k - 1 != 0) {
		itBegin++;
		k--;
	}
	std::vector<cv::Point> points;

	for (int i = 0; i < (*itBegin)["regions"].size(); i++) {
		for (int j = 0; j < 4; j++) {
			int x = (int)(*itBegin)["regions"][i]["shape_attributes"]["all_points_x"][j];
			int y = (int)(*itBegin)["regions"][i]["shape_attributes"]["all_points_y"][j];
			points.push_back(cv::Point(x, y));
		}
	}

	cv::polylines(img, points, true, cv::Scalar(255,255,255), 8);
	cv::imshow("TEST", img);
	fs.release();
}

#pragma once
