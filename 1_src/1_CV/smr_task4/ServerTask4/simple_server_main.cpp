#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <stdio.h>
#include <experimental/filesystem>
#include <vector>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
namespace fs = std::experimental::filesystem;


using namespace std;

// using namespace cv;
// namespace cv
// {
//     using std::vector;
// }

// // function declaration
// int tee_classification(Mat hsv, Scalar hsv_low, Scalar hsv_high, double minTargetRadius, Mat &cameraFrame, Scalar circle_colour, Mat &threshold_frame);
// void morphOps(Mat &thresh);
// string printToString(int tmp);

// /// Global variables
// struct hsv_color {
//     int minH=0, maxH=180; //Hue
//     int minS=0, maxS=255; //Saturation    
//     int minV=0, maxV=255; //Value
// };
// hsv_color pink, yellow, soil;

// Mat  hsv, cameraFrame;
// double minTargetRadius = 15;
// bool done = 0;
// cv::Scalar colour_blue(255, 0, 0);
// cv::Scalar colour_red(0, 0, 255);
    std::string data;

// int Number=0;
// int pd = 0;
// int hsv_val[12];
// int i=0;

int main(int, char**)
{
    // pink.minH = 146;
    // pink.maxH = 180;
    // yellow.minH = 30;
    // yellow.maxH = 43;
    // yellow.maxS = 196;
    std::cout << "running....\n";
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

    try
    {
        // Create the socket
        ServerSocket server ( 24920 );

        while ( true )
        {

            ServerSocket new_sock;
            server.accept ( new_sock );
            std::cout<<"Socket accept\n";

            try
            {
                while ( true )
                {

                    //VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
                    cv::VideoCapture cap(-1); // Open the default camera

                    if (!cap.isOpened()) { //check if video device has been initialised
                    cout << "cannot open camera";
                }

                double exposureTime =  -1.0;
                cap.set(cv::CAP_PROP_EXPOSURE, exposureTime);
                //int count_pink;
                //int count_yellow;
                //cap >> cameraFrame; // get a new frame from camera


                //blue detection
                //Scalar hsv_low_pink(pink.minH, pink.minS, pink.minV);
                //Scalar hsv_high_pink(pink.maxH, pink.maxS, pink.maxV);

                //Mat threshold_frame_pink;

                //red detection
                //Scalar hsv_low_yellow(yellow.minH, yellow.minS, yellow.minV);
                //Scalar hsv_high_yellow(yellow.maxH, yellow.maxS, yellow.maxV);
                //Mat threshold_frame_yellow;

                Mat input;
                cap >> input; // Get a new frame from camera
                cap.release();
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


                // //int count_pink_max = 0, count_yellow_max=0;
                // for (int j = 0; j<25; j++)
                // {

                //     stream1.read(cameraFrame);
                //     cvtColor(cameraFrame, hsv, COLOR_BGR2HSV);

                //     count_pink = tee_classification(hsv, 
                //                                     hsv_low_pink, 
                //                                     hsv_high_pink, 
                //                                     minTargetRadius, 
                //                                     cameraFrame, 
                //                                     colour_blue, 
                //                                     threshold_frame_pink);

                //     count_yellow = tee_classification(hsv, 
                //                                     hsv_low_yellow, 
                //                                     hsv_high_yellow, 
                //                                     minTargetRadius, 
                //                                     cameraFrame, 
                //                                     colour_red, 
                //                                     threshold_frame_yellow);
                //     cout << j << " y "<< count_yellow<< " p " << count_pink<< "\n";

                //     if (count_pink_max < count_pink) 
                //         count_pink_max=count_pink;

                //     if (count_yellow_max<count_yellow) 
                //         count_yellow_max=count_yellow;
                // }


                //pd=4;

                string r0, r1, r2, r3;
                // r0 = printToString(count_pink_max);
                // r1 = printToString(count_yellow_max);
                // r2 = printToString(count_yellow_max+count_pink_max);
                // r3 = printToString(pd); 
                r0 = best_fit_label;
                r1 = best_fit_label;
                r2 = best_fit_label;
                r3 = best_fit_label;

                // reply: count blue max, count red max, cbm+crm + pd
                std::cout << r0 << " - "<< r1 << " - "<< r2 << " - "<< r3 <<"  \n ";
                string reply="<vision vis1=\""+r0+"\" vis2=\""+r1+"\" vis3=\""+r2+"\" vis4=\""+r3+"\"/>\n";
                cout << reply;
                reply="<vision vis1=\"1\" vis2=\"2\" vis3=\"3\" vis4=\"4\" vis5=\"5\" vis6=\"6\" vis7=\"7\" vis8=\"8\" vis9=\"9\"/>\n";
                cout << reply;
                new_sock >> data;

                //new_sock << data;
                new_sock << reply;

                }
            }
            catch ( SocketException& ) {}

        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

    return 0;
}

//convert int to string
string printToString(int tmp)
{       
    ostringstream convert; // stream used for the conversion
    convert << tmp; // insert the textual representation of 'Number' in the characters in the stream
    return convert.str();
}



//function detection
int tee_classification(Mat hsv, Scalar hsv_low, Scalar hsv_high, double minTargetRadius, Mat &cameraFrame, Scalar circle_colour, Mat &threshold_frame)
{
    inRange(hsv, hsv_low, hsv_high, threshold_frame);
    morphOps(threshold_frame);

    cv::vector<cv::vector<cv::Point> > contours;
    cv::vector<cv::Vec4i> hierarchy;
    cv::vector<cv::Point2i> center;
    cv::vector<int> radius;

    cv::findContours(threshold_frame.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    size_t count = contours.size();

    for (int i = 0; i<count; i++)
    {
        cv::Point2f c;
        float r;
        cv::minEnclosingCircle(contours[i], c, r);

        if (r >= minTargetRadius)
        {
            center.push_back(c);
            radius.push_back(r);
        }
    }
    count = center.size();

    for (int i = 0; i < count; i++)
    {
        cv::circle(cameraFrame, center[i], radius[i], circle_colour, 3);
    }
    return count;
}


void morphOps(Mat &thresh) {

    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

    erode(thresh, thresh, erodeElement);
    erode(thresh, thresh, erodeElement);

    dilate(thresh, thresh, dilateElement);
    dilate(thresh, thresh, dilateElement);
}
