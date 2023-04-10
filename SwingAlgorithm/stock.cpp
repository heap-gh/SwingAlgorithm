#pragma warning( push )
#pragma warning( disable : 4244)

#include "stock.h"


#pragma warning( pop )


STOCK::STOCK() {

    std::cout << "Enter your Polygon-API key: \n";
    std::cin >> this->apikey;
    std::cout << "Enter the symbol in caps you want to analyze (Only from S&P500) \n";
    std::cin >> this->symbol;

    this->marketOpen = checkMarketStatus();
    this->previousDayData = getPreviousDayStruct();

    std::thread mainThread (&STOCK::main, this);
    mainThread.detach();

}

void STOCK::main() {

    // initialize
    
    init();
    stream();

}