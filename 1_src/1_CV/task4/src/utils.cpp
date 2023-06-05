#include "../inc/utils.hpp"


void printImageDetails(const cv::Mat& image) {
    int height = image.rows;
    int width = image.cols;
    printf("Image details: Height = %d, Width = %d\n", height, width);
}
