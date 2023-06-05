//Includes
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <stdio.h>
#include "../inc/utils.hpp"
#include <filesystem>
#include <vector>

//Namespace
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
namespace fs = std::filesystem; 

//Main
int main(int argc, char** argv)
{
    //Check input
    if (argc != 2) {
    printf("Invalid input - No argument given\n");
    return -1;
    }

    //Load images from `images` and make labels
    std::string folder = "images/";
    std::vector<cv::Mat> images;
    std::vector<std::string> labels;

    for (const auto & entry : fs::directory_iterator(folder)) {
        images.push_back(imread(entry.path()));

        std::string filename = entry.path().filename().string();

        if(filename.find("human") != std::string::npos) {
            labels.push_back("human");
        } else if(filename.find("deer") != std::string::npos) {
            labels.push_back("deer");
        } else if(filename.find("unknown") != std::string::npos) {
            labels.push_back("unknown");
        } else {
            labels.push_back("unrecognized");
        }
    }

    /*Tester
    if (!images.empty()) {
        namedWindow("Display Image", WINDOW_AUTOSIZE);
        if (!labels.empty()) {  
            printf("%s\n", labels[1].c_str());
        }
        imshow("Display Image", images[1]);
        waitKey(0);
    }
    */
  
    //Load camera photo
    Mat input = imread(argv[1], IMREAD_GRAYSCALE);

    //Check input
    if (input.empty()) {
        printf("Could not open or find the image\n");
        return -1;
    }

    //SIFT
    Ptr<SIFT> detector = SIFT::create();
    std::vector<KeyPoint> keypoints_input;
    Mat descriptors_input;
    detector->detectAndCompute(input, noArray(), keypoints_input, descriptors_input);

    BFMatcher matcher;
    double max_match = 0;
    Mat best_fit;

    for (const auto & entry : fs::directory_iterator(folder)) {
        Mat image = imread(entry.path(), IMREAD_GRAYSCALE);

        if (image.empty()) {
            printf("Could not open or find the image\n");
            continue;
        }

        std::vector<KeyPoint> keypoints_image;
        Mat descriptors_image;
        detector->detectAndCompute(image, noArray(), keypoints_image, descriptors_image);

        
        std::vector< DMatch > matches;
        matcher.match( descriptors_input, descriptors_image, matches );

        
        double match = matches.size();
        if (match > max_match) {
            max_match = match;
            best_fit = image;

            Mat img_matches;
            drawMatches( input, keypoints_input, best_fit, keypoints_image,
                         matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                         std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

            imshow("Best fit so far", image);
        }
    }

    waitKey(0);

    return 0;
}