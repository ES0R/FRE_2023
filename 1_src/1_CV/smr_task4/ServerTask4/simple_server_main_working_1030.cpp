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
      ServerSocket server ( 24920 );

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
          string reply="<vision vis1=\"1\" vis2=\"2\" vis3=\"3\" vis4=\"4\" vis5=\"5\" vis6=\"6\" vis7=\"7\" vis8=\"8\" vis9=\"9\"/>\n";

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
