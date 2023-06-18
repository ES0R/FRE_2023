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
string printToString(int tmp);

/// Global variables
struct hsv_color {
    int minH=0, maxH=180; //Hue
    int minS=0, maxS=255; //Saturation    
    int minV=0, maxV=255; //Value
};
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
    pink.minH = 146;
    pink.maxH = 180;
    yellow.minH = 30;
    yellow.maxH = 43;
    yellow.maxS = 196;
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


                int count_pink;
                int count_yellow;
                //cap >> cameraFrame; // get a new frame from camera


                //blue detection
                Scalar hsv_low_pink(pink.minH, pink.minS, pink.minV);
                Scalar hsv_high_pink(pink.maxH, pink.maxS, pink.maxV);

                Mat threshold_frame_pink;

                //red detection
                Scalar hsv_low_yellow(yellow.minH, yellow.minS, yellow.minV);
                Scalar hsv_high_yellow(yellow.maxH, yellow.maxS, yellow.maxV);
                Mat threshold_frame_yellow;


                int count_pink_max = 0, count_yellow_max=0;
                for (int j = 0; j<25; j++)
                {

                    stream1.read(cameraFrame);
                    cvtColor(cameraFrame, hsv, COLOR_BGR2HSV);

                    count_pink = tee_classification(hsv, 
                                                    hsv_low_pink, 
                                                    hsv_high_pink, 
                                                    minTargetRadius, 
                                                    cameraFrame, 
                                                    colour_blue, 
                                                    threshold_frame_pink);

                    count_yellow = tee_classification(hsv, 
                                                    hsv_low_yellow, 
                                                    hsv_high_yellow, 
                                                    minTargetRadius, 
                                                    cameraFrame, 
                                                    colour_red, 
                                                    threshold_frame_yellow);
                    cout << j << " y "<< count_yellow<< " p " << count_pink<< "\n";

                    if (count_pink_max < count_pink) 
                        count_pink_max=count_pink;

                    if (count_yellow_max<count_yellow) 
                        count_yellow_max=count_yellow;
                }


                pd=4;

                string r0, r1, r2, r3;
                r0 = printToString(count_pink_max);
                r1 = printToString(count_yellow_max);
                r2 = printToString(count_yellow_max+count_pink_max);
                r3 = printToString(pd); 

                std::cout << count_yellow << "  \n " << count_pink<< "  \n ";

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
