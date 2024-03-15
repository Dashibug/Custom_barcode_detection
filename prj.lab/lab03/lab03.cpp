#include <opencv2/opencv.hpp>
#include <iostream>


void contrast_per_channel(cv::Mat& channel) {
    double min_val, max_val;
    cv::minMaxLoc(channel, &min_val, &max_val);
    channel = (channel - min_val) * 255.0 / (max_val - min_val);//распределение значений яркости в канале
}

void image_contrast(cv::Mat& image) {
    std::vector<cv::Mat> channels;
    //разбивка изображения по каналам
    cv::split(image, channels);
    for (int i = 0; i < image.channels(); i++) {
        contrast_per_channel(channels[i]);
    }
    cv::merge(channels, image);
}

void contrast_histogram(cv::Mat& original, double black_q, double white_q, cv::Mat& current) {
    cv::Mat hist;
    int hist_size = 256;
    float range[] = { 0, 256 };
    const float* hist_range = { range };
    calcHist(&original, 1, 0, cv::Mat(), hist, 1, &hist_size, &hist_range, true, false);

    cv::Mat new_cdf;
    hist.copyTo(new_cdf);
    for (int i = 1; i < hist_size; i++) {
        new_cdf.at<float>(i) += new_cdf.at<float>(i - 1);
    }


    float total_pixels = original.rows * original.cols;
    float black_pixel = black_q * total_pixels;
    float white_pixel = white_q * total_pixels;

    int black_level = 0, white_level = 255;
    for (int i = 0; i < hist_size; ++i) {
        if (new_cdf.at<float>(i) >= black_pixel) {
            black_level = i;
            break;
        }
    }

    for (int i = hist_size - 1; i >= 0; --i) {
        if (new_cdf.at<float>(i) <= white_pixel) {
            white_level = i;
            break;
        }
    }

    //коррекция контраста
    double contrast = 255.0 / (white_level - black_level);
    double brightness = -black_level * contrast;

    original.convertTo(current, -1, contrast, brightness);
}


int main() {

    cv::Mat image = cv::imread("path");

    if (image.empty()) {
        std::cout << "Could not open or find the image!" << std::endl;
        return -1;
    }

    cv::Mat current;
    if (image.channels() == 1) {
        contrast_histogram(image, 0.5, 0.5, current);
    }
    else if (image.channels() == 3) {
        current = image.clone();
        image_contrast(current);
    }
    else {
        std::cerr << "Uncorrect image" << std::endl;
    }
    cv::imshow("Original", image);
    cv::imshow("Current", current);
    cv::waitKey(0);
    return 0;
}