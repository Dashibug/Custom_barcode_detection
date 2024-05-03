#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <random>

void add_noise(cv::Mat& image, double noise_level) {

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0, noise_level);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            uchar& pixel_value = image.at<uchar>(i, j);
            pixel_value += distribution(generator);
            if (pixel_value < 0) {
                pixel_value = 0;
            }
            else if (pixel_value > 255) {
                pixel_value = 255;
            }
        }
    }
}

cv::Mat LoG_DoG_Detection(const cv::Mat& image, double sigma1, double sigma2) {
    // Применяем оператор LoG
    cv::Mat blurred1, log_image;
    cv::GaussianBlur(image, blurred1, cv::Size(0, 0), sigma1);
    cv::Laplacian(blurred1, log_image, CV_64F);

    return log_image;
}

std::pair<cv::Mat, cv::Mat> generated_images(std::vector<int> radiuses, std::vector<int> smoothing) {
    size_t sz = *std::max_element(radiuses.begin(), radiuses.end()) * 2 + 30;
    cv::Mat result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1, 70);
    cv::Mat binary_result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1);
    int n = radiuses.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cv::Mat new_image(sz, sz, CV_8UC1, 70);
            cv::circle(new_image, cv::Point(sz / 2, sz / 2), radiuses[j], 110, -1);
            if (i > 0) {
                cv::Mat blurry_image;
                cv::GaussianBlur(new_image, new_image, cv::Size(smoothing[i], smoothing[i]), 0);
            }
            add_noise(new_image, 6);

            cv::Mat binary_new_image(sz, sz, CV_8UC1);
            cv::adaptiveThreshold(new_image, binary_new_image, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 23, -8);
            binary_new_image.copyTo(binary_result(cv::Rect(sz * j, sz * i, sz, sz)));
            new_image.copyTo(result(cv::Rect(sz * j, sz * i, sz, sz)));
        }
    }
    return { result, binary_result };
}


int main() {

    std::vector<int> radiuses = { 10, 20, 30, 40, 50 }; // Пример радиусов для кругов
    std::vector<int> smoothing = { 3, 5, 7, 9, 11 }; // Пример параметров размытия

    // Генерируем изображения
    std::pair<cv::Mat, cv::Mat> images = generated_images(radiuses, smoothing);
 
    double sigma1 = 1.0;
    double sigma2 = 2.5;
    cv::Mat LoG_result = LoG_DoG_Detection(images.second, sigma1, sigma2);

    
    // Отображаем сгенерированные изображения
    cv::imshow("Result Image", images.first); // Отображаем изображение с кругами
    cv::imshow("Binary Image", images.second); // Отображаем бинарное изображение
    cv::imshow("Detected Image", LoG_result);
    cv::waitKey(0);


    return 0;
}