#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat frame, frameGray, frameBlur, frameRed, frameEdges;
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

        blur(frame, frameBlur, Size(11, 11));
        inRange(frame, Scalar(0, 0, 150), Scalar(100, 100, 255) , frameRed);

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
