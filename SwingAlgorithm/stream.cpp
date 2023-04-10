// append the new stockdata
#pragma warning( push )
#pragma warning( disable : 4996)


#include "stock.h"



void STOCK::stream() {

	while (true) {

		tm time = getCurrentTMtime();

		streamData(time);

		Sleep(50);
	}


}



void STOCK::showStatus() {

	if (!this->min15.empty()) {
		
		std::cout << "\n-----------------------------------------------------------------------\nMIN15 AGGREGATES \n";

		for (aggregate& agg : this->min15) {
			std::cout << agg.open << "," << agg.close << "," << agg.high << "," << agg.low << "\n";
		}
		std::cout << "\n";
	}

	if (!this->min30.empty()) {

		std::cout << "MIN30 AGGREGATES \n";

		for (aggregate& agg : this->min30) {
			std::cout << agg.open << "," << agg.close << "," << agg.high << "," << agg.low << "\n";
		}
		std::cout << "\n";
	}


	if (!this->hour1.empty()) {

		std::cout << "1 HOUR AGGREGATES \n";

		for (aggregate& agg : this->hour1) {
			std::cout << agg.open << "," << agg.close << "," << agg.high << "," << agg.low << "\n";
		}
		std::cout << "\n";
	}

	if (!this->hour2.empty()) {

		std::cout << "2 HOUR AGGREGATES \n";

		for (aggregate& agg : this->hour2) {
			std::cout << agg.open << "," << agg.close << "," << agg.high << "," << agg.low << "\n";
		}
		std::cout << "\n";
	}


	if (!this->hour4.empty()) {

		std::cout << "4 HOUR AGGREGATES \n";

		for (aggregate& agg : this->hour4) {
			std::cout << agg.open << "," << agg.close << "," << agg.high << "," << agg.low << "\n";
		}
		std::cout << "\n";
	}

	std::cout << "DAILY DATA SIZE" << this->dailyData.size() << "\n\n";

	std::cout << "LAST REALTIMEDATA\n";
	std::cout << this->stockData.at(this->stockData.size() - 1).open << "," << this->stockData.at(this->stockData.size() - 1).close << ","
		<< this->stockData.at(this->stockData.size() - 1).high << "," << this->stockData.at(this->stockData.size() - 1).low << "\n\n";

	double priceGain = 0;
	std::cout << "TODAYS PRICE GAIN PERC\n";
	getTodaysChangePerc(priceGain);
	std::cout << priceGain << "\n\n";

	std::cout << "TODAYS PRICE GAIN\n";
	getTodaysChange(priceGain);
	std::cout << priceGain << "\n\n";

	long long int s = 0;
	std::cout << "TODAYS ACCUMULATED VOLUME\n";
	getAccumulatedVolume(s);
	std::cout << s << "\n\n";


	std::cout << "MARKET STATUS: " << checkMarketStatus() << "\n-----------------------------------------------------------------------\n";



}



bool STOCK::checkMarketStatus() {

	std::string url = "https://api.polygon.io/v1/marketstatus/now?apiKey=" + this->apikey;

	std::string response = cpr::Get(cpr::Url{ url }).text;
	size_t index = response.find("\"market\"");

	if (index != std::string::npos) {
		index += 10;
		std::string answer = "";

		while (response.at(index) != ',') {
			if (std::isalpha(response.at(index))) {
				answer += response.at(index);
			}
			index++;
		}

		if (answer == "open") {
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}



void STOCK::updateIndicators(tm& time) {

	if (time.tm_hour == 15 && time.tm_mon == 30 && time.tm_sec == 10) {
		updateDailyAverageVolume();
		Sleep(1500);
	}

}



void STOCK::appendDailyData(tm& checkTime) {

	if (checkTime.tm_hour == 6 && checkTime.tm_min == 30 && checkTime.tm_sec == 30) {
		this->previousDayData = getPreviousDayStruct();
		Sleep(2000);
	}
}



void STOCK::appendData() {
	

	std::ifstream streamfile(this->streamFile);

	if (streamfile.is_open()) {

		std::cout << getTimeStamp() << "[STREAM] Appending " + this->symbol +" Stock data--> ";

		std::string datalines = "";
		std::vector <std::string> data;

		while (getline(streamfile, datalines)) {
			data.push_back(datalines);
		}

		streamfile.close();

		for (std::string& dataline : data) {
			if (dataline.find("EquityAgg") != std::string::npos) {
				this->stockData.push_back(realtimedata::getRealTimeDataStruct(dataline));
			}
			if (dataline.find("(15min)") != std::string::npos) {
				this->min15.push_back(aggregate::getAggregateStruct(dataline));
			}
			if (dataline.find("(30min)") != std::string::npos) {
				this->min30.push_back(aggregate::getAggregateStruct(dataline));
			}
			if (dataline.find("(1hour)") != std::string::npos) {
				this->hour1.push_back(aggregate::getAggregateStruct(dataline));
			}
			if (dataline.find("(2hour)") != std::string::npos) {
				this->hour2.push_back(aggregate::getAggregateStruct(dataline));
			}
			if (dataline.find("(4hour)") != std::string::npos) {
				this->hour4.push_back(aggregate::getAggregateStruct(dataline));
			}
			if (dataline.find("{\"status\"") != std::string::npos) {
				this->dailyData.push_back(dailydata::getDailyDataStruct(dataline));
			}
		}

		std::cout << "Finished " << getTimeStamp() << "\n";
		
		clearStockStreamFile(false);

		Sleep(2000);
	}
	else {
		std::cout << "[-] Could not open stream file " << this->streamFile << "\n";
	}

}



tm STOCK::getCurrentTMtime() {

	time_t rawTime;
	struct tm* timeinfo;
	time(&rawTime);
	timeinfo = localtime(&rawTime);

	return *timeinfo;
}



void STOCK::streamData(tm& time) {

	int timer = getCurrentIntTime(time);

	if (time.tm_hour == 15 && time.tm_min == 36 && time.tm_sec == 30) {
		this->marketOpen = checkMarketStatus();
		std::cout << getTimeStamp() << "[MARKET] MARKET CLOSED. NO ANALYSIS ON " << time.tm_mday << "." << time.tm_mon + 1 << "." << time.tm_year << "\n";
		Sleep(2000);
	}

	if ((((timer > 1544) && (timer < 2216)) && time.tm_sec == 40) && this->marketOpen) {
		appendData();
		//showStatus();
	}

	appendDailyData(time);
	updateIndicators(time);

}



int STOCK::getCurrentIntTime(tm currentTime) {

	if (currentTime.tm_min < 10) {
		return std::stoi(std::to_string(currentTime.tm_hour) + "0" + std::to_string(currentTime.tm_min));
	}
	else {
		return std::stoi(std::to_string(currentTime.tm_hour) + std::to_string(currentTime.tm_min));
	}
}



void STOCK::clearStockStreamFile(bool showMessage) {

	std::ofstream ofs;
	ofs.open(this->streamFile, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	if (showMessage) {
		std::cout << "[CLEAR] Data from stream file cleared \n";
	}
}



uint64_t STOCK::timeSinceEpochMillisec() {
	using namespace std::chrono;
	auto s = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	auto newString = std::to_string(s);

	for (int x = 0; x < 4; x++) {
		newString[newString.length() - 1 - x] = 0;
	}

	return std::stoll(newString);
}



previousDay STOCK::getPreviousDayStruct() {
	
	return previousDay::getPreviousDayData("https://api.polygon.io/v2/aggs/ticker/" + this->symbol + "/prev?adjusted=true&apiKey=" + this->apikey);
}



#pragma warning( pop )


