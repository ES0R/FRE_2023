# Makefile for the socket programming example
#

simple_server_objects = ServerSocket.o Socket.o simple_server_main.o
simple_client_objects = ClientSocket.o Socket.o simple_client_main.o


all : simple_server simple_client
OPENCV = `pkg-config opencv --cflags --libs`	
LIBS = $(OPENCV)

simple_server: $(simple_server_objects)

	
	g++ -std=c++0x -o simple_server $(simple_server_objects) $(LIBS)
	#g++ -std=c++11 simple_server_main.cpp `pkg-config --libs --cflags opencv` -o simple_server $(simple_server_objects)
	#g++ -std=c++11 `pkg-config --libs --cflags opencv` -o simple_server $(simple_server_objects)

simple_client: $(simple_client_objects)
	g++ -o simple_client $(simple_client_objects)


Socket: Socket.cpp
ServerSocket: ServerSocket.cpp
ClientSocket: ClientSocket.cpp
simple_server_main: simple_server_main.cpp
simple_client_main: simple_client_main.cpp


clean:
	rm -f *.o simple_server simple_client
