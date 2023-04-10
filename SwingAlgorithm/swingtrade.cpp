
#pragma warning( push )
#pragma warning( disable : 4244)

#include "swingtrade.h"
#include "mysqlconnection.h"

#pragma warning( pop )

SWINGTRADE::SWINGTRADE(STOCK* stock) {

	this->stock = stock;

	init();

	std::thread main(&SWINGTRADE::main, this);
	main.detach();

}


void SWINGTRADE::init() {

	char acUserName[100];
	DWORD nUserName = sizeof(acUserName);
	GetUserNameA(acUserName, &nUserName);

	using namespace boost::filesystem;

	this->strategyPath = this->stock->portfolioPath + "swingtrade/";
	this->swingTradeDataFile = "C:/Users/"+ std::string(acUserName) + "/Desktop/TradingBot/portfolio/" + this->stock->symbol + "/swingtrade/data.txt";

	if (!exists(this->stock->portfolioPath)) {
		_mkdir(this->stock->portfolioPath.c_str());
	}

	if (!exists(this->strategyPath)) {
		_mkdir(this->strategyPath.c_str());
	}


	this->signalsFile = strategyPath + "signals.txt";

	if (!boost::filesystem::exists(this->signalsFile)) {
		std::ofstream outfile(this->signalsFile);
	}

	if (!boost::filesystem::exists(this->swingTradeDataFile)) {
		std::ofstream outfile(this->swingTradeDataFile);
	}

	std::ifstream signals(this->signalsFile);


	if (signals.is_open()) {

		std::vector <std::string> data;
		std::string line = "";

		while (getline(signals, line)) {
			if (!line.empty()) {
				data.push_back(line);
			}
		}

		if (!data.empty()) {
			getID(data);
		}

		signals.close();
	}

	//checkMarketTrend();

}



void SWINGTRADE::getID(std::vector<std::string>& data) {

	std::string line = "";
	std::string value = "";
	int index = 0;

	if (data.at(data.size() - 1).find("BUY") != std::string::npos) {
		line = data.at(data.size() - 1);

		while (isdigit(line[index])) {
			value += line[index];
			index++;
		}

		getBuyIn(line);
		getBuyInType(line);

		this->currentTradeID = std::stoi(value);

	}
	else if (data.at(data.size() - 2).find("BUY") != std::string::npos) {
		line = data.at(data.size() - 2);

		while (isdigit(line[index])) {
			value += line[index];
			index++;
		}

		this->currentTradeID = std::stoi(value) + 1;

	}
	else {
		std::cout << this->stock->getTimeStamp() << "[FUNC-GETID ERROR]\n";
	}

}



void SWINGTRADE::getBuyInType(std::string& line) {

	std::string value = "";
	size_t index = line.find("TYPE");

	if (index != std::string::npos) {

		for (index; line[index] != ')'; index++) {

			if (isdigit(line[index])) {
				value += line[index];
			}

		}

		this->buyInType = std::stoi(value);

	}

}



void SWINGTRADE::getBuyIn(std::string& line) {

	if (line.find("BUY") != std::string::npos) {
		// BUY [123.43,
		int index = 0;
		std::string value = "";

		while (line[index] != ',') {
			if (isdigit(line[index]) || line[index] == '.') {
				value += line[index];
			}
			index++;
		}

		this->buyIn = std::stod(value);


	}

}



void SWINGTRADE::main() {


	while (true) {
		double MAVG = 0;
		realtimedata lastPrice;
		bool isEntry = false;
		bool isExit = false;


		tm time = this->stock->getCurrentTMtime();

		if (!isEntry && this->buyIn == -1) {
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE] Searching for entry point ..." << " (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";

			while (!isEntry) {
				tm time = this->stock->getCurrentTMtime();
				int timer = this->stock->getCurrentIntTime(time);
				if (((((timer > 1544) && (timer < 2216)) && time.tm_sec == 50) && this->stock->marketOpen)) {
					isEntry = searchEntryPoint(MAVG, lastPrice);
				}
			}

		}

		std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE] Searching for exit point ..." << " (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";
		while (!isExit) {
			tm time = this->stock->getCurrentTMtime();
			int timer = this->stock->getCurrentIntTime(time);
			if (((((timer > 1544) && (timer < 2216)) && time.tm_sec == 50) && this->stock->marketOpen)) {
				isExit = searchExitPoint(MAVG, lastPrice);
			}
		}

		Sleep(100);

	}

}



bool SWINGTRADE::searchExitPoint(double& MAVG, realtimedata& lastData) {


	while (true) {

		// get the data and check
		try {

			lastData = this->stock->stockData.at(this->stock->stockData.size() - 1);

			if (this->stock->getSimpleMovingAverage(STOCK::MIN15, 50, MAVG, 0) == false) {
				std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][EXIT] Need more Data for SMA (" << 50 - this->stock->min15.size() << " 15min-aggregates missing) price:" << lastData.close << "$\n";
				Sleep(1000);
				return false;
			}

		}
		catch (std::exception) {
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][EXIT] Realtimedata = 0\n";
			Sleep(1000);
			return false;
		}

		// check stop loss
		if ((((lastData.close * 100) / this->buyIn) - 100) <= -8) {
			tm time = this->stock->getCurrentTMtime();
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][STOP-LOSS] EXITPOINT FOUND (SMA:" << MAVG << ", PRICE:" << lastData.close << ")" << " (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";
			this->sellSignal(buyIn, lastData.close, MAVG);
			this->buyOut = lastData.close;

			Sleep(1000);
			return true;
		}


		// check moving average
		if (analyzeForBuyOut(lastData, MAVG)) {
			tm time = this->stock->getCurrentTMtime();
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][SELL] EXITPOINT FOUND (SMA:" << MAVG << ", PRICE:" << lastData.close << ")" << " (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";
			sellSignal(buyIn, lastData.close, MAVG);
			Sleep(1000);
			return true;

		}
		else {
			tm time = this->stock->getCurrentTMtime();
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][EXIT] No exitpoint found yet (SMA:" << MAVG << ", PRICE:" << lastData.close << ", BUYIN: " << this->buyIn << "$) (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";
			Sleep(1000);
			return false;
		}

	}

}



void SWINGTRADE::updateData() {


	checkMarketTrend();


}


// 26 15 min candles per day

void SWINGTRADE::checkMarketTrend() {



	for (int x = 0; x < 140; x++) {

		double mavg = 0;
		this->stock->getSimpleMovingAverage(2, 50, mavg, x);
		
		if (mavg != 0)
			movingAverages.push_back(mavg);

	}


	double down = 1;
	double up = 1;
	double lastHighValue = movingAverages.at(0);

	for (int x = 0; x < movingAverages.size() - 2; x++) {

		if (movingAverages.at(x) > lastHighValue) {
			this->highestSMA5days = x;
		}

		if (movingAverages.at(x) < movingAverages.at(x + 1)) {
			down += 1;
		}
		else {
			up += 1;
		}

		switch (x) {

		case 24:
			this->marketTrend1day = up / down;
			break;

		case 50:
			this->marketTrend2days = up / down;
			break;

		case 76:
			this->marketTrend3days = up / down;
			break;

		case 112:
			this->marketTrend4days = up / down;
			break;

		case 128:
			this->marketTrend5days = up / down;
			break;

		}

	}


	std::cout << this->stock->getTimeStamp() << "[UPDATER] Market Trend updated\n";
	

}



bool SWINGTRADE::analyzeForBuyIn(realtimedata& lastData, double& MAVG) {

	// have to change buy In type
	
	
	// available data: highestSMA5days marketTrend, movingaverages, mavg, lastdata

	updateData();

	if (!lastData.close > MAVG) {
		return false;;
	}


	if (this->marketTrend1day < 1) {
		// analyze these cases
		return true;
	}


	return true;
}



bool SWINGTRADE::analyzeForBuyOut(realtimedata& lastData, double& MAVG) {

	// buy out has to depend on buyInType
	

	return false;
}



bool SWINGTRADE::searchEntryPoint(double& MAVG, realtimedata& lastData) {


	while (true) {

		// get the data
		try {

			lastData = this->stock->stockData.at(this->stock->stockData.size() - 1);

			if (this->stock->getSimpleMovingAverage(STOCK::MIN15, 50, MAVG, 0) == false) {
				std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][ENTRY] Need more Data for SMA (" << 50 - this->stock->min15.size() << " 15min-aggregates missing) last price:" << lastData.close << "$\n";
				Sleep(1000);
				return false;
			}

		}
		catch (std::exception) {
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][ENTRY] Realtimedata = 0\n";
			Sleep(1000);
			return false;
		}

		// check
		if (analyzeForBuyIn(lastData, MAVG)) {
			
			buySignal(lastData.close, MAVG);
			
			Sleep(1000);
			return true;

		}
		else {
			tm time = this->stock->getCurrentTMtime();
			std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][ENTRY] No Entrypoint found ... (SMA:" << MAVG << ", PRICE:" << lastData.close << ")" << " (" << time.tm_hour << ":" << time.tm_min << ":" << time.tm_sec << ")\n";
			Sleep(1000);
			return false;
		}

		Sleep(1000);
	}
}



void SWINGTRADE::buySignal(double buyIn, double MAVG) {

	using namespace ::databaseconnection;

	std::ofstream signals(this->signalsFile, std::ios::app);

	if (signals.is_open()) {
		
		tm time = this->stock->getCurrentTMtime();
		std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][SWINGTRADE][ENTRY] ENTRYPOINT FOUND (SMA:" << MAVG << ", PRICE:" << buyIn << ", TYPE: " << this->buyInType <<")" << " (" << time.tm_hour << ":" << time.tm_min << " : " << time.tm_sec << ")\n";
		this->buyIn = buyIn;
		signals << std::to_string(currentTradeID) + " BUY [" + std::to_string(buyIn) + "," + std::to_string(MAVG) + ",(" + std::to_string(time.tm_year + 1900) + "." + std::to_string(time.tm_mon + 1) + "." + std::to_string(time.tm_mday) + "-" + this->stock->getTimeStamp() + ")(TYPE:" + std::to_string(this->buyInType) + ")\n";

		//addBuyInMySQL(buyIn);

		signals.close();
	}
	else {
		std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][ERROR] Could not open signals File\n";
	}


}



void SWINGTRADE::sellSignal(double buyIn, double buyOut, double MAVG) {

	std::ofstream signals(this->signalsFile, std::ios::app);

	if (signals.is_open()) {
		tm time = this->stock->getCurrentTMtime();
		signals << "SELL [" + std::to_string(buyIn) + "," + std::to_string(buyOut) + "," + std::to_string(MAVG) + ",(" + std::to_string(time.tm_year + 1900) + "." + std::to_string(time.tm_mon + 1) + "." + std::to_string(time.tm_mday) + "-" + this->stock->getTimeStamp() + ")\n";

		//addBuyOutMySQL(buyOut);

		this->currentTradeID++;
		this->buyIn = -1;
		this->buyInType = -1;

		signals.close();
	}
	else {
		std::cout << this->stock->getTimeStamp() << "[" << this->stock->symbol << "][ERROR] Could not open signals File\n";
	}

}



void SWINGTRADE::addBuyInMySQL(double buyIn) {

	using namespace ::databaseconnection;

	MYSQL* con = nullptr;
	MYSQL_RES* res = nullptr;
	MYSQL_ROW row = NULL;

	con = mysql_connection_setup();
	tm time = getCurrentTMtime();
	res = mysql_perform_query(con, "INSERT INTO tb_swing_algorithm_signals VALUES(\"" + std::to_string(this->currentTradeID) + "\", \"BUY\", \"" + std::to_string(buyIn) + "\"," + "\"" + std::to_string(time.tm_year + 1900) + "." + std::to_string(time.tm_mon + 1) + "." + std::to_string(time.tm_mday) + "-" + this->stock->getTimeStamp() + "\"" + ", NULL, NULL)");
	mysql_free_result(res);
	mysql_close(con);

}



void SWINGTRADE::addBuyOutMySQL(double buyOut) {

	using namespace ::databaseconnection;

	MYSQL* con = nullptr;
	MYSQL_RES* res = nullptr;
	MYSQL_ROW row = NULL;


	con = mysql_connection_setup();
	tm time = getCurrentTMtime();
	res = mysql_perform_query(con, "UPDATE tb_swing_algorithm_signals SET state = \"SELL\", buy_out = \"" + std::to_string(buyOut) + "\", buy_out_timestamp = " + "\"" + std::to_string(time.tm_year + 1900) + "." + std::to_string(time.tm_mon + 1) + "." + std::to_string(time.tm_mday) + "-" + getTimeStamp() + "\"" + " WHERE ID = " + std::to_string(this->currentTradeID) + "; ");
	mysql_free_result(res);
	mysql_close(con);

}