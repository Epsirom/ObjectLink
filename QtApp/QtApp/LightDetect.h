#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class LightDetect
{
public:
	LightDetect(const cv::Mat& img, int patchRadius);
	~LightDetect();

	double pointSimilarity(const cv::Vec3b& p1, const cv::Vec3b& p2);
	void splitCon();
	cv::Vec2b getLightDirection();

protected:
	cv::Mat img;
	int patchRadius;
	cv::Mat mean;
	cv::Mat con;
	cv::Mat blockMap;
	std::vector<cv::Vec3b> blocks;
};

