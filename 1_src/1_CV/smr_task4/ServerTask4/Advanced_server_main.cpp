#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <fstream>

#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;

using namespace cv;
namespace cv
{
    using std::vector;
}

// function declaration
int tee_classification(Mat hsv, Scalar hsv_low, Scalar hsv_high, double minTargetRadius, Mat &cameraFrame, Scalar circle_colour, Mat &threshold_frame);
void morphOps(Mat &thresh);

/// Global variables
struct hsv_color {
    int minH, maxH; //Hue
    int minS, maxS; //Saturation    
    int minV, maxV; //Value
}
hsv_color pink, yellow, soil;

Mat  hsv, cameraFrame;
double minTargetRadius = 15;
bool done = 0;
cv::Scalar colour_blue(255, 0, 0);
cv::Scalar colour_red(0, 0, 255);
std::string data;

int Number=0;
int pd = 0;
int hsv_val[12];
int i=0;

int main(int, char**)
{


    std::cout << "running....\n";

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

                    VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.

                    if (!stream1.isOpened()) { //check if video device has been initialised
                    cout << "cannot open camera";
                }


                int count_blue;
                int count_red;
                //cap >> cameraFrame; // get a new frame from camera


                //blue detection
                Scalar hsv_low_blue(HUE_MIN_BLUE, SAT_MIN_BLUE, VAL_MIN_BLUE);
                Scalar hsv_high_blue(HUE_MAX_BLUE, SAT_MAX_BLUE, VAL_MAX_BLUE);

                Mat threshold_frame_blue;

                //red detection
                Scalar hsv_low_red(HUE_MIN_RED, SAT_MIN_RED, VAL_MIN_RED);
                Scalar hsv_high_red(HUE_MAX_RED, SAT_MAX_RED, VAL_MAX_RED);
                Mat threshold_frame_red;


                int count_blue_max = 0, count_red_max=0;
                for (int j = 0; j<25; j++)
                {

                    stream1.read(cameraFrame);
                    cvtColor(cameraFrame, hsv, COLOR_BGR2HSV);

                    count_blue = tee_classification(hsv, 
                                                    hsv_low_blue, 
                                                    hsv_high_blue, 
                                                    minTargetRadius, 
                                                    cameraFrame, 
                                                    colour_blue, 
                                                    threshold_frame_blue);

                    count_red = tee_classification(hsv, 
                                                    hsv_low_red, 
                                                    hsv_high_red, 
                                                    minTargetRadius, 
                                                    cameraFrame, 
                                                    colour_red, 
                                                    threshold_frame_red);
                    cout << j << "  "<< count_red<< "  " << count_blue<< "\n";

                    if (count_blue_max < count_blue) 
                        count_blue_max=count_blue;

                    if (count_red_max<count_red) 
                        count_red_max=count_red;
                }


                pd=0;

                string r0, r1, r2, r3;
                r0 = printToString(count_blue_max);
                r1 = printToString(count_red_max);
                r2 = printToString(count_red_max+count_blue_max);
                r3 = printToString(pd); 

                std::cout << count_red << "  \n " << count_blue<< "  \n ";

                // reply: count blue max, count red max, cbm+crm + pd
                string reply="<vision vis1=\""+r0+"\" vis2=\""+r1+"\" vis3=\""+r2+"\" vis4=\""+r3+"\"/>\n";


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
        Std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
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
