#include "obtain_edge_map.hpp"
#include <opencv2/opencv.hpp>
#include <algorithm> // For std::min and std::max

const double sigma = 0.3;

std::pair<cv::Mat, cv::Mat> obtain_edge_map(const cv::Mat& img) {
    cv::Mat gray_img;
    cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
    cv::Mat blurred;
    cv::medianBlur(gray_img, blurred, 7);
    cv::imshow("blurred", blurred);
    cv::Scalar median = cv::mean(img);
    double lower_threshold = std::max(0.0, (1.0 - sigma) * median[0]);
    double upper_threshold = std::min(255.0, (1.0 + sigma) * median[0]);
    cv::Mat edges;
    cv::Canny(blurred, edges, lower_threshold, upper_threshold);
    cv::Mat color_edges;
    cv::cvtColor(edges, color_edges, cv::COLOR_GRAY2RGB);
    cv::Mat plot_input;
    cv::hconcat(img, color_edges, plot_input);
    return { edges, plot_input };
}
