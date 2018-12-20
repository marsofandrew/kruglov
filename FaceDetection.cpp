#include "FaceDetection.hpp"
#include <string>
FaceDetection::FaceDetection(const std::string& cascadeSource):
detection_(),
inited_(false)
{
	detection_.Load(cascadeSource);
}

cv::Mat FaceDetection::detect(cv::Mat &frame)
{
	FaceDetection::Detection::View image = frame;
	if (!inited_)
        {
            detection_.Init(image.Size(), 1.2, image.Size() / 20);
            inited_ = true;
        }
	Detection::Objects objects;
        detection_.Detect(image, objects);

        for (size_t i = 0; i < objects.size(); ++i){
            Simd::DrawRectangle(image, objects[i].rect, Simd::Pixel::Bgr24(0, 255, 255));
	}
	return frame;
}
