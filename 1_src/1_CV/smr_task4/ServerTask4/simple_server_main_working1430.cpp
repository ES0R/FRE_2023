#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>


#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;

cv::Mat test;
int Number=0;

//int main ( int argc, int argv[] )
int main(int, char**)
{
  std::cout << "running....\n";


  try
    {
      // Create the socket
      ServerSocket server ( 30000 );

      while ( true )
    {

      ServerSocket new_sock;
      server.accept ( new_sock );

      try
        {
          while ( true )
        {


              string Result;          // string which will contain the result
              ostringstream convert;   // stream used for the conversion
              convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
              Result = convert.str();

          std::string data;
          new_sock >> data;
          //new_sock << data;
          string reply="<vision vis1=\"6.0\"/>\n";

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
