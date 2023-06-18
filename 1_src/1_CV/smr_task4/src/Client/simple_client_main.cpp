#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include <unistd.h>

int main( int argc, int argv[] )
//int main(int, char**)
{

    //while(true)
    //{
  try
    {

      ClientSocket client_socket ( "localhost", 24920 );

      std::string reply;

      try
    {
      client_socket << "Test message.";
      client_socket >> reply;
    }
      catch ( SocketException& ) {}

      std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;

    }
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\n";
    }

       // usleep(1000000);
    //}

  return 0;
}
