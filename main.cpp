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
cv::Mat filtreSobelX(cv::Mat input, int delta = 128)
{
    Mat output;
    Mat kernel = (Mat_<float>(3, 3) << -1.0 / 4.0, 0.0, 1.0 / 4.0, -2.0 / 4, 0.0, 2.0 / 4.0, -1.0 / 4.0, 0.0, 1.0 / 4.0);
    filter2D(input, output, -1, kernel, Point(0, 0), delta);
    return output;
}

// ----- Filtre derivatif Sobel Y ----- //
cv::Mat filtreSobelY(cv::Mat input, int delta = 128)
{
    Mat output;
    Mat kernel = (Mat_<float>(3, 3) << -1.0 / 4.0, -2.0 / 4, -1.0 / 4.0, 0.0, 0.0, 0.0, 1.0 / 4.0, 2.0 / 4.0, 1.0 / 4.0);
    filter2D(input, output, -1, kernel, Point(0, 0), delta);
    return output;
}

// ----- Gradient ----- //
cv::Mat myGradient(cv::Mat input)
{
    input.convertTo(input, CV_32F);
    Mat sobX = filtreSobelX(input, 0);
    Mat sobY = filtreSobelY(input, 0);
    Mat output = input.clone();

    for (int y = 0; y < input.cols; y++)
    {
        for (int x = 1; x < input.rows; x++)
        {
            output.at<float>(x, y) = sqrt(sobX.at<float>(x, y) * sobX.at<float>(x, y) + sobY.at<float>(x, y) * sobY.at<float>(x, y));
        }
    }
    input.convertTo(input, CV_8U);
    output.convertTo(output, CV_8U);
    return output;
}

// ----- Détection de contours à la Marr-Hildreth ----- //
cv::Mat contourHildreth(cv::Mat input, int seuil, int alpha)
{
    input.convertTo(input, CV_32F);
    Mat lapla = input.clone();
    Mat gradient = myGradient(input);
    gradient.convertTo(gradient, CV_32F);
    Mat output = input.clone();
    lapla = rehaussementC(lapla, alpha);

    // Parcours tableau
    for (int y = 0; y < input.cols; y++)
    {
        for (int x = 1; x < input.rows; x++)
        {
            bool changedNeighbour = false;

            // Si les pixels aux alentours changent de signe
            for (int yi = y - 1; yi <= y + 1; yi++)
            {
                for (int xi = x - 1; xi <= x + 1; xi++)
                {
                    if (input.at<float>(xi, yi) >= 0 && lapla.at<float>(xi, yi) < 0 || input.at<float>(xi, yi) < 0 && lapla.at<float>(xi, yi) >= 0)
                        changedNeighbour = true;
                }
            }
            if (gradient.at<float>(x, y) >= (float)seuil && changedNeighbour)
                output.at<float>(x, y) = 0.0;
            else
                output.at<float>(x, y) = 255.0;
        }
    }
    output.convertTo(output, CV_8U);
    return output;
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
    int seuil = 20;
    createTrackbar("alpha (en %)", "Image", nullptr, 200, NULL);
    setTrackbarPos("alpha (en %)", "Image", alpha);
    createTrackbar("seuil (en %)", "Image", nullptr, 200, NULL);
    setTrackbarPos("seuil (en %)", "Image", seuil);
    Mat out = input.clone();
    while (true)
    {
        int keycode = waitKey(50);
        int asciicode = keycode & 0xff;
        if (asciicode == 'q')
            break;
        if (asciicode == 'a')
            out = filtreM(input);
        if (asciicode == 'm')
            out = myMedianBlur(input);
        if (asciicode == 's')
            out = rehaussementC(input, getTrackbarPos("alpha (en %)", "Image"));
        if (asciicode == 'x')
            out = filtreSobelX(input);
        if (asciicode == 'y')
            out = filtreSobelY(input);
        if (asciicode == 'g')
            out = myGradient(input);
        if (asciicode == 'c')
            out = contourHildreth(input, getTrackbarPos("seuil (en %)", "Image"), getTrackbarPos("alpha (en %)", "Image"));
        imshow("Image", out);
    }
}