#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat matIn, matTrap, matOutTrap, matM;
    VideoCapture video("solidYellowLeft.mp4");

    if (!video.isOpened())
    {
        cerr << "Error: unable to open video" << endl;
        return 1;
    }

    int widthIn = 640;
    int heightIn = 480;
    int widthOut = 360;
    int heightOut = 280;
    char fpsStr[20];
    char timeStr[20];

    Point2f inputPoints[4];
    Point2f outputPoints[4];
    outputPoints[0] = Point2f(0, heightOut);
    outputPoints[1] = Point2f(0, 0);
    outputPoints[2] = Point2f(widthOut, 0);
    outputPoints[3] = Point2f(widthOut, heightOut);

    namedWindow("video");

    int valueTopline = 220;
    int valueBaseline = 540;
    int valueHeight = 330;

    createTrackbar("Topline", "video", &valueTopline, widthIn);
    createTrackbar("Baseline", "video", &valueBaseline, widthIn);
    createTrackbar("Height", "video", &valueHeight, heightIn - 20);

    clock_t start = clock();

    for(;;)
    {
        video.read(matIn);

        if(matIn.empty())
            break;

        resize(matIn, matIn, Size(widthIn, heightIn));
        matTrap = matIn.clone();

<<<<<<< HEAD
        inputPoints[0] = Point2f((widthIn - valueBaseline) / 2, heightIn - 20);
        inputPoints[1] = Point2f((widthIn - valueTopline) / 2, valueHeight);
        inputPoints[2] = Point2f((widthIn + valueTopline) / 2, valueHeight);
        inputPoints[3] = Point2f((widthIn + valueBaseline) / 2, heightIn - 20);
=======
        inputPoints[0] = Point2f(valueBaseline, 460);
        inputPoints[1] = Point2f(valueTopline, valueHeight);
        inputPoints[2] = Point2f(widthIn - valueTopline, valueHeight);
        inputPoints[3] = Point2f(widthIn - valueBaseline, 460);
>>>>>>> fc271f77d3515b85d7170b133c73b2c70d50836b

        circle(matTrap, inputPoints[0], 1, Scalar(0, 0, 255), FILLED);
        circle(matTrap, inputPoints[1], 1, Scalar(0, 0, 255), FILLED);
        circle(matTrap, inputPoints[2], 1, Scalar(0, 0, 255), FILLED);
        circle(matTrap, inputPoints[3], 1, Scalar(0, 0, 255), FILLED);

        line(matTrap, inputPoints[0], inputPoints[1], Scalar(0, 0, 255), 3);
        line(matTrap, inputPoints[1], inputPoints[2], Scalar(0, 0, 255), 3);
        line(matTrap, inputPoints[2], inputPoints[3], Scalar(0, 0, 255), 3);
        line(matTrap, inputPoints[3], inputPoints[0], Scalar(0, 0, 255), 3);

        sprintf(fpsStr, "FPS:%3.0f", video.get(CAP_PROP_FPS));
        putText(matTrap, fpsStr, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        sprintf(timeStr, "TIME(msec): %6.1f", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
        putText(matTrap, timeStr, Point(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        matM = getPerspectiveTransform(inputPoints, outputPoints);
        warpPerspective(matIn, matOutTrap, matM, Size(widthOut, heightOut));

        imshow("video", matTrap);
        imshow("output trap", matOutTrap);

        if(waitKey(1) >= 0) break;
    }

    video.release();
    return 0;
}
