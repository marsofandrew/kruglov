#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat doNothing(cv::Mat& frame)
{
  return frame;
}

//tries to make the best brightness
cv::Mat doHistogramEqual(cv::Mat& frame)
{
  cv::Mat hist_equalized_image;
  cv::cvtColor(frame, hist_equalized_image, cv::COLOR_BGR2YCrCb); //BGR -> Y Cr Cb

//Split the image into 3 channels; Y, Cr and Cb channels respectively and store it in a std::vector
  std::vector<cv::Mat> vec_channels;
  cv::split(hist_equalized_image, vec_channels);

  //Equalize the histogram of only the Y channel 
  cv::equalizeHist(vec_channels[0], vec_channels[0]);

  //Merge 3 channels in the vector to form the color image in YCrCB color space.
  cv::merge(vec_channels, hist_equalized_image);

  //Convert the histogram equalized image from YCrCb to BGR color space again
  cv::cvtColor(hist_equalized_image, hist_equalized_image, cv::COLOR_YCrCb2BGR);
  return hist_equalized_image;
}

cv::Mat doHomogenBlur(cv::Mat& frame)
{
  cv::Mat temp;
  cv::blur(frame, temp, cv::Size(3, 3));
  return temp;
}


cv::Mat doGaussianBlur(cv::Mat& frame)
{
  cv::Mat temp;
  cv::GaussianBlur(frame, temp, cv::Size(5, 5), 0);//supports in-pace filtering
  return temp;
}


cv::Mat doMedianBlur(cv::Mat& frame)
{
  cv::Mat temp;
  medianBlur(frame, temp, 5);//supports in-pace filtering
  return temp;
}

//increases the dark parts of the image (chooses the darkest pixel from the neighboorhood)
cv::Mat doEroding(cv::Mat& frame)
{
  cv::Mat temp;
  cv::erode(frame, temp, cv::Mat(), cv::Point(-1, -1), 5);
  //Mat() sets default 3*3 kernel. (-1,-1) - anchor will be center of element. 2 - number of iterations
  return temp;
}


//increases the light parts of the image (chooses the lightest pixel from the neighboorhood)
cv::Mat doDilating(cv::Mat& frame)
{
  cv::Mat temp;
  cv::dilate(frame, temp, cv::Mat(), cv::Point(-1, -1), 3);
  return temp;
}

cv::Mat doNegative(cv::Mat& frame)
{
  std::vector<cv::Mat> vec_channels;
  cv::split(frame, vec_channels);
  for (int i = 0; i <= 2; i++)
  {
    vec_channels[i] = 255 - vec_channels[i];
  }

  cv::merge(vec_channels, frame);
  return frame;
}

//cannot swap and mae it void. because colored != binary (8uC3 !=8UC)
cv::Mat doCannyEdgeDetection(cv::Mat& frame)
{
  //canny edge detection is done before contouring.
  //1. turn colored iamge to binary.
  //2. gaussian blur 5*5 
  //3. canny() method
  cv::Mat binImage;
  cv::cvtColor(frame, binImage, CV_BGR2GRAY);
  doGaussianBlur(binImage);
  cv::Mat canniedFrame;
  cv::Canny(binImage, canniedFrame, 60, 120);
  return canniedFrame;
}

cv::Mat doContours(cv::Mat& frame)
{
  cv::Mat canniedFrame = doCannyEdgeDetection(frame);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(canniedFrame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

  cv::Mat drawing = cv::Mat::zeros(canniedFrame.size(), CV_8UC3); //matrix filled with zeros
  for (int i = 0; i< contours.size(); i++)
  {
    drawContours(drawing, contours, i, cv::Scalar(255, 255, 255), 2, 8, hierarchy, 0, cv::Point());
  }
  return drawing;
}
