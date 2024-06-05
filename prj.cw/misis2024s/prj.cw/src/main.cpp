#include "detect_barcode_lines.hpp"
#include <opencv2/opencv.hpp>
#include "preprocessing.hpp"
#include "Otsenka_kachestva.hpp"

using namespace std;
using namespace cv;


int main(int argc, char** argv) {
   
    cv::Mat original_img = imread("C:/Users/megre/OneDrive/Desktop/pr/dataset/Original_Subset/14.png");
    cv::Mat detected_img = imread("C:/Users/megre/OneDrive/Desktop/pr/dataset/Detection_Subset/14.png");
	readJSON(original_img, 14);// не забыть поменять при необходимости
    detect_barcode_lines(original_img, detected_img);
}
