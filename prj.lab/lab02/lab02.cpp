#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>


cv::Mat generateTestImage(int level1, int level2, int level3, int inner_square_size, int radius) {
    cv::Mat image(256, 256, CV_8UC1, cv::Scalar(255));
    cv::Rect square1(0, 0, 256, 256);
    rectangle(image, square1, cv::Scalar(0), -1);
    cv::Rect square2((256 - inner_square_size) / 2, (256 - inner_square_size) / 2, inner_square_size, inner_square_size);
    rectangle(image, square2, cv::Scalar(level1), -1);
    cv::Point circle_center(256 / 2, 256 / 2);
    circle(image, circle_center, radius, cv::Scalar(level2), -1);

    return image;
}

cv::Mat drawHistogram(const cv::Mat& image) {
    cv::Mat hist_image(256, 256, CV_8UC1, cv::Scalar(230));
    cv::Mat hist;
    int hist_size = 256;
    float range[] = { 0, 256 };
    const float* hist_range = { range };
    calcHist(&image, 1, nullptr, cv::Mat(), hist, 1, &hist_size, &hist_range);
    normalize(hist, hist, 0, hist_image.rows, cv::NORM_MINMAX, -1, cv::Mat());
    for (int i = 0; i < 256; ++i) {
        line(hist_image, cv::Point(i, hist_image.rows), cv::Point(i, hist_image.rows - hist.at<float>(i)), cv::Scalar(0));
    }

    return hist_image;
}

cv::Mat addGaussianNoise(const cv::Mat& image, double mean, double stddev) {
    cv::Mat noise(image.size(), CV_8UC1);
    cv::randn(noise, mean, stddev);
    cv::Mat noisy_image;
    add(image, noise, noisy_image, cv::Mat(), CV_8UC1);

    return noisy_image;
}

int main() {

    cv::Mat test_image1 = generateTestImage(0, 127, 255, 209, 83);
    cv::Mat test_image2 = generateTestImage(20, 127, 235, 209, 83);
    cv::Mat test_image3 = generateTestImage(55, 127, 200, 209, 83);
    cv::Mat test_image4 = generateTestImage(90, 127, 165, 209, 83);

    std::vector<double> stddevs = { 3, 7, 15 };
    std::vector<cv::Mat> noisyImages;
    for (double stddev : stddevs) {
        noisyImages.push_back(addGaussianNoise(test_image1, 0, stddev));
        noisyImages.push_back(addGaussianNoise(test_image2, 0, stddev));
        noisyImages.push_back(addGaussianNoise(test_image3, 0, stddev));
        noisyImages.push_back(addGaussianNoise(test_image4, 0, stddev));
    }

    std::vector<cv::Mat> noisyHistograms;
    for (const auto& image : noisyImages) {
        noisyHistograms.push_back(drawHistogram(image));
    }

    cv::namedWindow("Result", cv::WINDOW_NORMAL); // Режим WINDOW_NORMAL позволяет изменять размер окна

    cv::Mat resultImage(256 * 5, 256 * (stddevs.size() * 4 + 1), CV_8UC1, cv::Scalar(255));
    for (int i = 0; i < stddevs.size() * 4; ++i) {
        cv::Mat roi = resultImage(cv::Rect(256 * (i + 1), 0, 256, 256));
        test_image1.copyTo(roi);
        drawHistogram(test_image1).copyTo(resultImage(cv::Rect(256 * (i + 1), 256, 256, 256)));
        test_image2.copyTo(roi);
        drawHistogram(test_image2).copyTo(resultImage(cv::Rect(256 * (i + 1), 256, 256, 256)));
        test_image3.copyTo(roi);
        drawHistogram(test_image3).copyTo(resultImage(cv::Rect(256 * (i + 1), 256, 256, 256)));
        test_image4.copyTo(roi);
        drawHistogram(test_image4).copyTo(resultImage(cv::Rect(256 * (i + 1), 256, 256, 256)));

        line(resultImage, cv::Point(256 * (i + 1), 0), cv::Point(256 * (i + 1), resultImage.rows), cv::Scalar(100));
    }
    for (int i = 0; i < stddevs.size() * 4; ++i) {
        noisyImages[i].copyTo(resultImage(cv::Rect(256 * (i + 1), 256 * 2, 256, 256)));
        noisyHistograms[i].copyTo(resultImage(cv::Rect(256 * (i + 1), 256 * 3, 256, 256)));

        line(resultImage, cv::Point(256 * (i + 1), 256 * 2), cv::Point(256 * (i + 1), resultImage.rows), cv::Scalar(100));
    }

    imshow("Result", resultImage);
    cv::resizeWindow("Result", 1920, 1080); // Устанавливаем размер окна равным разрешению экрана
    cv::waitKey(0);

    return 0;
}