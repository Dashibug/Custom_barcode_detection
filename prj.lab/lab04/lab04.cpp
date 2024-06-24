#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <random>

int binary1 = 5;
int binary2 = 1;

void BernsenThreshHold(cv::Mat& image, float apertureSize, float contrastThreshold, std::string fileName = "") {
    int rows = image.rows;
    int cols = image.cols;
    int apertureSizeHalf = static_cast<int>(apertureSize / 2);
    int contrastThreshold255 = static_cast<int>(contrastThreshold * 255);

    cv::Mat binarizedImage = image.clone();

    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            int minVal = 255;
            int maxVal = 0;

            // Step 1: Find Min and Max within the aperture
            for (int i = -apertureSizeHalf; i <= apertureSizeHalf; i++) {
                for (int j = -apertureSizeHalf; j <= apertureSizeHalf; j++) {
                    int newX = x + i;
                    int newY = y + j;

                    if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) {
                        int currVal = image.at<uchar>(newX, newY);
                        minVal = std::min(minVal, currVal);
                        maxVal = std::max(maxVal, currVal);
                    }
                }
            }

            // Step 2: Calculate Avg
            int avg = (minVal + maxVal) / 2;
            int threshold = avg;
            if (maxVal - minVal < contrastThreshold255) {
                threshold = 128;
            }

            // Step 3: Binarize the current pixel
            if (image.at<uchar>(x, y) >= threshold) {
                binarizedImage.at<uchar>(x, y) = 255;  // set pixel to white
            }
            else {
                binarizedImage.at<uchar>(x, y) = 0;    // set pixel to black
            }
        }
    }

    image = binarizedImage;

    if (!fileName.empty()) {
        cv::imwrite(fileName + ".jpg", image);
    }
}

void BernsenThreshHoldGui(int, void* data) {
    cv::Mat image = *static_cast<cv::Mat*>(data);

    float binary[2] = { static_cast<float>(binary1), static_cast<float>(binary2) / 100.0f };

    BernsenThreshHold(image, binary[0], binary[1]);
    cv::imshow("Bernsen", image);
}

// Функция для добавления шума к изображению
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

// Функция для обнаружения краев методом LoG (Laplacian of Gaussian)
cv::Mat LoG_DoG_Detection(const cv::Mat& image, double sigma1, double sigma2) {
    cv::Mat blurred1, log_image;
    cv::GaussianBlur(image, blurred1, cv::Size(0, 0), sigma1);
    cv::Laplacian(blurred1, log_image, CV_64F);

    return log_image;
}

// Функция для генерации изображений
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
                cv::GaussianBlur(new_image, new_image, cv::Size(smoothing[i], smoothing[i]), 0);
            }
            add_noise(new_image, 6);

            cv::Mat binary_new_image(sz, sz, CV_8UC1);
            BernsenThreshHold(new_image, 15.0f, 0.15f); // Используем Бернсена для бинаризации
            new_image.copyTo(binary_new_image);

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

//Вариант бинаризации алгоритмом Брэдли-Рота
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <opencv2/highgui.hpp>
//#include <random>
//
//// Локальная бинаризация алгоритмом Брэдли-Рота
//void BradleyThreshHold(cv::Mat& image, float S, float s) {
//    cv::Mat res = image.clone();
//    int width = image.cols;
//    int height = image.rows;
//
//    int s2 = S / 2;
//    unsigned long* integral_image = 0;
//    long sum = 0;
//    int count = 0;
//    int index;
//    int x1, y1, x2, y2;
//
//    // Рассчитываем интегральное изображение
//    integral_image = new unsigned long[width * height];
//    for (int i = 0; i < width; i++) {
//        sum = 0;
//        for (int j = 0; j < height; j++) {
//            index = j * width + i;
//            sum += image.at<uchar>(j, i);
//            if (i == 0)
//                integral_image[index] = sum;
//            else
//                integral_image[index] = integral_image[index - 1] + sum;
//        }
//    }
//
//    // Находим границы для локальных областей
//    for (int i = 0; i < width; i++) {
//        for (int j = 0; j < height; j++) {
//            index = j * width + i;
//
//            x1 = i - s2;
//            x2 = i + s2;
//            y1 = j - s2;
//            y2 = j + s2;
//
//            if (x1 < 0)
//                x1 = 0;
//            if (x2 >= width)
//                x2 = width - 1;
//            if (y1 < 0)
//                y1 = 0;
//            if (y2 >= height)
//                y2 = height - 1;
//
//            count = (x2 - x1) * (y2 - y1);
//
//            sum = integral_image[y2 * width + x2] - integral_image[y1 * width + x2] -
//                integral_image[y2 * width + x1] + integral_image[y1 * width + x1];
//            if ((long)(image.at<uchar>(j, i) * count) < (long)(sum * (1.0 - s)))
//                res.at<uchar>(j, i) = 0;
//            else
//                res.at<uchar>(j, i) = 255;
//        }
//    }
//
//    delete[] integral_image;
//    image = res;
//}
// 
// Функция для добавления шума к изображению
//void add_noise(cv::Mat& image, double noise_level) {
//    std::default_random_engine generator;
//    std::normal_distribution<double> distribution(0.0, noise_level);
//
//    for (int i = 0; i < image.rows; i++) {
//        for (int j = 0; j < image.cols; j++) {
//            uchar& pixel_value = image.at<uchar>(i, j);
//            pixel_value += distribution(generator);
//            if (pixel_value < 0) {
//                pixel_value = 0;
//            }
//            else if (pixel_value > 255) {
//                pixel_value = 255;
//            }
//        }
//    }
//}
//
//// Функция для обнаружения краев методом LoG (Laplacian of Gaussian)
//cv::Mat LoG_DoG_Detection(const cv::Mat& image, double sigma1, double sigma2) {
//    cv::Mat blurred1, log_image;
//    cv::GaussianBlur(image, blurred1, cv::Size(0, 0), sigma1);
//    cv::Laplacian(blurred1, log_image, CV_64F);
//
//    return log_image;
//}
//
//// Функция для генерации изображений
//std::pair<cv::Mat, cv::Mat> generated_images(std::vector<int> radiuses, std::vector<int> smoothing) {
//    size_t sz = *std::max_element(radiuses.begin(), radiuses.end()) * 2 + 30;
//    cv::Mat result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1, 70);
//    cv::Mat binary_result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1);
//    int n = radiuses.size();
//    for (int i = 0; i < n; ++i) {
//        for (int j = 0; j < n; ++j) {
//            cv::Mat new_image(sz, sz, CV_8UC1, 70);
//            cv::circle(new_image, cv::Point(sz / 2, sz / 2), radiuses[j], 110, -1);
//            if (i > 0) {
//                cv::GaussianBlur(new_image, new_image, cv::Size(smoothing[i], smoothing[i]), 0);
//            }
//            add_noise(new_image, 6);
//
//            cv::Mat binary_new_image(sz, sz, CV_8UC1);
//            BradleyThreshHold(new_image, 15.0f, 0.15f); // Используем Брэдли-Рота для бинаризации
//            new_image.copyTo(binary_new_image);
//
//            binary_new_image.copyTo(binary_result(cv::Rect(sz * j, sz * i, sz, sz)));
//            new_image.copyTo(result(cv::Rect(sz * j, sz * i, sz, sz)));
//        }
//    }
//    return { result, binary_result };
//}
//
//int main() {
//    std::vector<int> radiuses = { 10, 20, 30, 40, 50 }; // Пример радиусов для кругов
//    std::vector<int> smoothing = { 3, 5, 7, 9, 11 }; // Пример параметров размытия
//
//    // Генерируем изображения
//    std::pair<cv::Mat, cv::Mat> images = generated_images(radiuses, smoothing);
//
//    double sigma1 = 1.0;
//    double sigma2 = 2.5;
//    cv::Mat LoG_result = LoG_DoG_Detection(images.second, sigma1, sigma2);
//
//    // Отображаем сгенерированные изображения
//    cv::imshow("Result Image", images.first); // Отображаем изображение с кругами
//    cv::imshow("Binary Image", images.second); // Отображаем бинарное изображение
//    cv::imshow("Detected Image", LoG_result);
//    cv::waitKey(0);
//
//    return 0;
//}


//Вариант бинаризации алгоритмом Ниблека
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <opencv2/highgui.hpp>
//#include <random>
//
//// Функция для бинаризации изображения методом Ниблека
//void NiblackThreshHold(cv::Mat& image, float aperturesize, float k) {
//    int rows = image.rows;
//    int cols = image.cols;
//
//    cv::Mat binarizedimage = cv::Mat::zeros(rows, cols, CV_8UC1);
//
//    for (int x = 0; x < rows; x++) {
//        for (int y = 0; y < cols; y++) {
//            float mean, stddev;
//            float sum = 0;
//            int count = 0;
//            // mean и stddev
//            for (int i = -aperturesize / 2; i <= aperturesize / 2; i++) {
//                for (int j = -aperturesize / 2; j <= aperturesize / 2; j++) {
//                    int newX = x + i;
//                    int newY = y + j;
//
//                    if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) {
//                        sum += image.at<uchar>(newX, newY);
//                        count += 1;
//                    }
//                }
//            }
//
//            mean = sum / count;
//
//            float sum_stddev = 0;
//            for (int i = -aperturesize / 2; i <= aperturesize / 2; i++) {
//                for (int j = -aperturesize / 2; j <= aperturesize / 2; j++) {
//                    int newX = x + i;
//                    int newY = y + j;
//
//                    if (newX >= 0 && newX < rows && newY >= 0 && newY < cols) {
//                        sum_stddev += (image.at<uchar>(newX, newY) - mean) * (image.at<uchar>(newX, newY) - mean);
//                    }
//                }
//            }
//
//            stddev = sqrt(sum_stddev / count);
//
//            int threshold = static_cast<int>(mean + k * stddev);
//
//            if (image.at<uchar>(x, y) >= threshold) {
//                binarizedimage.at<uchar>(x, y) = 255;
//            }
//            else {
//                binarizedimage.at<uchar>(x, y) = 0;
//            }
//        }
//    }
//
//    image = binarizedimage;
//}
//
//// Функция для добавления шума к изображению
//void add_noise(cv::Mat& image, double noise_level) {
//    std::default_random_engine generator;
//    std::normal_distribution<double> distribution(0.0, noise_level);
//
//    for (int i = 0; i < image.rows; i++) {
//        for (int j = 0; j < image.cols; j++) {
//            uchar& pixel_value = image.at<uchar>(i, j);
//            pixel_value += distribution(generator);
//            if (pixel_value < 0) {
//                pixel_value = 0;
//            }
//            else if (pixel_value > 255) {
//                pixel_value = 255;
//            }
//        }
//    }
//}
//
//// Функция для обнаружения краев методом LoG (Laplacian of Gaussian)
//cv::Mat LoG_DoG_Detection(const cv::Mat& image, double sigma1, double sigma2) {
//    // Применяем оператор LoG
//    cv::Mat blurred1, log_image;
//    cv::GaussianBlur(image, blurred1, cv::Size(0, 0), sigma1);
//    cv::Laplacian(blurred1, log_image, CV_64F);
//
//    return log_image;
//}
//
//// Функция для генерации изображений
//std::pair<cv::Mat, cv::Mat> generated_images(std::vector<int> radiuses, std::vector<int> smoothing) {
//    size_t sz = *std::max_element(radiuses.begin(), radiuses.end()) * 2 + 30;
//    cv::Mat result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1, 70);
//    cv::Mat binary_result(sz * radiuses.size(), sz * radiuses.size(), CV_8UC1);
//    int n = radiuses.size();
//    for (int i = 0; i < n; ++i) {
//        for (int j = 0; j < n; ++j) {
//            cv::Mat new_image(sz, sz, CV_8UC1, 70);
//            cv::circle(new_image, cv::Point(sz / 2, sz / 2), radiuses[j], 110, -1);
//            if (i > 0) {
//                cv::GaussianBlur(new_image, new_image, cv::Size(smoothing[i], smoothing[i]), 0);
//            }
//            add_noise(new_image, 6);
//
//            cv::Mat binary_new_image(sz, sz, CV_8UC1);
//            NiblackThreshHold(new_image, 15, 0.5);  // Используем Ниблека для бинаризации
//            new_image.copyTo(binary_new_image);
//
//            binary_new_image.copyTo(binary_result(cv::Rect(sz * j, sz * i, sz, sz)));
//            new_image.copyTo(result(cv::Rect(sz * j, sz * i, sz, sz)));
//        }
//    }
//    return { result, binary_result };
//}
//
//int main() {
//    std::vector<int> radiuses = { 10, 20, 30, 40, 50 }; // Пример радиусов для кругов
//    std::vector<int> smoothing = { 3, 5, 7, 9, 11 }; // Пример параметров размытия
//
//    // Генерируем изображения
//    std::pair<cv::Mat, cv::Mat> images = generated_images(radiuses, smoothing);
//
//    double sigma1 = 1.0;
//    double sigma2 = 2.5;
//    cv::Mat LoG_result = LoG_DoG_Detection(images.second, sigma1, sigma2);
//
//    // Отображаем сгенерированные изображения
//    cv::imshow("Result Image", images.first); // Отображаем изображение с кругами
//    cv::imshow("Binary Image", images.second); // Отображаем бинарное изображение
//    cv::imshow("Detected Image", LoG_result);
//    cv::waitKey(0);
//
//    return 0;
//}
