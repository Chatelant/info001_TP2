#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

// ----- Filtre moyenneur ----- //
cv::Mat filtreM(cv::Mat input)
{
    Mat output;
    Mat kernel = (Mat_<float>(3, 3) << 1.0 / 16, 2.0 / 16, 1.0 / 16.0,
                  2.0 / 16, 4.0 / 16, 2.0 / 16.0,
                  1.0 / 16, 2.0 / 16, 1.0 / 16.0);
    filter2D(input, output, -1, kernel);
    return output;
}

// ----- Filtre median ----- //
cv::Mat myMedianBlur(cv::Mat input)
{
    Mat output;
    medianBlur(input, output, 3);
    return output;
}

// ----- Rehaussement de contraste ----- //
cv::Mat rehaussementC(cv::Mat input, float alpha)
{
    Mat output;
    Mat mat = (Mat_<float>(3, 3) << 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
    Mat matInter = (Mat_<float>(3, 3) << 0.0, 1.0, 0.0, 1.0, -4.0, 1.0, 0.0, 1.0, 0.0);
    Mat kernel = mat - alpha * matInter;
    filter2D(input, output, -1, kernel);
    return output;
}

// ----- Filtre derivatif Sobel X ----- //
cv::Mat filtreSobelX(cv::Mat input)
{
    Mat output;
    Mat kernel = (Mat_<float>(3, 3) << -1.0 / 4.0, 0.0, 1.0 / 4.0, -2.0 / 4, 0.0, 2.0 / 4.0, -1.0 / 4.0, 0.0, 1.0 / 4.0);
    filter2D(input, output, -1, kernel, Point(0, 0), 128);
    return output;
}

// ----- Filtre derivatif Sobel Y ----- //
cv::Mat filtreSobelY(cv::Mat input)
{
    Mat output;
    Mat kernel = (Mat_<float>(3, 3) << -1.0 / 4.0, -2.0 / 4, -1.0 / 4.0, 0.0, 0.0, 0.0, 1.0 / 4.0, 2.0 / 4.0, 1.0 / 4.0);
    filter2D(input, output, -1, kernel, Point(0, 0), 128);
    return output;
}

// ----- Gradient ----- //
cv::Mat gradient(cv::Mat input)
{
    
}

int main(int argc, char *argv[])
{
    std::string path = "/home/user/TP2/";

    if (argv[1] == nullptr)
    {
        std::cout << "Usage : ./main <image>" << std::endl;
        exit(1);
    }

    Mat input = imread(path + argv[1]);

    if (input.channels() == 3)
        cv::cvtColor(input, input, COLOR_BGR2GRAY);

    namedWindow("Image");
    imshow("Image", input);

    int alpha = 20;
    createTrackbar("alpha (en %)", "Image", nullptr, 200, NULL);
    setTrackbarPos("alpha (en %)", "Image", alpha); // init à 20

    while (true)
    {
        int keycode = waitKey(50);
        int asciicode = keycode & 0xff;
        if (asciicode == 'q')
            break;
        if (asciicode == 'a')
            input = filtreM(input);
        if (asciicode == 'm')
            input = myMedianBlur(input);
        if (asciicode == 's')
            input = rehaussementC(input, getTrackbarPos("alpha (en %)", "Image"));
        if (asciicode == 'x')
            input = filtreSobelX(input);
        if (asciicode == 'y')
            input = filtreSobelY(input);
        imshow("Image", input);
    }
}