#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

vector<Point2f> searchWindow(Mat frame)
{
    Rect rect(0, 0, 10, 10);
    vector<Point2f> points;
    int x_point, y_point;
    Mat region;
    vector<Point2f> regionPoints;

    for(rect.y = 0; rect.y < frame.rows; rect.y += rect.height)
    {
        for(rect.x = 0; rect.x < frame.cols; rect.x += rect.width)
        {
            region = frame(rect);
            findNonZero(region, regionPoints);
            x_point = 0; y_point = 0;

            int sizeRegPoints = regionPoints.size();

            if(!sizeRegPoints)
                continue;
            
            for(int i = 0; i < sizeRegPoints; ++i)
            {
                x_point += rect.x + regionPoints[i].x;
                y_point += rect.y + regionPoints[i].y;
            }
            x_point /= sizeRegPoints;
            y_point /= sizeRegPoints;
            
            points.push_back(Point(x_point, y_point));
        }
    }

    return points;
}

int main(int argc, char *argv[])
{
    Mat frame, frameSlice, frameSliceProc, M, invM;
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

    vector<Point2f> invPoints;
    vector<Point2f> points;

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
        points.clear();
        
        video.read(frame);

        if(frame.empty())
            break;

        resize(frame, frame, Size(widthIn, heightIn));

        inputPoints[0] = Point2f((widthIn - valueBaseline) / 2, heightIn - 20);
        inputPoints[1] = Point2f((widthIn - valueTopline) / 2, valueHeight);
        inputPoints[2] = Point2f((widthIn + valueTopline) / 2, valueHeight);
        inputPoints[3] = Point2f((widthIn + valueBaseline) / 2, heightIn - 20);

        circle(frame, inputPoints[0], 1, Scalar(0, 0, 255), FILLED);
        circle(frame, inputPoints[1], 1, Scalar(0, 0, 255), FILLED);
        circle(frame, inputPoints[2], 1, Scalar(0, 0, 255), FILLED);
        circle(frame, inputPoints[3], 1, Scalar(0, 0, 255), FILLED);

        line(frame, inputPoints[0], inputPoints[1], Scalar(0, 0, 255), 3);
        line(frame, inputPoints[1], inputPoints[2], Scalar(0, 0, 255), 3);
        line(frame, inputPoints[2], inputPoints[3], Scalar(0, 0, 255), 3);
        line(frame, inputPoints[3], inputPoints[0], Scalar(0, 0, 255), 3);

        M = getPerspectiveTransform(inputPoints, outputPoints);
        warpPerspective(frame, frameSlice, M, Size(widthOut, heightOut));

        cvtColor(frameSlice, frameSliceProc, COLOR_BGR2GRAY);
        blur(frameSliceProc, frameSliceProc, Size(9, 9));
        threshold(frameSliceProc, frameSliceProc, 150, 255, THRESH_BINARY);

        invPoints = searchWindow(frameSliceProc);

        invert(M, invM);
        if(invPoints.size())
            perspectiveTransform(invPoints, points, invM);

        for(int i = 0; i < points.size(); ++i)
            circle(frame, points[i], 3, Scalar(255, 0, 0), FILLED);

        sprintf(fpsStr, "FPS:%3.0f", video.get(CAP_PROP_FPS));
        putText(frame, fpsStr, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        sprintf(timeStr, "TIME(msec): %6.1f", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
        putText(frame, timeStr, Point(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        imshow("video", frame);
        imshow("ipm", frameSliceProc);

        if(waitKey(1) >= 0) break;
    }

    video.release();
    return 0;
}
