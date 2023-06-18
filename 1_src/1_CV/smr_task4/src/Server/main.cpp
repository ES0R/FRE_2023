#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "../inc/ServerSocket.h"
#include "../inc/SocketException.h"
#include <stdio.h>
#include <experimental/filesystem>
#include <vector>
#include <iostream>

// Namespace
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;
namespace fs = std::experimental::filesystem;

// Main
int main()
{
    // Load images from `images` and make labels
    std::string folder = "images/";
    std::vector<cv::Mat> images;
    std::vector<std::string> labels;
    std::string best_fit_label;
    std::string final_guess = "0";

    for (const auto &entry : fs::directory_iterator(folder))
    {
        images.push_back(imread(entry.path()));

        std::string filename = entry.path().filename().string();

        if (filename.find("2") != std::string::npos) // HUMAN
        {
            labels.push_back("2");
        }
        else if (filename.find("1") != std::string::npos) // DEER
        {
            labels.push_back("1");
        }
        else
        {
            labels.push_back("unrecognized");
        }
    }

    try
    {
        // Create the socket
        std::cout << "Initalizing server\n";
        ServerSocket server(24920);
        while (true)
        {
            ServerSocket new_sock;
            server.accept(new_sock);
            std::cout << "Socket accept\n";
            try
            {
                while (true)
                {
                    // Camera Capture
                    cv::VideoCapture cap(-1); // Use -1 or 0

                    if (!cap.isOpened())
                    {
                        printf("Could not open the camera\n");
                        return -1;
                    }

                    Mat input;
                    cap >> input;
                    cap.release();


                    
                    cv::imwrite("captured_image.jpg", input); //Save image optional line

                    // Make camera take gray instead of this?
                    if (input.channels() > 1)
                    {
                        cvtColor(input, input, COLOR_BGR2GRAY);
                    }

                    // Check input
                    if (input.empty())
                    {
                        printf("Could not capture an image\n");
                        return -1;
                    }

                    // SIFT
                    Ptr<SIFT> detector = SIFT::create();
                    std::vector<KeyPoint> keypoints_input;
                    Mat descriptors_input;
                    detector->detectAndCompute(input, noArray(), keypoints_input, descriptors_input);

                    BFMatcher matcher;
                    double max_match = 0;
                    Mat best_fit;

                    for (const auto &entry : fs::directory_iterator(folder))
                    {
                        Mat image = imread(entry.path(), IMREAD_GRAYSCALE);

                        if (image.empty())
                        {
                            printf("Could not open or find the image\n");
                            continue;
                        }

                        std::vector<KeyPoint> keypoints_image;
                        Mat descriptors_image;
                        detector->detectAndCompute(image, noArray(), keypoints_image, descriptors_image);

                        std::vector<std::vector<DMatch>> knn_matches;
                        matcher.knnMatch(descriptors_input, descriptors_image, knn_matches, 2);

                        std::vector<DMatch> good_matches;
                        //Between 0.3 and 0.4 is best
                        float threshhold = 0.4; // Between [0;1], 0 is strict 1 is accepting

                        for (size_t i = 0; i < knn_matches.size(); i++)
                        {
                            if (knn_matches[i][0].distance < threshhold * knn_matches[i][1].distance)
                            {
                                good_matches.push_back(knn_matches[i][0]);
                            }
                        }

                        double match = good_matches.size();
                        if (match > max_match)
                        {
                            max_match = match;
                            best_fit = image;

                            best_fit_label =  entry.path().filename().string();
                            

                            if (best_fit_label.find("deer") != std::string::npos) {
                                final_guess = "1";
                            } else if (best_fit_label.find("human") != std::string::npos) {
                                final_guess = "2";
}

                            Mat img_matches;
                            drawMatches(input, keypoints_input, best_fit, keypoints_image,
                                        good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                                        std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
                        }
                    }

                    string data;
                    //string r0 = "0";
                    //string r1 = "1";
                    //string r2 = "2";
                    //string r3 = "3";
                    //reply="<vision vis1=\"1\" vis2=\"2\" vis3=\"3\" vis4=\"4\" vis5=\"5\" vis6=\"6\" vis7=\"7\" vis8=\"8\" vis9=\"9\"/>\n";
                    //cout << reply;
                    //string reply = "<vision vis1=\""+r0+"\" vis2=\""+r1+"\" vis3=\""+r2+"\" vis4=\""+r3+"\"/>\n";
                    string reply =  "<vision vis1=\""+final_guess+"\" />\n";
                    final_guess = "0";
                    cout << reply;

                    new_sock >> data;

                    //new_sock << data;
                    new_sock << reply;
                }
            }
            catch (const SocketException &e){
                std::cerr << "Exception 1: "; //<< e.what() << std::endl;
            }
            /*
            catch (const Exception &e){
                std::cerr << "Exception 2: " << e.what() << std::endl;
            }
            */


        }
    }
    catch (const SocketException &e)
    {
        std::cerr << "Exception 2: \n";
    }

    std::cout << "Guess: " << best_fit_label << std::endl;
    

    return 0;
}