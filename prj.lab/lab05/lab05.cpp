#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

cv::Mat generateCircleVariants() {
    // Создание изображения с белым фоном
    Mat square(99, 99, CV_8UC1, Scalar(0));

    // Рисование серого круга в центре
    circle(square, Point(49, 49), 25, Scalar(127), -1);

    // Разделение на прямоугольники
    Mat result(198, 297, CV_8UC1, Scalar(127));

    //первый квалрат с кругом
    Mat roi = result(Rect(0, 0, 99, 99));
    square.copyTo(roi);

    //второй квалрат с кругом
    roi = result(Rect(99, 0, 99, 99));
    square.setTo(127); // Очистка квадрата для серого фона
    circle(square, Point(49, 49), 25, Scalar(0), -1);
    square.copyTo(roi);

    //третий квалрат с кругом
    roi = result(Rect(198, 0, 99, 99));
    square.setTo(255);
    circle(square, Point(49, 49), 25, Scalar(0), -1);
    square.copyTo(roi);

    //4 квалрат с кругом
    roi = result(Rect(0, 99, 99, 99));
    square.setTo(255);
    circle(square, Point(49, 49), 25, Scalar(127), -1);
    square.copyTo(roi);

    //5 квалрат с кругом
    roi = result(Rect(99, 99, 99, 99));
    square.setTo(0);
    circle(square, Point(49, 49), 25, Scalar(255), -1);
    square.copyTo(roi);

    //6 квалрат с кругом
    roi = result(Rect(198, 99, 99, 99));
    square.setTo(127);
    circle(square, Point(49, 49), 25, Scalar(255), -1);
    square.copyTo(roi);

    return result;
}

int main() {

    Mat result;
    result = generateCircleVariants();
    imshow("Circle Variants", result);
    waitKey(0);
    //создание ядер
    cv::Mat kernel_1 = (Mat_<float>(2, 2) << 1, 0, 0, 1);
    cv::Mat kernel_2 = (Mat_<float>(2, 2) << 0, -1, -1, 0);

    //Изображения для хранения результата линейной фильтрации
    Mat filtered_1, filtered_2;

    filter2D(result, filtered_1, CV_32F, kernel_1);
    filter2D(result, filtered_2, CV_32F, kernel_2);

   /* for (int i = 0; i < filtered_1.rows; i++) {
        for (int j = 0; j < filtered_1.cols; j++) {
            filtered_1.at<float>(i, j) = saturate_cast<float>(127.5 + 0.5 * filtered_1.at<float>(i, j));

        }
    }*/
    for (int i = 0; i < filtered_2.rows; i++) {
        for (int j = 0; j < filtered_2.cols; j++) {
            filtered_2.at<float>(i, j) = saturate_cast<float>(255 + 0.5 * filtered_2.at<float>(i, j));

        }
    }

    imshow("Filtered Image 1", filtered_1);
    imshow("Filtered Image 2", filtered_2);
    waitKey(0);

    return 0;
}
