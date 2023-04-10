
#pragma warning( push )
#pragma warning( disable : 4267)
#pragma warning( disable : 4715)

#include "stock.h"


bool STOCK::getTodaysChangePerc(double& priceGain) {

	std::string queryString = "https://api.polygon.io/v2/snapshot/locale/us/markets/stocks/tickers/" + this->symbol + "?apiKey=" + this->apikey;
	std::string response = cpr::Get(cpr::Url{ queryString }).text;

	size_t index = response.find("\"todaysChangePerc\"");
	std::string value = "";

	if (index != std::string::npos) {
		while (response.at(index) != ',') {
			if (std::isdigit(response.at(index)) || response.at(index) == '.') {
				value += response.at(index);
			}
			index++;
		}

		priceGain = std::stod(value);
		return true;
	}

	return false;
}



bool STOCK::getTodaysChange(double& priceGain) {

	std::string queryString = "https://api.polygon.io/v2/snapshot/locale/us/markets/stocks/tickers/" + this->symbol + "?apiKey=" + this->apikey;
	std::string response = cpr::Get(cpr::Url{ queryString }).text;

	size_t index = response.find("\"todaysChange\"");
	std::string value = "";

	if (index != std::string::npos) {
		while (response.at(index) != ',') {
			if (std::isdigit(response.at(index)) || response.at(index) == '.') {
				value += response.at(index);
			}
			index++;
		}

		priceGain = std::stod(value);
		return true;
	}

	return false;
}



int STOCK::updateDailyAverageVolume() {

	long long int avgvol = 0;

		for (dailydata& ddata : this->dailyData) {
			avgvol += ddata.volume;
		}

		return avgvol / this->dailyData.size();
}



bool STOCK::getSimpleMovingAverage(std::string timespan, std::string window, double& SMA) {

	std::string queryString = "https://api.polygon.io/v1/indicators/sma/" + this->symbol + "?timestamp=" + getCurrentDayString() + "&timespan=" + timespan + "&adjusted=true&window=" + window + "&series_type=close&order=asc&limit=5000&apiKey=" + this->apikey;
	std::string response = cpr::Get(cpr::Url{ queryString }).text;

	int index = response.find("\"value\"");
	std::string value = "";

	if (index != std::string::npos) {
		while (response.at(index) != '}') {
			if (std::isdigit(response.at(index)) || response.at(index) == '.') {
				value += response.at(index);
			}
			index++;
		}
		SMA = std::stod(value);
		return true;
	}
	
	return false;
}



bool STOCK::getExponentialMovingAverage(std::string timespan, std::string window, double& SMA) {

	std::string queryString = "https://api.polygon.io/v1/indicators/ema/" + this->symbol + "?timestamp=" + getCurrentDayString() + "&timespan=" + timespan + "&adjusted=true&window=" + window + "&series_type=close&order=asc&limit=5000&apiKey=" + this->apikey;
	std::string response = cpr::Get(cpr::Url{ queryString }).text;

	int index = response.find("\"value\"");
	std::string value = "";

	if (index != std::string::npos) {
		while (response.at(index) != '}') {
			if (std::isdigit(response.at(index)) || response.at(index) == '.') {
				value += response.at(index);
			}
			index++;
		}
		SMA = std::stod(value);
		return true;
	}

	return false;
}

// TEST THIS
// 1 day = 	670 minutes
bool STOCK::getSimpleMovingAverage(int interval, int ticks, double& mavg, int startingPos) {

	mavg = 0;

	switch (interval) {

	case 1:
	{
		if (this->stockData.size() - startingPos >= ticks) {
			for (int index = this->stockData.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->stockData.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 2:
	{
		if (this->min15.size() - startingPos >= ticks) {
			for (int index = this->min15.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->min15.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 3:
	{
		if (this->min30.size() - startingPos >= ticks) {
			for (int index = this->min30.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->min30.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 4:
	{
		if (this->hour1.size() - startingPos >= ticks) {
			for (int index = this->hour1.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->hour1.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 5:
	{
		if (this->hour2.size() - startingPos >= ticks) {
			for (int index = this->hour2.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->hour2.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 6:
	{
		if (this->hour4.size() - startingPos >= ticks) {
			for (int index = this->hour4.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->hour4.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	case 7:
	{
		if (this->dailyData.size() - startingPos >= ticks) {
			for (int index = this->dailyData.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
				mavg += this->dailyData.at(index).close;
			}
			mavg /= ticks;
			return true;
		}
		return false;
	} break;


	}
	
}


// TEST THIS
bool STOCK::getAccumulatedVolume(long long int& volume) {

	if (!this->stockData.empty()) {
		
		volume = this->stockData.at(this->stockData.size() - 1).accumulated_volume;
		return true;
	}

	return false;
}

#pragma warning( pop )


