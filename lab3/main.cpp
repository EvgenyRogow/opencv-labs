#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat frame, frameGray, frameHSV, frameBlur, frameRed, frameEdges, redMask1, redMask2;
    VideoCapture cap;

    cap.open(0, CAP_ANY);

    if (!cap.isOpened())
    {
        cerr << "Error: unable to open camera" << endl;
        return 1;
    }

    int threshold = 45;

    for(;;)
    {
        cap.read(frame);

        if(frame.empty())
        {
            cerr << "Error: blank frame grabbed" << endl;
            break;
        }

        cvtColor(frame, frameGray, COLOR_BGR2GRAY);
        cvtColor(frame, frameHSV, COLOR_BGR2HSV);

        blur(frame, frameBlur, Size(11, 11));

        Mat mask1, mask2;
        inRange(frameHSV, Scalar(0, 100, 100), Scalar(10, 255, 255) , redMask1);
        inRange(frameHSV, Scalar(160, 100, 100), Scalar(180, 255, 255) , redMask2);
        frameRed = redMask1 | redMask2;

        blur(frameGray, frameEdges, Size(3, 3));
        Canny(frameEdges, frameEdges, threshold, 2 * threshold);

        imshow("webcam", frame);
        imshow("webcam blur", frameBlur);
        imshow("webcam red", frameRed);
        imshow("webcam edges", frameEdges);

        if(waitKey(1) >= 0) break;
    }
    
    return 0;
}
