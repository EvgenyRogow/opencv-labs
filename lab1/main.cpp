#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2) 
    {
        cerr << "Error: image not found" << endl;
        return 1;
    }

    string imagePath = argv[1];
    Mat imageOrig, imageHSV, imageGray, imageBin;

    imageOrig = imread(imagePath);

    if(imageOrig.empty())
    { 
        cerr << "Error: could not read the image: " << imagePath << endl;
        return 1;
    }

    cvtColor(imageOrig, imageHSV, COLOR_BGR2HSV);
    cvtColor(imageOrig, imageGray, COLOR_BGR2GRAY);
    threshold(imageGray, imageBin, 127, 255, THRESH_BINARY);

    imshow("Original image", imageOrig);
    imshow("HSV image", imageHSV);
    imshow("Gray image", imageGray);
    imshow("Binary image", imageBin);

    circle(imageOrig, Point(350, 170), 140, Scalar(255, 255, 255), 2);
    circle(imageOrig, Point(200, 310), 85, Scalar(255, 255, 255), 2);

    imshow("Image with circle", imageOrig);

    waitKey(0);

    return 0;
}
