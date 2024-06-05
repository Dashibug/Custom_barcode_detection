#ifndef OBTAIN_EDGE_MAP_HPP
#define OBTAIN_EDGE_MAP_HPP

#include <opencv2/opencv.hpp>
#include <utility>

std::pair<cv::Mat, cv::Mat> obtain_edge_map(const cv::Mat& img);

#endif // OBTAIN_EDGE_MAP_HPP
