#pragma once

#ifndef MOVINGAVERAGESTRATEGY_H
#define MOVINGAVERAGESTRATEGY_H

#include "stock.h"

// constructor ---> main


class SWINGTRADE {

public:
	STOCK* stock;
	SWINGTRADE(STOCK* tesla);

private: // variables
	std::string strategyPath = "";
	std::string signalsFile = "";
	std::string swingTradeDataFile = "";

private: // data
	int currentTradeID = 0;
	double buyIn = -1;
	double buyOut = -1;
	dailydata entryPoint;

private: // Data for the buy in algo "C:/Users/USER/Desktop/TradingBot/portfolio/STOCK/swingtrade/data.txt"
	std::vector <double> movingAverages;
	double marketTrend5days = -1; // 0 = false, 1 = true, -1 not initialized
	double marketTrend4days = -1;
	double marketTrend3days = -1;
	double marketTrend2days = -1;
	double marketTrend1day = -1;
	int highestSMA5days = -1; // last high in the last 5 days, -1 uninitialized
	

	int buyInType = -1; // what type of pattern is this

private: // methods
	void main();
	void init();
	void getBuyIn(std::string& line);
	void getID(std::vector <std::string>& data);
	bool searchEntryPoint(double& MAVG, realtimedata& lastData);
	bool searchExitPoint(double& MAVG, realtimedata& lastData);
	void sellSignal(double buyIn, double buyOut, double MAVG);
	void buySignal(double buyIn, double MAVG);
	void addBuyInMySQL(double buyIn);
	void addBuyOutMySQL(double buyOut);
	bool analyzeForBuyIn(realtimedata& lastData, double& MAVG);
	bool analyzeForBuyOut(realtimedata& lastData, double& MAVG);
	void checkMarketTrend();
	void updateData();
	void getBuyInType(std::string& line);

};


#endif // !MOVINGAVERAGESTRATEGY_H

