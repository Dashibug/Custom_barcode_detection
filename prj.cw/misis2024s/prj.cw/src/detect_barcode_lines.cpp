#include "detect_barcode_lines.hpp"
#include "obtain_edge_map.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

const int line_thickness = 2;
const int num_thetas = 180;

const std::vector<double> thetas = []() {
    std::vector<double> result(num_thetas);
    for (int i = 0; i < num_thetas; ++i) {
        result[i] = static_cast<double>(i - 90) * CV_PI / 180.0;
    }
    return result;
}();
const std::vector<double> cos_theta = []() {
    std::vector<double> result(num_thetas);
    for (int i = 0; i < num_thetas; ++i) {
        result[i] = std::cos(thetas[i]);
    }
    return result;
}();
const std::vector<double> sin_theta = []() {
    std::vector<double> result(num_thetas);
    for (int i = 0; i < num_thetas; ++i) {
        result[i] = std::sin(thetas[i]);
    }
    return result;
}();

void detect_barcode_lines(cv::Mat image1, cv::Mat image2) {
    
    cv::Mat original_img = image1;
    cv::Mat detected_img = image2;

    if (original_img.empty() || detected_img.empty()) {
        std::cerr << "Could not load images!" << std::endl;
        return;
    }

    auto edge_map = obtain_edge_map(original_img);
    cv::Mat edges = edge_map.first;
    cv::imshow("edges", edges);
    cv::Mat plot_input = edge_map.second;
    cv::imshow("plot_input", plot_input);
    cv::Mat masked_img;
    cv::cvtColor(edges, edges, cv::COLOR_GRAY2RGB);
    cv::bitwise_and(detected_img, edges, masked_img);
    cv::imshow("masked_img", masked_img);
    cv::cvtColor(masked_img, masked_img, cv::COLOR_RGB2GRAY);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(masked_img, lines, 1, CV_PI / 180, 50, 50, 10);

    for (size_t i = 0; i < lines.size(); ++i) {
        cv::Vec4i l = lines[i];
        cv::line(original_img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), line_thickness);
        cv::line(detected_img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), line_thickness);
    }

    cv::Rect barcode_rect;
    if (!lines.empty()) {
        std::vector<cv::Point> points;
        for (const auto& line : lines) {
            points.push_back(cv::Point(line[0], line[1]));
            points.push_back(cv::Point(line[2], line[3]));
        }
        barcode_rect = cv::boundingRect(points);

        cv::rectangle(original_img, barcode_rect, cv::Scalar(0, 255, 0), 2);
        cv::rectangle(detected_img, barcode_rect, cv::Scalar(0, 255, 0), 2);
    }

    cv::resize(detected_img, detected_img, original_img.size());
    cv::resize(plot_input, plot_input, original_img.size());

    cv::Mat concatenated_img;
    cv::Mat concatenated_img_2;
    cv::Mat result;

    cv::Mat plot_input_copy = plot_input.clone();
    cv::hconcat(plot_input_copy, original_img, concatenated_img);
    cv::hconcat(plot_input, detected_img, concatenated_img_2);
    cv::hconcat(concatenated_img, concatenated_img_2, result);

    cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
    cv::imshow("Result", result);
    cv::waitKey(0);
}
