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

std::string data;

///Constants
///Modify these values to filter out different range of colors.
/*
int HUE_MIN_BLUE = 90, HUE_MAX_BLUE = 130, HUE_MIN_RED = 0, HUE_MAX_RED = 13; //Hue
int SAT_MIN_BLUE = 150, SAT_MAX_BLUE = 255, SAT_MIN_RED = 100, SAT_MAX_RED = 255; //Saturation
int VAL_MIN_BLUE = 150, VAL_MAX_BLUE = 255, VAL_MIN_RED = 100, VAL_MAX_RED = 255; //Value
*/
int HUE_MIN_BLUE = 28, HUE_MAX_BLUE = 130, HUE_MIN_RED = 0, HUE_MAX_RED = 13; //Hue
int SAT_MIN_BLUE = 150, SAT_MAX_BLUE = 255, SAT_MIN_RED = 100, SAT_MAX_RED = 255; //Saturation
int VAL_MIN_BLUE = 150, VAL_MAX_BLUE = 255, VAL_MIN_RED = 100, VAL_MAX_RED = 255; //Value



/// Global variables
Mat  hsv, cameraFrame;
double minTargetRadius = 15;
bool done = 0;
cv::Scalar colour_blue(255, 0, 0);
cv::Scalar colour_red(0, 0, 255);

// function declaration
int tee_classification(Mat hsv, Scalar hsv_low, Scalar hsv_high, double minTargetRadius, Mat &cameraFrame, Scalar circle_colour, Mat &threshold_frame);
void morphOps(Mat &thresh);


int Number=0;
int pd = 0;
int hsv_val[12];
int i=0;

int main(int, char**)
{
/*
string line;
  ifstream myfile ("logParam.txt");
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
		getline (myfile,line);
		hsv_val[i]=atoi(line.c_str());
      		cout << line << endl;
		i=i+1;
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

HUE_MIN_BLUE = hsv_val[0];
SAT_MIN_BLUE = hsv_val[1];
VAL_MIN_BLUE = hsv_val[2];

HUE_MAX_BLUE = hsv_val[3];
SAT_MAX_BLUE = hsv_val[4];
VAL_MAX_BLUE = hsv_val[5];

HUE_MIN_RED = hsv_val[6];
SAT_MIN_RED = hsv_val[7];
VAL_MIN_RED = hsv_val[8];

HUE_MAX_RED = hsv_val[9];
SAT_MAX_RED = hsv_val[10];
VAL_MAX_RED = hsv_val[11];

cout << HUE_MIN_BLUE;
*/

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

		count_blue = tee_classification(hsv, hsv_low_blue, hsv_high_blue, minTargetRadius, cameraFrame, colour_blue, threshold_frame_blue);
		count_red = tee_classification(hsv, hsv_low_red, hsv_high_red, minTargetRadius, cameraFrame, colour_red, threshold_frame_red);
cout << j << "  "<< count_red<< "  " << count_blue<< "\n";

	if (count_blue_max<count_blue) count_blue_max=count_blue;
	if (count_red_max<count_red) count_red_max=count_red;
	}




      pd=0;
// TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4 TASK 4
      if ((count_red_max+count_blue_max)>=1) {
	      if (count_red_max>count_blue_max) pd=1;
	      else pd=2;
      }



              string Result;          // string which will contain the result
              ostringstream convert;   // stream used for the conversion
              convert << count_blue_max;      // insert the textual representation of 'Number' in the characters in the stream
              Result = convert.str();
		string Result2;
ostringstream convert2; 
convert2<<count_red_max;
Result2=convert2.str();		
		
		string Result3;
ostringstream convert3; 
convert3<<(count_red_max+count_blue_max);
Result3=convert3.str();	

		string Result4;
ostringstream convert4; 
convert4<<pd;
Result4=convert4.str();	

std::cout << count_red << "  \n " << count_blue<< "  \n ";

             string reply="<vision vis1=\""+Result+"\" vis2=\""+Result2+"\" vis3=\""+Result3+"\" vis4=\""+Result4+"\"/>\n";


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
