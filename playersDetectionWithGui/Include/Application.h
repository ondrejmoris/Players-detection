#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include <fstream>

// TODO: Ukládat èas do int není uplnì ideální... Pøi dlouhém videu a pomalém trackeru snadno pøeteèe.
// TODO: Gui - 1 stránka: pøidat video a obrazek pozadí... 2 stránka: editace parametru spoleènì s nìjakým prewiev a start.
// TODO: Gui - Na druhé stránce udìlat horní lištu s možnosti exportu/importu aktuálního nastavení
// TODO: Vyøešit kontrolu, jestli jsou stejné rozlišení

class Application
{
public:
	Application();

	bool loadBackground(std::string& path);
	bool loadVideo(std::string& path);

	void setCrop(cv::Point first, cv::Point second);
	void setWhereTrack(int whereTrack);
	void setErodeSize(int erodeSize);
	void setDialteSize(int dialteSize);
	void setTreshold(int threshold);
	void setTracker(int tracker);
	void setMinBoxArea(int minBoxArea);
	void setFramesToWrite(int framesToWirte);

	cv::Mat getPreview();

	int run();

private:
	cv::Mat background;
	// 0 - origo obraz, 1 - po erozi a dilataci, 2 - kruhy
	int whereTrack;
	cv::Point first, second;
	int erodeSize, dilateSize;
	int threshold;
	// 0 - MOSSE, 1 - KCF, 2 - TLD, 3 - MEDIANFLOW, 4 - MIL, 5 - CSRT
	int tracker;
	int minBoxArea;
	int framesToWrite;
	cv::VideoCapture cap;
	cv::Mat elementForDilate;
	cv::Mat elementForErode;
	bool failOfApp;
	std::string basePath;
	int updateTime;
	int detectTime;
	int initTime;
	int allTime;

	void createWindows();
	void createStructingElements();
	// xmin - 0, ymin - 750, xmax - 3840, ymax - 1780
	void crop(cv::Mat &img);
	cv::Mat readFrame();
	void clear();
	void findPlayers(cv::Mat& img, std::vector<cv::Rect>& playersBox, cv::Mat& circleImg, std::vector<cv::Rect>& circlesBox);
	cv::Mat removeBackground(cv::Mat& foregroundImg);
	void makeImgWithCircles(cv::Mat& img, std::vector<cv::Rect>& playersBox, std::vector<cv::Rect>& circlesBox);
	void initTrackers(std::vector<cv::Ptr<cv::Tracker>>& trackersList, std::vector<cv::Rect>& playersBox, 
		std::vector<cv::Rect>& circlesBox, std::vector<cv::Rect2d>& objects, cv::Mat& imgOriginal,
		cv::Mat& imgAfterMor, cv::Mat& circleImg);
	cv::TrackerKCF::Params getKcfParams();
	void updateTrackers(std::vector<cv::Ptr<cv::Tracker>>& trackersList, std::vector<cv::Rect2d>& objects, cv::Mat& imgOriginal,
		cv::Mat& imgAfterMor, cv::Mat& circleImg, bool& trackRes, std::vector<std::vector<cv::Vec3i>>& playersPos, int& cntOfFrames);
	void showImgs(cv::Mat& imgOriginal, cv::Mat& imgAfterMor, cv::Mat& circleImg);
	void writeTrajectories(std::vector<std::vector<cv::Vec3i>>& playersPos);
	void writeTimes();
	void drawRects(cv::Mat& img, std::vector<cv::Rect2d>& objects, cv::Scalar color);
	void drawRect(cv::Mat& img, cv::Rect2d& object, cv::Scalar color);
};

