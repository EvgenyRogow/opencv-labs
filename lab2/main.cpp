#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

uchar Pixel1b(const Mat& image, const vector<vector<double>> &kernel, int i, int j)
{
    int pixel = 0;

    for(int k_i = 0; k_i < kernel.size(); ++k_i)
        for(int k_j = 0; k_j < kernel.size(); ++k_j)
            pixel += kernel[k_i][k_j] * image.at<uchar>(i - 1 + k_i, j - 1 + k_j);

    pixel = (pixel > 255) ? 255 : pixel;
    pixel = (pixel < 0) ? 0 : pixel;

    return pixel;
}

Vec3b Pixel3b(const Mat& image, const vector<vector<double>> &kernel, int i, int j)
{
    Vec3b pixel;

    for(int p = 0; p < 3; ++p)
    {
        pixel[p] = 0;
        for(int k_i = 0; k_i < kernel.size(); ++k_i)
            for(int k_j = 0; k_j < kernel.size(); ++k_j)
                pixel[p] += kernel[k_i][k_j] * image.at<Vec3b>(i - 1 + k_i, j - 1 + k_j)[p];

        pixel[p] = (pixel[p] > 255) ? 255 : pixel[p];
        pixel[p] = (pixel[p] < 0) ? 0 : pixel[p];
    }

    return pixel;
}

Mat customBlur(const Mat &image)
{
    Mat imageBlur = image.clone();

    int ksize = 3;

    vector<vector<double>> kernel(ksize, vector<double>(ksize, 1. / (ksize * ksize)));
    
    if(!imageBlur.type())
    {
        for(int i = 1; i < image.rows - 1; ++i)
            for(int j = 1; j < image.cols - 1; ++j)
                imageBlur.at<uchar>(i, j) = Pixel1b(image, kernel, i, j);
    } 
    else 
    {
        for(int i = 1; i < image.rows - 1; ++i)
            for(int j = 1; j < image.cols - 1; ++j)
                imageBlur.at<Vec3b>(i, j) = Pixel3b(image, kernel, i, j);
    }

    return imageBlur;
}

Mat gradientEast(const Mat &image)
{
    Mat imageGradEast = image.clone();

    vector<vector<double>> kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    for(int i = 1; i < image.rows - 1; ++i)
        for(int j = 1; j < image.cols - 1; ++j)
            imageGradEast.at<uchar>(i, j) = Pixel1b(image, kernel, i, j);

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
    cvtColor(imageOrig, imageGray, COLOR_BGR2GRAY);

    imshow("Original image", imageOrig);
    imshow("GaussianBlur image", imageGaussianBlur);
    imshow("CustomBlur image", customBlur(imageOrig));
    imshow("Gradient East", gradientEast(imageGray));

    waitKey(0);

    return 0;
}
