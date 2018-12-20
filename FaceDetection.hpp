
#ifndef FACE_DETECTION
#define FACE_DETECTION

#include <string>
#include "opencv2/opencv.hpp"
#ifndef SIMD_OPENCV_ENABLE
#define SIMD_OPENCV_ENABLE
#endif
#include "Simd/SimdDetection.hpp"
#include "Simd/SimdDrawing.hpp"

class FaceDetection
{
public:
	using Detection = Simd::Detection<Simd::Allocator>;
	
	FaceDetection(const std::string& cascadeSource);
	cv::Mat detect(cv::Mat &frame);
	
private:
	Detection detection_;
	bool inited_;
};

#endif
