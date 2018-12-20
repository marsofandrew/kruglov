#ifndef Anaglyph_Functions_hpp
#define Anaglyph_Functions_hpp

#include "opencv2/opencv.hpp"  
#include <iostream>
#include <string>

cv::Mat makeAnaglyph(cv::Mat &image, int shift);

int performWebcam();

int performImage(std::string &str);

int performVideo(std::string &str);

#endif //Anaglyph_Functions_hpp
