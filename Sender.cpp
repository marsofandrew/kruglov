#include "Sender.hpp"

#include <string>
#include <iostream>
#include <stdexcept>
#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

Sender::Sender(std::string hostname, int port):
socketFd_(-1)
{
	socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd_ < 0){
		std::cerr << "Can't open socket!!!\n";
		throw std::runtime_error("Can't open socket!!!");
	}
	hostent *server;
	
	server = gethostbyname(hostname.c_str());
	if (server == NULL){
		std::cerr << "No such host!!!\n";
		throw std::runtime_error("No such host!!!");
	}
	
	sockaddr_in serv_addr;
	bzero((char*) &serv_addr, sizeof(&serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	if(connect(socketFd_, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		std::cerr << "Can not connect to server\n";
		throw std::runtime_error("Can not connect to server");
	}
}

Sender::~Sender()
{
	close(socketFd_);
}

void Sender::write(cv::Mat &frame)
{
	frame = frame.reshape(0,1);
	int imgSize = frame.total()*frame.elemSize();
	int bytesDone = send(socketFd_, frame.data, imgSize, 0);
	
	if (bytesDone < 0){
		std::cerr << "No data sended\n";
	}
}


