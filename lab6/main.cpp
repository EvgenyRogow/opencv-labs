#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

vector<vector<Point2f>> searchWindow(Mat frame)
{
    Rect rect(0, 0, 10, 10);
    vector<vector<Point2f>> allPoints;
    vector<Point2f> leftPoints;
    vector<Point2f> rightPoints;

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

            if(x_point <= frame.cols / 2)
                leftPoints.push_back(Point2f(x_point, y_point));
            else
                rightPoints.push_back(Point2f(x_point, y_point));
        }
    }

    allPoints.push_back(leftPoints);
    allPoints.push_back(rightPoints);

    return allPoints;
}

vector<double> approximation(vector<Point2f> points, int degree)
{
    int N = points.size();
    int n = degree;
    double x[N], y[N];
    double X[2 * n + 1], Y[n + 1];
    double B[n + 1][n + 2];
    vector<double> a(n + 1);

    /* x = f(y) */
    for(int i = 0; i < N; ++i)
    {
        y[i] = points[i].x;
        x[i] = points[i].y;
    }

    for(int i = 0; i < 2 * n + 1; ++i)
    {
        X[i] = 0;
        for(int j = 0; j < N; ++j)
            X[i] += pow(x[j], i);
    }

    for(int i = 0; i <= n; ++i)
        for(int j = 0; j <= n; ++j)
            B[i][j] = X[i + j];

    for(int i = 0; i < n + 1; ++i)
    {
        Y[i] = 0;
        for(int j = 0; j < N; ++j)
            Y[i] += pow(x[j], i) * y[j];
    }

    for(int i = 0; i <= n; ++i)
        B[i][n + 1] = Y[i];
    
    ++n;

    for(int i = 0; i < n; ++i)
    {
        for(int k = i + 1; k < n; ++k)
        {
            if(B[i][i] < B[k][i])
            {
                for(int j = 0; j <= n; ++j)
                {
                    double temp = B[i][j];
                    B[i][j] = B[k][j];
                    B[k][j] = temp;
                }
            }
        }
    }

    for(int i = 0; i < n - 1; ++i)
    {
        for(int k = i + 1; k < n; ++k)
        {
            double t = B[k][i] / B[i][i];
            for(int j = 0; j <= n; ++j)
                B[k][j] -= t * B[i][j];
        }
    }

    for(int i = n - 1; i >= 0; --i)
    {
        a[i] = B[i][n];
        for(int j = 0; j < n; ++j)
            if(j != i)
                a[i] -= B[i][j] * a[j];
        a[i] /= B[i][i];
    }

    return a;
}

Point2f getMaxPoint(vector<Point2f> points)
{
    if(!points.size()) return Point2f(0, 0);
    Point2f max = points[0];

    for(int i = 1; i < points.size(); ++i)
        if(points[i].y > max.y)
            max = points[i];

    return max;
}

Point2f getMinPoint(vector<Point2f> points)
{
    if(!points.size()) return Point2f(0, 0);
    Point2f min = points[0];

    for(int i = 1; i < points.size(); ++i)
        if(points[i].y < min.y)
            min = points[i];

    return min;
}

int main(int argc, char *argv[])
{
    Mat frame, frameLines, frameSlice, frameSliceProc, M, invM;
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

    vector<vector<Point2f>> invAllPoints;
    vector<Point2f> leftPoints;
    vector<Point2f> rightPoints;
    vector<Point2f> invLeftLines;
    vector<Point2f> invRightLines;
    vector<Point2f> leftLines;
    vector<Point2f> rightLines;   
    vector<double> left_coeffs;
    vector<double> right_coeffs;
    Point2f point;
    Point2f maxPointLeft;
    Point2f minPointLeft;
    Point2f maxPointRight;
    Point2f minPointRight;

    int agree = 2;

    namedWindow("frame");

    int valueTopline = 220;
    int valueBaseline = 540; 
    int valueHeight = 330;

    createTrackbar("Topline", "frame", &valueTopline, widthIn);
    createTrackbar("Baseline", "frame", &valueBaseline, widthIn);
    createTrackbar("Height", "frame", &valueHeight, heightIn - 20);

    clock_t start = clock();

    for(;;)
    {
        leftPoints.clear();
        rightPoints.clear();
        leftLines.clear();
        rightLines.clear();
        invLeftLines.clear();
        invRightLines.clear();

        video.read(frame);

        if(frame.empty())
            break;

        resize(frame, frame, Size(widthIn, heightIn));
        frameLines = frame.clone();

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

        invAllPoints = searchWindow(frameSliceProc);

        invert(M, invM);

        if(invAllPoints[0].size())
            perspectiveTransform(invAllPoints[0], leftPoints, invM);
        if(invAllPoints[1].size())
            perspectiveTransform(invAllPoints[1], rightPoints, invM);

        for(int i = 0; i < leftPoints.size(); ++i)
            circle(frame, leftPoints[i], 3, Scalar(255, 0, 0), FILLED);

        for(int i = 0; i < rightPoints.size(); ++i)
            circle(frame, rightPoints[i], 3, Scalar(255, 0, 0), FILLED);

        left_coeffs = approximation(invAllPoints[0], agree);
        right_coeffs = approximation(invAllPoints[1], agree);

        maxPointLeft = getMaxPoint(invAllPoints[0]);
        minPointLeft = getMinPoint(invAllPoints[0]);

        for(point.y = minPointLeft.y; point.y < maxPointLeft.y; point.y += 0.1)
        {
            point.x = 0;
            for(int k = 0; k < left_coeffs.size(); ++k)
                point.x += left_coeffs[k] * pow(point.y, k);
            invLeftLines.push_back(point);
        }

        maxPointRight = getMaxPoint(invAllPoints[1]);
        minPointRight = getMinPoint(invAllPoints[1]);

        for(point.y = minPointRight.y; point.y < maxPointRight.y; point.y += 0.1)
        {
            point.x = 0;
            for(int k = 0; k < right_coeffs.size(); ++k)
                point.x += right_coeffs[k] * pow(point.y, k);
            invRightLines.push_back(point);
        }

        if(invLeftLines.size())
            perspectiveTransform(invLeftLines, leftLines, invM);
        if(invRightLines.size())
            perspectiveTransform(invRightLines, rightLines, invM);

        for(int i = 0; i < leftLines.size(); ++i)
            circle(frameLines, leftLines[i], 3, Scalar(255, 0, 0), FILLED);

        for(int i = 0; i < rightLines.size(); ++i)
            circle(frameLines, rightLines[i], 3, Scalar(255, 0, 0), FILLED);

        /* area */
        vector<vector<Point>> polygon;
        vector<Point> pts;
        pts.push_back(getMinPoint(leftLines));
        pts.push_back(getMinPoint(rightLines));
        pts.push_back(getMaxPoint(rightLines));
        pts.push_back(getMaxPoint(leftLines));
        polygon.push_back(pts);

        Mat temp = frameLines.clone();
        fillPoly(temp, polygon, Scalar(0, 0, 255));
        addWeighted(frameLines, 1, temp, 0.5, 0, frameLines);

        sprintf(fpsStr, "FPS:%3.0f", video.get(CAP_PROP_FPS));
        putText(frame, fpsStr, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        sprintf(timeStr, "TIME(msec): %6.1f", (double)(clock() - start) / CLOCKS_PER_SEC * 1000);
        putText(frame, timeStr, Point(20, 60), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        imshow("frame", frame);
        imshow("frameLines", frameLines);
        imshow("ipm", frameSliceProc);

        if(waitKey(1) >= 0) break;
    }

    video.release();
    return 0;
}
