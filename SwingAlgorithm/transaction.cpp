
#pragma warning( push )
#pragma warning( disable : 4715)


//#include "stock.h"


void STOCK::buy(int amount, double buyIn, std::string strategie) {

	if (strategie == "MAVGSTRAT") {
		std::cout << getTimeStamp() << "[MAVGSTRAT][BUY] " << amount << "STOCK(S) BOUGHT@ " << buyIn << "$\n";
		this->myPortfolio.boughtStocks.at(this->myPortfolio.getStockIndex(this->symbol)).MAVGStratTransaction.buyIn = buyIn;
		this->myPortfolio.boughtStocks.at(this->myPortfolio.getStockIndex(this->symbol)).MAVGStratTransaction.amount = amount;

		std::ofstream data(this->myPortfolio.MAVGStratTransactionData, std::ios::app);
		if (data.is_open()) {
			data << amount << " " << buyIn << " ";
			data.close();
		}
	}

	std::ifstream moneyFileRead(this->myPortfolio.portfolioMoney);
	if (moneyFileRead.is_open()) {
		std::string money = "";
		while (getline(moneyFileRead, money));
		moneyFileRead.close();
		std::ofstream moneyFileWrite(this->myPortfolio.portfolioMoney);
		if (moneyFileWrite.is_open()) {
			double newMoney = std::stod(money) - amount * buyIn;
			moneyFileWrite << newMoney;
		}
	}

}


void STOCK::sell(int amount, double buyOut, std::string strategie) {

	// MAVGSTRAT
	if (strategie == "MAVGSTRAT") {
		std::cout << getTimeStamp() << "[MAVGSTRAT][SELL] " << amount << "STOCK(S) SOLD @ " << buyOut << "$\n";
		std::cout << getTimeStamp() << "[MAVGSTRAT][SELL] BuyIn: " << this->myPortfolio.boughtStocks.at(this->myPortfolio.getStockIndex(this->symbol)).MAVGStratTransaction.buyIn << "$ / BuyOut: " << buyOut << "$ | Profit (%): " << ((buyOut * 100) / this->myPortfolio.boughtStocks.at(this->myPortfolio.boughtStocks.size() - 1).MAVGStratTransaction.buyIn) - 100 << " / Profit ($): " << buyOut - this->myPortfolio.boughtStocks.at(this->myPortfolio.boughtStocks.size() - 1).MAVGStratTransaction.buyIn << "\n";

		tm time = getCurrentTMtime();

		std::string dataLine = "";
		std::vector <std::string> dataVector;

		std::ifstream file(this->myPortfolio.MAVGStratTransactionData);
		if (file.is_open()) {
			while (getline(file, dataLine)) {
				dataVector.push_back(dataLine);
			}
		}
		file.close();

		std::ofstream data(this->myPortfolio.MAVGStratTransactionData);
		if (data.is_open()) {
			for (int x = 0; x < dataVector.size(); x++) {

				if (x == dataVector.size() - 1) {
					data << dataVector.at(x) << buyOut << "|" << time.tm_mday << ":" << time.tm_mon << ":" << time.tm_year + 1900 << " " << time.tm_hour << ":" << time.tm_min << "\n";
				}
				else {
					data <<  dataVector.at(x) << "\n";
				}
				
			}
			data.close();
		}
		this->myPortfolio.boughtStocks.at(this->myPortfolio.getStockIndex(this->symbol)).MAVGStratTransaction = transaction(); // new transaction for new buy in
	}

	std::ifstream moneyFileRead(this->myPortfolio.portfolioMoney);
	if (moneyFileRead.is_open()) {
		std::string money = "";
		while (getline(moneyFileRead, money));
		moneyFileRead.close();
		std::ofstream moneyFileWrite(this->myPortfolio.portfolioMoney);
		if (moneyFileWrite.is_open()) {
			double newMoney = std::stod(money) + amount * buyOut;
			moneyFileWrite << newMoney;
		}
	}

}


#pragma warning ( pop )