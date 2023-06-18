//Includes
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "ServerSocket.h"
#include "SocketException.h"
#include <stdio.h>
#include <experimental/filesystem>
#include <vector>

//Namespace
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
namespace fs = std::experimental::filesystem;


//Main
int main()
{
    //Load images from `images` and make labels
    std::string folder = "images/";
    std::vector<cv::Mat> images;
    std::vector<std::string> labels;

    std::string best_fit_label;

    for (const auto & entry : fs::directory_iterator(folder)) {
        images.push_back(imread(entry.path()));

        std::string filename = entry.path().filename().string();

        if(filename.find("human") != std::string::npos) {
            labels.push_back("human");
        } else if(filename.find("deer") != std::string::npos) {
            labels.push_back("deer");
        } else {
            labels.push_back("unrecognized");
        }
    }

    //Camera Capture
    cv::VideoCapture cap(-1); // Open the default camera

    if(!cap.isOpened())  // Check if we succeeded
    {
        printf("Could not open the camera\n");
        return -1;
    }

    double exposureTime =  -1.0;
    cap.set(cv::CAP_PROP_EXPOSURE, exposureTime);


    Mat input;
    cap >> input; // Get a new frame from camera
    cap.release();

    //cv::imwrite("captured_image.jpg", input);

    // If the image is colored, convert it to grayscale
    if(input.channels() > 1) {
        cvtColor(input, input, COLOR_BGR2GRAY);
    }

    //Check input
    if (input.empty()) {
        printf("Could not capture an image\n");
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

        
        std::vector< std::vector<DMatch> > knn_matches;
        matcher.knnMatch(descriptors_input, descriptors_image, knn_matches, 2);

        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++) {
            if (knn_matches[i][0].distance < 0.4 * knn_matches[i][1].distance) {
                good_matches.push_back(knn_matches[i][0]);
            }
        }

        double match = good_matches.size();
        if (match > max_match) {
            max_match = match;
            best_fit = image;
            best_fit_label = entry.path().filename().string();

            Mat img_matches;
            drawMatches( input, keypoints_input, best_fit, keypoints_image,
                     good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                     std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        }
    }

    std::cout << "Guess: " << best_fit_label << std::endl;

    return 0;
}
