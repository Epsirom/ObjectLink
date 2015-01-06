#include "LightDetect.h"


LightDetect::LightDetect(const cv::Mat& img, int patchRadius)
{
	this->img = img.clone();
	this->patchRadius = patchRadius;

	int patchSize = 2 * patchRadius + 1;

	this->mean = img.clone();
	cv::blur(this->img, this->mean, cv::Size(patchSize, patchSize));
	splitCon();
}


LightDetect::~LightDetect()
{
}


double LightDetect::pointSimilarity(const cv::Vec3b& p1, const cv::Vec3b& p2)
{
	cv::Vec3d s;
	double m = 0;
	for (int i = 0; i < 3; ++i)
	{
		s[i] = (p1[i] + 0.1) / (p2[i] + 0.1);
		m += s[i];
	}
	m /= 3;
	double result = 0;
	for (int i = 0; i < 3; ++i)
	{
		double t = s[i] - m;
		result += t * t;
	}
	return result;
}

void LightDetect::splitCon()
{
	con = cv::Mat(img.size(), CV_64FC1);
	int r = patchRadius;
	std::vector<double> cons;
	for (int row = r + 1; row < con.rows - r; ++row)
	{
		for (int col = r + 1; col < con.cols - r; ++col)
		{
			double sim = std::max<double>(
				pointSimilarity(mean.at<cv::Vec3b>(row, col), mean.at<cv::Vec3b>(row - 1, col)), 
				pointSimilarity(mean.at<cv::Vec3b>(row, col), mean.at<cv::Vec3b>(row, col - 1))
			);
			con.at<double>(row, col) = sim;
			cons.push_back(sim);
		}
	}
	std::sort(cons.begin(), cons.end());
	double line = cons[int(cons.size() * 0.8)];
	for (int row = r + 1; row < con.rows - r; ++row)
	{
		for (int col = r + 1; col < con.cols - r; ++col)
		{
			double& tmp = con.at<double>(row, col);
			tmp = tmp >= line ? 1 : 0;
		}
	}
	cv::Mat con2 = con.clone();
	cv::dilate(con2, con, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	std::vector<cv::Vec3d> blockSum;
	std::vector<int> blockCount;
	blockMap = cv::Mat::ones(con.size(), CV_32SC1) * -1;
	static int dirMap[4][2] = {
		{0, 1},
		{1, 0},
		{0, -1},
		{-1, 0}
	};
	std::vector<cv::Vec2i> bfs;
	for (int row = r + 1; row < con.rows - r; ++row)
	{
		for (int col = r + 1; col < con.cols - r; ++col)
		{
			if (con.at<double>(row, col) < 1 && blockMap.at<int>(row, col) < 0)
			{
				bfs.clear();
				bfs.push_back(cv::Vec2i(row, col));
				int idx = blockSum.size();
				blockMap.at<int>(row, col) = idx;
				blockSum.push_back(cv::Vec3d(0, 0, 0));
				blockCount.push_back(0);
				int head = 0, tail = 1;
				while (head < tail)
				{
					const cv::Vec2i p = bfs[head];
					++blockCount[idx];
					auto& meanPixel = mean.at<cv::Vec3b>(p[0], p[1]);
					for (int i = 0; i < 3; ++i)
					{
						blockSum[idx][i] += meanPixel[i];
					}
					for (int i = 0; i < 4; ++i)
					{
						cv::Vec2i tp(p[0] + dirMap[i][0], p[1] + dirMap[i][1]);
						if (tp[0] >= r + 1 && tp[0] < con.rows - r && tp[1] >= r + 1 && tp[1] < con.cols - r)
						{
							if (con.at<double>(tp[0], tp[1]) < 1 && blockMap.at<int>(tp[0], tp[1]) < 0)
							{
								blockMap.at<int>(tp[0], tp[1]) = idx;
								bfs.push_back(tp);
								++tail;
							}
						}
					}
					++head;
				}
			}
		}
	}
	blocks.clear();
	for (int i = 0; i < blockSum.size(); ++i)
	{
		int count = blockCount[i];
		auto& sum = blockSum[i];
		cv::Vec3b inst;
		for (int c = 0; c < 3; ++c)
		{
			inst[c] = sum[c] / count;
		}
		blocks.push_back(inst);
	}
}

cv::Vec2s LightDetect::getLightDirection()
{
	static cv::Vec2s dirs[8] = {
		cv::Vec2s(1, 0),
		cv::Vec2s(1, 1),
		cv::Vec2s(0, 1),
		cv::Vec2s(-1, 1),
		cv::Vec2s(-1, 0),
		cv::Vec2s(-1, -1),
		cv::Vec2s(0, -1),
		cv::Vec2s(1, -1)
	};
	static double ratios[8] = {
		1,
		sqrt(2),
		1,
		sqrt(2),
		1,
		sqrt(2),
		1,
		sqrt(2)
	};
	double hits[8] = {0};
	int r = patchRadius;
	for (int row = r + 1; row < con.rows - r; ++row)
	{
		for (int col = r + 1; col < con.cols - r; ++col)
		{
			if (con.at<double>(row, col) >= 1)
			{
				for (int i = 0; i < 8; ++i)
				{
					int h = 1;
					while (true)
					{
						cv::Vec2i p(row + dirs[i][0] * h, col + dirs[i][1] * h);
						if (p[0] > r && p[0] < con.rows - r && p[1] > r && p[1] < con.cols - r && con.at<double>(p[0], p[1]) < 1)
						{
							const cv::Vec3b& blockAvg = blocks[blockMap.at<int>(p[0], p[1])];
							const cv::Vec3b& pixel = mean.at<cv::Vec3b>(p[0], p[1]);
							if (pixel[0] <= blockAvg[0] && pixel[1] <= blockAvg[1] && pixel[2] <= blockAvg[2] && pointSimilarity(pixel, blockAvg) < 0.01)
							{
								++h;
							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					--h;
					hits[i] += h * ratios[i];
				}
			}
		}
	}
	int m = 0;
	for (int i = 1; i < 8; ++i)
	{
		if (hits[i] > hits[m])
		{
			m = i;
		}
	}
	//return dirs[m];
	cv::Vec2s result(-dirs[m][0], -dirs[m][1]);
	return result;
}
