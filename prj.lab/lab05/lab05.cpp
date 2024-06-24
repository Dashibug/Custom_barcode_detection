#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
	vector<vector<int>> colour = { {0, 127},
									{0, 255 },
									{127, 0},
									{127, 255},
									{255, 0},
									{255, 127}
	};
	int indx = 0;
	int r = 25;
	Mat rezult;

	for (int i = 0; i < 2; i++) {
		Mat total;
		for (int k = 0; k < 3; k++) {
			Mat image(99, 99, CV_8UC1, Scalar(colour[k + indx][0]));
			circle(image, Point(49, 49), r, Scalar(colour[k + indx][1]), FILLED);
			if (k != 0) { hconcat(total, image, total); }
			else {
				image.copyTo(total);
			}
		}
		if (i != 0) { vconcat(rezult, total, rezult); }
		else { total.copyTo(rezult); }
		indx = indx + 3;
	}
	imshow("image_1", rezult);
	waitKey(0);

	Mat rezultOne, rezultTwo, rezulthree;
	Mat kernelOne = (Mat_<int>(2, 2) <<
		1, 0,
		0, -1);
	Mat kernelT = (Mat_<int>(2, 2) <<
		0, 1,
		-1, 0);

	rezult.convertTo(rezult, CV_32FC1);
	filter2D(rezult, rezultOne, -1, kernelOne);
	filter2D(rezult, rezultTwo, -1, kernelT);


	rezultTwo.copyTo(rezulthree);
	for (int i = 0; i < rezult.rows; i++) {
		for (int k = 0; k < rezult.cols; k++) {
			rezulthree.at<float>(i, k) = sqrt(pow(rezultTwo.at<float>(i, k), 2) + pow(rezultOne.at<float>(i, k), 2));
		}
	}
	normalize(rezultTwo, rezultTwo, 0, 175, NORM_MINMAX, -1, Mat());
	normalize(rezultOne, rezultOne, 0, 175, NORM_MINMAX, -1, Mat());
	normalize(rezulthree, rezulthree, 0, 175, NORM_MINMAX, -1, Mat());
	Mat threeChannelImage;
	vector<Mat> channels;

	channels.push_back(rezultOne);
	channels.push_back(rezultTwo);
	channels.push_back(rezulthree);
	merge(channels, threeChannelImage);
	threeChannelImage.convertTo(threeChannelImage, CV_8UC3);
	imshow("final_image", threeChannelImage);
	waitKey(0);
	return 0;
}