#ifndef SENDER_HPP
#define DENDRE_HPP

#include <string>
#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

class Sender
{
public:
	Sender(std::string hostname, int port);
	~Sender();
	void write(cv::Mat &frame);

private:
	int socketFd_;
	
};


#endif