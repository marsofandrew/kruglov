#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>
#include <iosfwd>

bool interrupted = false;
cv::Mat doNothing(cv::Mat& frame);
cv::Mat doHistogramEqual(cv::Mat& frame);
cv::Mat doHomogenBlur(cv::Mat& frame);
cv::Mat doGaussianBlur(cv::Mat& frame);
cv::Mat doMedianBlur(cv::Mat& frame);
cv::Mat doEroding(cv::Mat& frame);
cv::Mat doDilating(cv::Mat& frame);
cv::Mat doNegative(cv::Mat& frame);
cv::Mat doCannyEdgeDetection(cv::Mat& frame);
cv::Mat doContours(cv::Mat& frame);


using filter =cv::Mat(cv::Mat&);
template <filter& doFilter>
void showWebCamVid()
{
  cv::VideoCapture videoCapture(0);
  if (!videoCapture.isOpened())
  {
    return;
  }

  cv::namedWindow("Raw", CV_WINDOW_NORMAL);
  cv::resizeWindow("Raw", 640, 480);

  while (!interrupted)
  {
    cv::Mat frame;
    videoCapture.read(frame);
    cv::Mat filtered = doFilter(frame);
    cv::imshow("Raw", filtered);
    cv::waitKey(30);
  }
  cv::destroyAllWindows();
}


#endif //FILTERS_H

