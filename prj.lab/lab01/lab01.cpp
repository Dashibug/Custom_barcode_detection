#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

void gamma_correction(cv::Mat& image, cv::Mat& new_image, double gamma) {
    //так как pow ожидает 32или64 бит, преобразуем картинку
    cv::Mat float_image;
    image.convertTo(float_image, CV_32F, 1.0 / 255.0);

    cv::Mat gammaCorrected;
    cv::pow(float_image, gamma, gammaCorrected);//деление для нормализации в диапозоне от 0 - 1
    gammaCorrected *= 255.0;
    gammaCorrected.convertTo(new_image, CV_8U);
}

cv::Mat generated_gradient_image(int width, int height) {
    cv::Mat new_gradient(height, width, CV_8U);
    for (int i = 0; i < height; i++) {
        uchar* p = new_gradient.ptr(i);//uchar* - указатель на безнаковый 8бит символ; каждый элемент - интенсивность пикселя
        for (int j = 0; j < width; j++) {//ptr - возвращает указатель, доступ к пикселю
            p[j] = static_cast<uchar>(j * 255 / width);//j * 255/ width - формула создает градиент
        }
    }
    return new_gradient;
}

int main() {
   
    int s = 1200; 
    int h = 300; 
    double gamma = 2.4;

    cv::Mat gradientImage = generated_gradient_image(s, h);

    cv::Mat gamma_corrected_gradient_image(s, h, CV_8U);
    gamma_correction(gradientImage, gamma_corrected_gradient_image, gamma);
    
    cv::Mat combinedImage(2 * h, s, CV_8U);
    cv::Mat first_line = combinedImage(cv::Rect(0, 0, s, h));
    cv::Mat second_line = combinedImage(cv::Rect(0, h, s, h));

    gradientImage.copyTo(first_line);
    gamma_corrected_gradient_image.copyTo(second_line);

    cv::imshow("gradient_and_gamma", combinedImage);
    cv::waitKey(0);

    return 0;
}
