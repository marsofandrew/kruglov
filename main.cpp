#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <map>
#include <functional>

#include "opencv2/opencv.hpp"
#include "FaceDetection.hpp"
#include "Sender.hpp"
#include "Filters.hpp"
#include "AnaglyphFunctions.hpp"

const std::string SOCKET_COMMAND = "socket";
const std::string VIDEO_FILE_COMMAND = "video_file";

bool shouldContinueCompute = true;
bool shouldContinueCommands = true;

//global defaults
int anaglyphShift = 8;
int fps = 8;
int width = 640;
int height = 480;
std::string cascadeSource = "";
std::string host = "127.0.0.1";
int port = 5550;
const std::string EXTENTION = ".avi";
std::string outputVideo= "output.avi";

enum WorkType{
	FACE_DETECT,
	ANAGLIPH,
	FILTER_ERODING,
	FILTER_DILATING,
	FILTER_NEGATIVE,
	FILTER_MEDIAN_BLUR,
	FILTER_GAUSSIAN_BLUR,
	EDGE_DETECTION,
	FILTER_HISTOGRAM,
	FIND_CONTOURS,
	RAW_DATA
};

enum OutputMode{
	VIDEO_FILE,
	SOCKET
};

enum InputMode{
	CONSOLE,
	CLIENT
};

OutputMode outputMode = OutputMode::VIDEO_FILE;

WorkType workType = WorkType::RAW_DATA;


namespace Commands{
	void commandExitAction(std::string &)
	{
		shouldContinueCompute = false;
		shouldContinueCommands = false;
	}

	void switchToRawData(std::string &)
	{
		workType = WorkType::RAW_DATA;
	}

	void switchToFaceDetect(std::string &)
	{
		if (cascadeSource == "none" || cascadeSource == ""){
			workType = WorkType::RAW_DATA;
			std::cerr << "No cascade provided\n";
			return;
		}
		workType = WorkType::FACE_DETECT;
	}
	void switchToFilterEroding(std::string &)
	{
		workType = WorkType::FILTER_ERODING;
	}
	void switchToFilterDilating(std::string &)
	{
		workType = WorkType::FILTER_DILATING;
	}
	void switchToFilterNegative(std::string &)
	{
		workType = WorkType::FILTER_NEGATIVE;
	}
	void switchToFilterMedianBlur(std::string &)
	{
		workType = WorkType::FILTER_MEDIAN_BLUR;
	}
	void switchToFilterGaussianBlur(std::string &)
	{
		workType = WorkType::FILTER_GAUSSIAN_BLUR;
	}
	void switchToEdgeDetection(std::string &)
	{
		workType = WorkType::EDGE_DETECTION;
	}
	void switchToFilterHistogram(std::string &)
	{
		workType = WorkType::FILTER_HISTOGRAM;
	}
	void switchToFindContours(std::string &)
	{
		workType = WorkType::FIND_CONTOURS;
	}
	void switchToAnagliph(std::string &s)
	{
    	workType = WorkType::ANAGLIPH;
	}
}

const std::map<std::string, std::function<void(std::string&)>> commands = {
	{"exit", Commands::commandExitAction},
	{"switch_to_raw_data",Commands::switchToRawData},
	{"switch_to_face_detect",Commands::switchToFaceDetect},
	{"switch_to_filter_eroding",Commands::switchToFilterEroding },
	{"switch_to_filter_dilating",Commands::switchToFilterDilating },
	{"switch_to_filter_negative",Commands::switchToFilterNegative },
	{"switch_to_filter_median_blur",Commands::switchToFilterMedianBlur },
	{"switch_to_filter_gaussian_blur",Commands::switchToFilterGaussianBlur },
	{"switch_to_edge_detection",Commands::switchToEdgeDetection },
	{"switch_to_filter_histogram",Commands::switchToFilterHistogram },
	{"switch_to_find_contours",Commands::switchToFindContours },
	{"switch_to_anagliph",Commands::switchToAnagliph }
};

void consoleCommands()
{
	std::string str = "";
	while (shouldContinueCommands){
		std::string command;
		std::cin >> command;
		commands.at(command)(str);
	}
}

void commandsFromServer(){
	
}

void commandThread(const InputMode &mode){
	if (mode == InputMode::CONSOLE){
		consoleCommands();
	}
}

void computeThread(int source){
    cv::VideoCapture capture(source);
    
    if (!capture.isOpened())
    {
        std::cout << "Can't capture '" << source << "' !" << std::endl;
        return;
    }

	FaceDetection faceDetection(cascadeSource);
   
    cv::Mat firstFrame;
    capture >> firstFrame;
    
    width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	std::unique_ptr<cv::VideoWriter> writer = nullptr;
	std::unique_ptr<Sender> sender = nullptr;
	
	switch (outputMode){
		case VIDEO_FILE:
		{
			writer = std::make_unique<cv::VideoWriter>(outputVideo, CV_FOURCC('M','J','P','G'), 
					fps, cv::Size(width, height), true);
			break;
		}
		case SOCKET:
		{
			sender = std::make_unique<Sender>(host, port);
			break;
		}
	}
 
    while (shouldContinueCompute)
    {
        cv::Mat frame;
        capture >> frame;
		
		switch (workType){
			case FACE_DETECT:
			{
				frame = faceDetection.detect(frame);
				break;
			}
			case RAW_DATA:
			{
				break;
			}
			case FILTER_DILATING:
			{
				frame = doDilating(frame);
				break;
			}
			
			case FILTER_ERODING:
			{
				frame = doEroding(frame);
				break;
			}
			case FILTER_NEGATIVE:
			{
				frame = doNegative(frame);
				break;
			}
			case FILTER_HISTOGRAM:
			{
				frame = doHistogramEqual(frame);
				break;
			}
			case FILTER_GAUSSIAN_BLUR:
			{
				frame = doGaussianBlur(frame);
				break;
			}
			case FILTER_MEDIAN_BLUR:
			{
				frame = doMedianBlur(frame);
				break;
			}
			case ANAGLIPH:
			{
				cv::Mat tempFrame = frame;
				tempFrame = makeAnaglyph(tempFrame, anaglyphShift);
				cv::resize(tempFrame, frame, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
				break;
			}
			case EDGE_DETECTION:
			{
				cv::Mat tempFrame = frame;
				tempFrame = doCannyEdgeDetection(frame);
				cv::resize(tempFrame, frame, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
				break;
			}
			case FIND_CONTOURS:
			{
				cv::Mat tempFrame = frame;
				tempFrame = doContours(frame);
				cv::resize(tempFrame, frame, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
				break;
			}
			default:
			{
				std::cerr<<"no known function\n";
			}
			
			//WRITE HERE YOUR FUNCTIONS
		}
		
		
		switch (outputMode){
			case VIDEO_FILE: 
			{
				writer->write(frame);
				break;
			}
			case SOCKET:
			{
				sender->write(frame);
				break;
			}
		}
		
    }
	
}

void printHelp()
{
	std::cout << "It will be implemented later\n";
}

int main(int argc, char * argv[])
{	
	cv::CommandLineParser parser(argc, argv,
		"{help h||}"
		"{source|0|}"
		"{output|outputvideo|}"
		"{cascade|none|}"
		"{fps|8|}"
		"{start_mode|switch_to_raw_data|}"
	);
	
	if(parser.has("help")){
		printHelp();
		return 0;
	}
	
	std::string str = "";
	int source = parser.get<int>("source");
	cascadeSource = parser.get<std::string>("cascade");
	
	outputVideo = parser.get<std::string>("output") + EXTENTION;
	commands.at(parser.get<std::string>("start_mode"))(str);
	fps = parser.get<int>("fps");
	
	outputMode = OutputMode::VIDEO_FILE;
	
	
	//It would be fine to write all commands that aveilable for this program 
	
	std::thread thread1(commandThread, InputMode::CONSOLE);
	std::thread thread2(computeThread, source);
	
	thread1.join();
	thread2.join();
   
    return 0;
}


