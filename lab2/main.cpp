#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat customBlur(const Mat &image)
{
    Mat imageBlur;

    int ksize = 3;

    Mat kernel(Size(ksize, ksize), CV_64F);
    kernel.setTo(1. / (ksize * ksize));

    filter2D(image, imageBlur, -1, kernel);

    return imageBlur;
}

Mat gradientEast(const Mat &image)
{
    Mat imageGradEast = image.clone();

    int kernel[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int gx, gy;
    gy = 0;

    for(int i = 1; i < image.rows - 1; ++i)
    {
        for(int j = 1; j < image.cols - 1; ++j)
        {
            gx = 0;
            for(int k_i = 0; k_i < 3; ++k_i)
                for(int k_j = 0; k_j < 3; ++k_j)
                    gx += kernel[k_i][k_j] * image.at<uchar>(i - 1 + k_i, j - 1 + k_j);
            gx = (gx > 255) ? 255 : gx;
            gx = (gx < 0) ? 0 : gx;
            imageGradEast.at<uchar>(i, j) = gx;
        }
    }
    
    return imageGradEast;
}

int main(int argc, char *argv[])
{
    if(argc != 2) 
    {
        cerr << "Error: image not found" << endl;
        return 1;
    }

    Mat imageOrig, imageGaussianBlur, imageGray;

    imageOrig = imread(argv[1]);

    if(imageOrig.empty())
    { 
        cerr << "Error: could not read the image" << endl;
        return 1;
    }

    GaussianBlur(imageOrig, imageGaussianBlur, Size(9, 9), 0);

    imshow("Original image", imageOrig);
    imshow("GaussianBlur image", imageGaussianBlur);
    imshow("CustomBlur image", customBlur(imageOrig));

    cvtColor(imageOrig, imageGray, COLOR_BGR2GRAY);
    imshow("Gradient East", gradientEast(imageGray));

    waitKey(0);

    return 0;
}
