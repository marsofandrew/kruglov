#include "AnaglyphFunctions.hpp"

cv::Mat makeAnaglyph(cv::Mat &image, int shift)
{
	cv::Mat left(image.size(), CV_8UC3);
	cv::Mat right(image.size(), CV_8UC3);
	int width = image.size().width;
	int height = image.size().height;
	cv::Mat anaglyph(width - shift, height, CV_8UC3);
	std::vector<cv::Mat> channelsRed(3);
	cv::split(image, channelsRed);
	std::vector<cv::Mat> channelsCyan(3);
	cv::split(image, channelsCyan);
	channelsRed[0].convertTo(channelsRed[0], -1, 0, 0);
	channelsRed[1].convertTo(channelsRed[1], -1, 0, 0);
	cv::merge(channelsRed, left);
	channelsCyan[2].convertTo(channelsCyan[2], -1, 0, 0);
	cv::merge(channelsCyan, right);
	left = left(cv::Rect(shift, 0, width - shift, height));
	right = right(cv::Rect(0, 0, width - shift, height));
	cv::add(left, right, anaglyph);
	return anaglyph;
}

int performWebcam()
{
  int shift = 8;
  cv::VideoCapture cap(0);
  if (!cap.isOpened())
  {
	cv::destroyAllWindows();
	return -1;
  }
  cv::namedWindow("Webcam");
  cv::Mat image;
  while (true)
  {
	cap >> image;
	cv::Mat anaglyph = makeAnaglyph(image, shift);
	cv::imshow("Webcam", anaglyph);
	if (cv::waitKey(10) >= 0) break;
  }
  cv::destroyAllWindows();
  return 0;
}

int performImage(std::string &str)
{
	int shift = 8;
	cv::Mat image = cv::imread(str);
	if (image.empty())
	{
		cv::destroyAllWindows();
		return -1;
	}
	cv::Mat anaglyph = makeAnaglyph(image, shift);
	cv::namedWindow("Image");
	cv::imshow("Image", anaglyph);
	cv::waitKey(0);
	bool isSuccess = imwrite("D:\\Results\\Result.jpg", anaglyph);
	if (!isSuccess)
	{
		cv::destroyAllWindows();
		return -2;
	}
	cv::destroyAllWindows();
	return 0;
}

int performVideo(std::string &str)
{
	int shift = 8;
	cv::VideoCapture cap(str);
	if (!cap.isOpened())
	{
		cv::destroyAllWindows();
		return -1;
	}
	cv::Mat image;
	bool endOfTheVideoFlag = cap.read(image);
	cv::Mat anaglyph = makeAnaglyph(image, shift);
	cv::namedWindow("Video");
	int frame_width = anaglyph.size().width;
	int frame_height = anaglyph.size().height;
	cv::Size frame_size(frame_width, frame_height);
	int frames_per_second = 10;
	cv::VideoWriter oVideoWriter("D:\\Results\\ResultVideo.avi", cv::VideoWriter::fourcc('D', 'I', 'V', 'X'),
		frames_per_second, frame_size, true);
	if (oVideoWriter.isOpened() == false)
	{
		cv::destroyAllWindows();
		return -2;
	}
	while (true)
	{
		endOfTheVideoFlag = cap.read(image);
		if (endOfTheVideoFlag == false)
		{
			break;
		}
		anaglyph = makeAnaglyph(image, shift);
		oVideoWriter.write(anaglyph);
		cv::imshow("Video", anaglyph);
		if (cv::waitKey(30) >= 0) break;
	}
	oVideoWriter.release();
	cv::destroyAllWindows();
	return 0;
}
