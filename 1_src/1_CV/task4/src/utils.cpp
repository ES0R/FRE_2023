#include "../inc/utils.hpp"
using namespace cv;

void printImageDetails(const cv::Mat& image) {
    int height = image.rows;
    int width = image.cols;
    printf("Image details: Height = %d, Width = %d\n", height, width);
}

int DisplayImage(const cv::Mat& image){


    if (!image.data) {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(0);  
    return 0;
}