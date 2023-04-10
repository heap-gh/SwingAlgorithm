#pragma once

#pragma warning(suppress: 4244)

#ifndef STOCK_H
#define STOCK_H

#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <Windows.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <stdio.h>
#include <tchar.h>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <iostream>
#include <cpr/cpr.h>

#include "previousday.h"
#include "aggregate.h"
#include "realtimedata.h"
#include "dailydata.h"


class STOCK {

public:

	std::string symbol = "";
    std::string apikey = "";
	std::string basePath;
	std::string realtimeFile;
	std::string streamFile;
	std::string min15AggFile;
	std::string min30AggFile;
	std::string hour1AggFile;
	std::string hour2AggFile;
	std::string hour4AggFile;
	std::string dailyDataFile;
	std::string portfolioPath;

public:
	bool marketOpen;
	long long int averageDailyVolume = 0;
	previousDay previousDayData;
	std::vector <realtimedata> stockData;
	std::vector <dailydata> dailyData;
	std::vector <aggregate> min15;
	std::vector <aggregate> min30;
	std::vector <aggregate> hour1;
	std::vector <aggregate> hour2;
	std::vector <aggregate> hour4;

public:
	STOCK();
	void main();

	// init
	bool initialized = false;
	bool firstInit = false;

	void init();
	void initRealTime();
	void init15minAgg();
	void init30minAgg();
	void init1hourAgg();
	void init2hourAgg();
	void init4hourAgg();
	void initDailyAverageVolume();
	void sortDailyData();
	std::string getTimeStamp();
	std::string getTimeStamp(tm& time);

	// stream
	void stream();
	void checkBasePath();
	void appendData();
	void clearStockStreamFile(bool showMessage);
	tm getCurrentTMtime();
	void streamData(tm& time);
	int getCurrentIntTime(tm currentTime);
	uint64_t timeSinceEpochMillisec();
	void initDailyDataSince();
	void makeGetRequestAndPushBack(std::string url, std::ofstream& dataFile);
	void appendDailyData(tm& checkTime);
	std::string getCurrentDayString();
	void updateIndicators(tm& time);
	previousDay getPreviousDayStruct();
	bool checkMarketStatus();

	void showStatus();

	// indicators
	bool getTodaysChangePerc(double& priceGain);
	bool getTodaysChange(double& priceGain);
	int updateDailyAverageVolume();
	bool getSimpleMovingAverage(int interval, int ticks, double& mavg, int startingPos); // starting pos = size() -1 - startingPos
	bool getSimpleMovingAverage(std::string timespan, std::string window, double& SMA);
	bool getExponentialMovingAverage(std::string timespan, std::string window, double& SMA);
	bool getAccumulatedVolume(long long int& volume);

	// transaction
	
	//void buy(int amount, double buyIn, std::string strategie);
	//void sell(int amount, double buyOut, std::string strategie);


	enum intervals {

		REALTIME = 1,
		MIN15 = 2,
		MIN30 = 3,
		HOUR1 = 4,
		HOUR2 = 5,
		HOUR4 = 6,
		DAILY = 7

	};

};



#endif // !STOCK_H
