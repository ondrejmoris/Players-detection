#include "../Include/Application.h"

Application::Application()
{
	this->failOfApp = false;
	this->basePath = "";
	this->initTime = 0;
	this->updateTime = 0;
	this->allTime = 0;
	this->detectTime = 0;
	this->tracker = 0;
	this->whereTrack = 0;
	this->threshold = 0;
	this->minBoxArea = 0;
}

bool Application::loadBackground(std::string& path)
{
	this->background = cv::imread(path);
	if (this->background.empty())
		return false;
	return true;
}

bool Application::loadVideo(std::string& path)
{
	this->cap.open(path);
	if (!cap.isOpened())
		return false;
	return true;
}

void Application::setCrop(cv::Point first, cv::Point second)
{
	// Dodìlat ovìøení! Parametry musí být stejné u videa i pozadí
	this->first = first;
	this->second = second;
}

void Application::setWhereTrack(int whereTrack)
{
	this->whereTrack = whereTrack;
}

void Application::setErodeSize(int erodeSize)
{
	this->erodeSize = erodeSize;
}

void Application::setDialteSize(int dialteSize)
{
	this->dilateSize = dialteSize;
}

void Application::setTreshold(int threshold)
{
	this->threshold = threshold;
}

void Application::setTracker(int tracker)
{
	this->tracker = tracker;
}

void Application::setMinBoxArea(int minBoxArea)
{
	this->minBoxArea = minBoxArea;
}

void Application::setFramesToWrite(int framesToWirte)
{
	this->framesToWrite = framesToWirte;
}

cv::Mat Application::getPreview()
{
	// container of the tracked objects
	std::vector<cv::Ptr<cv::Tracker>> trackersList;

	// container of the tracked objects
	std::vector<cv::Rect2d> objects;

	// Container of the players positions
	std::vector<std::vector<cv::Vec3i>> playersPos;

	std::vector<cv::Rect> playersBox;
	cv::Mat circleImg;
	cv::Mat imgAfterMor;
	std::vector<cv::Rect> circlesBox;

	cv::Mat imgOriginal = this->readFrame();

	this->crop(imgOriginal);
	std::cout << "Crooping done" << std::endl;

	imgAfterMor = this->removeBackground(imgOriginal);
	std::cout << "Remove background done" << std::endl;
	this->findPlayers(imgAfterMor, playersBox, circleImg, circlesBox);
	std::cout << "Find players done" << std::endl;

	bool trackRes;
	int cntOfFrames = 0;
	this->initTrackers(trackersList, playersBox, circlesBox, objects, imgOriginal, imgAfterMor, circleImg);
	std::cout << "Init tracker done" << std::endl;
	drawRects(imgOriginal, objects, cv::Scalar(255, 0, 0));
	//this->updateTrackers(trackersList, objects, imgOriginal, imgAfterMor, circleImg, trackRes, playersPos, cntOfFrames);
	//std::cout << "Update tracker done" << std::endl;

	// Nastavení ètení na zaèátek videa
	cap.set(cv::CAP_PROP_POS_FRAMES, 0);

	return imgOriginal;
}

int Application::run()
{
	// container of the tracked objects
	std::vector<cv::Ptr<cv::Tracker>> trackersList;

	// container of the tracked objects
	std::vector<cv::Rect2d> objects;

	// Container of the players positions
	std::vector<std::vector<cv::Vec3i>> playersPos;

	bool firstRun = true;
	int cntOfFrames = 0;

	this->createWindows();

	while (true) {
		std::vector<cv::Rect> playersBox;
		cv::Mat circleImg;
		cv::Mat imgAfterMor;
		std::vector<cv::Rect> circlesBox;

		bool trackRes = true;

		auto startTimeOfLoop = std::chrono::steady_clock::now();

		cv::Mat imgOriginal = this->readFrame();

		if (this->failOfApp) {
			break;
		}

		this->crop(imgOriginal);

		if (this->whereTrack > 0 || firstRun) {
			// Start timer for detection part
			auto startTime = std::chrono::steady_clock::now();

			imgAfterMor = this->removeBackground(imgOriginal);
			this->findPlayers(imgAfterMor, playersBox, circleImg, circlesBox);

			// End timer for detection part
			auto endTime = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
			this->detectTime += elapsed.count();

			if (firstRun) {
				startTime = std::chrono::steady_clock::now();
				this->initTrackers(trackersList, playersBox, circlesBox, objects, imgOriginal, imgAfterMor, circleImg);
				endTime = std::chrono::steady_clock::now();
				elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
				this->initTime += elapsed.count();
			}
			firstRun = false;
		}

		auto startUpdateTime = std::chrono::steady_clock::now();

		this->updateTrackers(trackersList, objects, imgOriginal, imgAfterMor, circleImg, trackRes, playersPos, cntOfFrames);

		auto endUpdateTime = std::chrono::steady_clock::now();
		auto elapsedUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(endUpdateTime - startUpdateTime);
		this->updateTime += elapsedUpdateTime.count();

		this->showImgs(imgOriginal, imgAfterMor, circleImg);

		auto endTimeOfLoop = std::chrono::steady_clock::now();
		auto elapsedTimeOfLoop = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeOfLoop - startTimeOfLoop);
		this->allTime += elapsedTimeOfLoop.count();

		// wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		if (cv::waitKey(30) == 27)
			break;

		cntOfFrames++;
	}

	clear();

	if (cntOfFrames > this->framesToWrite || cntOfFrames == this->framesToWrite) {
		this->writeTrajectories(playersPos);
		this->writeTimes();
	}
	
	if (this->failOfApp)
		return -1;
	return 0;
}

void Application::createWindows()
{
	if (this->whereTrack > 0) {
		cv::namedWindow("Maska", cv::WINDOW_NORMAL);
		cv::resizeWindow("Maska", 720, 360);

		//cv::namedWindow("Detekce", cv::WINDOW_NORMAL);
		//cv::resizeWindow("Detekce", 720, 360);

		cv::namedWindow("Kruhy", cv::WINDOW_NORMAL);
		cv::resizeWindow("Kruhy", 720, 360);
	}

	cv::namedWindow("Original", cv::WINDOW_NORMAL);
	cv::resizeWindow("Original", 720, 360);
}

void Application::createStructingElements()
{
	this->elementForDilate = cv::getStructuringElement(cv::MORPH_RECT, 
		cv::Size(2 * this->dilateSize + 1, 2 * this->dilateSize + 1), cv::Point(-1, -1));

	this->elementForErode = cv::getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * this->erodeSize + 1, 2 * this->erodeSize + 1), cv::Point(-1, -1));
}

void Application::crop(cv::Mat& img)
{
	img = img(cv::Rect(this->first, this->second));
}

cv::Mat Application::readFrame()
{
	cv::Mat img;
	bool bSuccess = cap.read(img);
	if (!bSuccess)
	{
		this->failOfApp = true;
	}
	return img;
}

void Application::clear()
{
	this->cap.release();
	cv::destroyAllWindows();
}

void Application::findPlayers(cv::Mat& img, std::vector<cv::Rect>& playersBox, cv::Mat& circleImg, std::vector<cv::Rect>& circlesBox)
{
	std::vector<std::vector<cv::Point> > contours;

	// Find contures
	cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	// Bounding contures
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::Rect bBox;
		bBox = cv::boundingRect(contours[i]);

		// Minmal boudnig area
		if (bBox.area() >= this->minBoxArea)
		{
			playersBox.push_back(bBox);
		}
	}

	if (this->whereTrack == 2) {
		circleImg = cv::Mat::zeros(img.rows, img.cols, img.type());
		this->makeImgWithCircles(circleImg, playersBox, circlesBox);
	}
}

cv::Mat Application::removeBackground(cv::Mat& foregroundImg)
{
	cv::Mat tmp;

	// Calc diff 
	cv::absdiff(foregroundImg, this->background, tmp);

	// Convert to grayscale
	cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);

	// Threshold converted img
	cv::threshold(tmp, tmp, this->threshold, 255, cv::THRESH_BINARY);

	this->createStructingElements();
	// Erode and dilate
	cv::erode(tmp, tmp, this->elementForErode, cv::Point(-1, -1), 1);
	cv::dilate(tmp, tmp, this->elementForDilate, cv::Point(-1, -1), 1);

	return tmp;
}

void Application::makeImgWithCircles(cv::Mat& img, std::vector<cv::Rect>& playersBox, std::vector<cv::Rect>& circlesBox)
{
	for (size_t i = 0; i < playersBox.size(); i++)
	{

		cv::Point center;
		center.x = playersBox[i].x + playersBox[i].width / 2;
		center.y = playersBox[i].y + playersBox[i].height / 2;

		// Printing circle in image with circle
		cv::circle(img, center, 20, CV_RGB(255, 255, 255), -1);

		// Adding circle to vecotr.
		// Box it is small and move si so fast... Then make them bigger and correction when is out of range.
		circlesBox.push_back(cv::Rect((center.x - 40) < 0 ? 0 : (center.x - 40), (center.y - 40) < 0 ? 0 : (center.y - 40), 80, 80));
	}
}

void Application::initTrackers(std::vector<cv::Ptr<cv::Tracker>>& trackersList, std::vector<cv::Rect>& playersBox, 
	std::vector<cv::Rect>& circlesBox, std::vector<cv::Rect2d>& objects, cv::Mat& imgOriginal, 
	cv::Mat& imgAfterMor, cv::Mat& circleImg)
{
	// Tracker init
	for (size_t i = 0; i < playersBox.size(); i++)
	{
		// Selecting tracker
		if (this->tracker == 0)
			trackersList.push_back(cv::TrackerMOSSE::create());
		if (this->tracker == 1 && (this->whereTrack == 1 || this->whereTrack == 2))
			trackersList.push_back(cv::TrackerKCF::create(this->getKcfParams()));
		if (this->tracker == 1 && whereTrack == 0)
			trackersList.push_back(cv::TrackerKCF::create());
		if (this->tracker == 2)
			trackersList.push_back(cv::TrackerTLD::create());
		if (this->tracker == 3)
			trackersList.push_back(cv::TrackerMedianFlow::create());
		if (this->tracker == 4)
			trackersList.push_back(cv::TrackerMIL::create());
		if (this->tracker == 5)
			trackersList.push_back(cv::TrackerCSRT::create());

		// Value for scale bounding box
		int val = (imgOriginal.cols <= 3840 ? (20 / (3840 / imgOriginal.cols)) : ((imgOriginal.cols / 3840) * 20));

		// Start init in right image
		if (whereTrack == 0) {
			objects.push_back(cv::Rect2d((playersBox[i].x - val) < 0 ? 0 : (playersBox[i].x - val), 
				(playersBox[i].y - val) < 0 ? 0 : (playersBox[i].y - val), 
				playersBox[i].width + (val * 2), playersBox[i].height + (val * 2)));
			trackersList[i]->init(imgOriginal, objects[i]);
		}
		else if (whereTrack == 1) {
			objects.push_back(cv::Rect2d((playersBox[i].x - val) < 0 ? 0 : (playersBox[i].x - val), 
				(playersBox[i].y - val) < 0 ? 0 : (playersBox[i].y - val), 
				playersBox[i].width + (val * 2), playersBox[i].height + (val * 2)));
			trackersList[i]->init(imgAfterMor, objects[i]);
		}
		else {
			objects.push_back(circlesBox[i]);
			trackersList[i]->init(circleImg, objects[i]);
		}
	}
}

cv::TrackerKCF::Params Application::getKcfParams()
{
	// Setting of kcf
	cv::TrackerKCF::Params param;
	param.desc_pca = cv::TrackerKCF::GRAY;
	param.compressed_size = 1;
	return param;
}

void Application::updateTrackers(std::vector<cv::Ptr<cv::Tracker>>& trackersList, std::vector<cv::Rect2d>& objects, cv::Mat& imgOriginal,
	cv::Mat& imgAfterMor, cv::Mat& circleImg, bool& trackRes, std::vector<std::vector<cv::Vec3i>>& playersPos, int& cntOfFrames)
{
	// Update the tracking result
	for (int i = 0; i < trackersList.size(); i++) {
		if (this->whereTrack == 0)
			trackRes = trackersList[i]->update(imgOriginal, objects[i]);
		if (this->whereTrack == 1)
			trackRes = trackersList[i]->update(imgAfterMor, objects[i]);
		if (this->whereTrack == 2)
			trackRes = trackersList[i]->update(circleImg, objects[i]);

		// Saving players position
		if (cntOfFrames < this->framesToWrite) {
			playersPos[i].push_back(cv::Vec3i((int)(objects[i].x + objects[i].width / 2), 
				(int)(objects[i].y + objects[i].height / 2), trackRes));
		}

		// Printig rect about player
		if (!trackRes) {
			//rectangle(imgOriginal, objects[i], cv::Scalar(0, 0, 255), 2, 1);
			drawRect(imgOriginal, objects[i], cv::Scalar(0, 0, 255));
		}
		else
		{
			drawRect(imgOriginal, objects[i], cv::Scalar(255, 0, 0));
		}
	}
}

void Application::drawRects(cv::Mat& img, std::vector<cv::Rect2d>& objects, cv::Scalar color) {
	for (int i = 0; i < objects.size(); i++) {
		drawRect(img, objects[i], color);
	}
}

void Application::drawRect(cv::Mat& img, cv::Rect2d& object, cv::Scalar color) {
		rectangle(img, object, color, 2, 1);
}

void Application::showImgs(cv::Mat& imgOriginal, cv::Mat& imgAfterMor, cv::Mat& circleImg)
{
	// Show right windows
	if (this->whereTrack > 0) {
		cv::imshow("Maska", imgAfterMor);
		cv::imshow("Kruhy", circleImg);
		//cv::imshow(winName, mask);
	}
	imshow("Original", imgOriginal);
}

void Application::writeTrajectories(std::vector<std::vector<cv::Vec3i>>& playersPos)
{
	std::string outPutFile;

	outPutFile.append(this->basePath + "trajektorie" + std::to_string(this->tracker) + std::to_string(this->whereTrack) + ".txt");

	std::ofstream myFile(outPutFile);
	if (myFile.is_open()) {
		for (int i = 0; i < playersPos.size(); i++) {
			for (int a = 0; a < playersPos[i].size(); a++) {
				myFile << "[" << playersPos[i][a][0] << "," << playersPos[i][a][1] << "," << playersPos[i][a][2] << "]" << ";";
			}
			if ((i + 1) < playersPos.size())
				myFile << "\n";
		}
	}
	myFile.close();
}

void Application::writeTimes()
{
	std::string outPutTimeFile;

	outPutTimeFile.append(this->basePath + "casy" + std::to_string(this->tracker) + std::to_string(this->whereTrack) + ".txt");

	std::ofstream myFile1(outPutTimeFile);
	if (myFile1.is_open()) {
		myFile1 << "Doba initializace trackeru: " << this->initTime << "ms" << "\n";
		myFile1 << "Prumerny cas aktualizace po " << this->framesToWrite << " snimcich je: " << this->updateTime / this->framesToWrite << "ms" << "\n";
		if (whereTrack > 0)
			myFile1 << "Prumerna doba detekce po" << this->framesToWrite << " snimcich je:" << this->detectTime / this->framesToWrite << "ms" << "\n";
		myFile1 << "Prumerny cas jednoho snimku po " << this->framesToWrite << " snimcich je: " << this->allTime / this->framesToWrite << "ms" << "\n";
	}
	myFile1.close();
}

