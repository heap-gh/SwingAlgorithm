// initialize the data for the tesla stock

#pragma warning( push )
#pragma warning( disable : 4244)

#include "stock.h"
#include "swingtrade.h"

#pragma warning( pop )

std::mutex lock;


void STOCK::init() {

    char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    GetUserNameA(acUserName, &nUserName);

    this->basePath = "C:/Users/"+ std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/";
    this->realtimeFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "RTDATA.txt";
    this->streamFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "stream.txt";
    this->min15AggFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "15m.txt";
    this->min30AggFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "30m.txt";
    this->hour1AggFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "1hour.txt";
    this->hour2AggFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "2hour.txt";
    this->hour4AggFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "4hour.txt";
    this->dailyDataFile = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/symbols/" + symbol + "/" + symbol + "DAILY.txt";
    this->portfolioPath = "C:/Users/" + std::string(acUserName) + "/Desktop/TradingBot/portfolio/" + symbol + "/";


    clearStockStreamFile(true);
    std::cout << getTimeStamp() <<"[INIT] Starting Initialization\n";

    checkBasePath();
    initRealTime();
    init15minAgg();
    init30minAgg();
    init1hourAgg();
    init2hourAgg();
    init4hourAgg();
    //initDailyDataSince();
    //this->averageDailyVolume = updateDailyAverageVolume();
    this->initialized = true;
    
    if (this->marketOpen == false)
        std::cout << getTimeStamp() << "[MARKET] MARKET CURRENTLY CLOSED\n";
    else
        std::cout << getTimeStamp() << "[MARKET] MARKET CURRENTLY OPEN\n";
    
    std::cout << getTimeStamp() << "[INIT] Initialization done. Starting Data stream ...\n";
}



void STOCK::checkBasePath() {

    using namespace boost::filesystem;

    if (!exists(this->basePath)) {
        if (_mkdir(this->basePath.c_str())) {
            std::cout << getTimeStamp() << "[INIT] Base Path created(" << this->basePath << ")\n";
        }
        else {
            std::cout << getTimeStamp() << "[INIT] Could not create Base Path (" << this->basePath << ")\n";
        }
    }

}



std::string STOCK::getTimeStamp() {

    tm time = getCurrentTMtime();

    std::string timestamp = "[";

    if (time.tm_hour < 10) {
        timestamp += "0" + std::to_string(time.tm_hour) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_hour) + ":";
    }

    if (time.tm_min < 10) {
        timestamp += "0" + std::to_string(time.tm_min) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_min) + ":";
    }

    if (time.tm_sec < 10) {
        timestamp += "0" + std::to_string(time.tm_sec);
    }
    else {
        timestamp += std::to_string(time.tm_sec);
    }

    timestamp += "]";

    return timestamp;

}



std::string STOCK::getTimeStamp(tm& time) {

    std::string timestamp = "[";

    if (time.tm_hour < 10) {
        timestamp += "0" + std::to_string(time.tm_hour) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_hour) + ":";
    }

    if (time.tm_min < 10) {
        timestamp += "0" + std::to_string(time.tm_min) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_min) + ":";
    }

    if (time.tm_sec < 10) {
        timestamp += "0" + std::to_string(time.tm_sec);
    }
    else {
        timestamp += std::to_string(time.tm_sec);
    }

    timestamp += "]";

    return timestamp;

}



void STOCK::initRealTime() {

    std::ifstream dataFile(this->realtimeFile);
    std::string data = "";
 
    if (dataFile.is_open())
    {
        while (getline(dataFile, data))
        {
            this->stockData.push_back(realtimedata::getRealTimeDataStruct(data));
        }

        dataFile.close();

        std::cout << "    [+] Real Time Data initialized\n";
    }
    else {
        tm time = getCurrentTMtime();
        std::cout << "    [-] Could not open realtimedata-File for " << this->realtimeFile << "\n";
    }

}



void STOCK::init15minAgg() {

    std::ifstream min15File(this->min15AggFile);
    if (min15File.is_open()) {

        aggregate min15Aggregate;
        std::string data = "";

        while (getline(min15File, data)) {

            this->min15.push_back(min15Aggregate.getAggregateStruct(data));
        }

        std::cout << "    [+] 15min aggregates initialized\n";
    }
    else {
        std::cerr << "    [-] Could not open 15min aggregate File for " << this->min15AggFile << "\n";
    }

}



void STOCK::init30minAgg() {

    std::ifstream min30File(this->min30AggFile);
    if (min30File.is_open()) {

        aggregate min30Aggregate;
        std::string data = "";

        while (getline(min30File, data)) {

            this->min30.push_back(min30Aggregate.getAggregateStruct(data));
        }

        std::cout << "    [+] 30min aggregates initialized\n";

    }
    else {
        std::cerr << "    [-] Could not open 30min aggregate File for " << this->min30AggFile << "\n";
    }

}



void STOCK::init1hourAgg() {

    std::ifstream hour1File(this->hour1AggFile);
    if (hour1File.is_open()) {

        aggregate hour1Aggregate;
        std::string data = "";

        while (getline(hour1File, data)) {

            this->hour1.push_back(hour1Aggregate.getAggregateStruct(data));
        }

        std::cout << "    [+] 1 hour aggregates initialized\n";

    }
    else {
        std::cerr << "    [-] Could not open 1 hour aggregate File for " << this->hour1AggFile << "\n";
    }

}



void STOCK::init2hourAgg() {

    std::ifstream hour2File(this->hour2AggFile);
    if (hour2File.is_open()) {

        aggregate hour2Aggregate;
        std::string data = "";

        while (getline(hour2File, data)) {

            this->hour2.push_back(hour2Aggregate.getAggregateStruct(data));
        }

        std::cout << "    [+] 2 hour aggregates initialized\n";

    }
    else {
        std::cerr << "    [-] Could not open 2 hour aggregate File for " << this->hour2AggFile << "\n";
    }

}



void STOCK::init4hourAgg() {

    std::ifstream hour4File(this->hour4AggFile);
    if (hour4File.is_open()) {

        aggregate hour4Aggregate;
        std::string data = "";

        while (getline(hour4File, data)) {

            this->hour4.push_back(hour4Aggregate.getAggregateStruct(data));
        }

        std::cout << "    [+] 4 hour aggregates initialized\n";

    }
    else {
        std::cerr << "    [-] Could not open 4 hour aggregate File for " << this->hour4AggFile << "\n";
    }

}



void STOCK::initDailyAverageVolume() {

    double avgVol = 0.0;

    for (realtimedata& rdata : this->stockData) {
        avgVol += rdata.volume;
    }

    std::cout << "    [+] Daily Average Volume initialized (" << avgVol << ")\n";

}



void STOCK::makeGetRequestAndPushBack(std::string url, std::ofstream& dataFile) {

    auto response = cpr::Get(cpr::Url{ url }).text;

    if (response.find("ERROR") == std::string::npos && response.find("NOT_FOUND") == std::string::npos) {
        lock.lock();
        dataFile << response << "\n";
        this->dailyData.push_back(dailydata::getDailyDataStruct(response));
        lock.unlock();
    }

}



std::string STOCK::getCurrentDayString() {

    tm time = getCurrentTMtime();

    std::string yearSSTR = std::to_string(time.tm_year + 1900);
    std::string monthSTR = "";
    if (time.tm_mon < 10) {
        monthSTR += "0" + std::to_string(time.tm_mon + 1);
    }
    else {
        monthSTR = std::to_string(time.tm_mon + 1);
    }

    std::string daySTR = "";
    if (time.tm_mday < 10) {
        daySTR += "0" + std::to_string(time.tm_mday);
    }
    else {
        daySTR = std::to_string(time.tm_mday);
    }

    return yearSSTR + "-" + monthSTR + "-" + daySTR;

}



void STOCK::initDailyDataSince() {

    if (!boost::filesystem::exists(this->dailyDataFile)) {

        std::cout << "    [+] Downloading and initializing Daily Aggreagtes --> ";

        std::string daystring = getCurrentDayString();

        std::ofstream dataFile(this->dailyDataFile, std::ios::app);

        if (dataFile.is_open()) {

            tm time = getCurrentTMtime();
            std::string date = "";
            std::vector <std::thread> workerThreads;

            for (int year = time.tm_year + 1900 - 5; year < time.tm_year + 1901; year++) {
                std::string yearSSTR = std::to_string(year);

                for (int month = 1; month < 13; month++) {
                    std::string monthSTR = "";
                    if (month < 10) {
                        monthSTR += "0" + std::to_string(month);
                    }
                    else {
                        monthSTR = std::to_string(month);
                    }

                    for (int day = 1; day < 32; day++) {
                        std::string daySTR = "";
                        if (day < 10) {
                            daySTR += "0" + std::to_string(day);
                        }
                        else {
                            daySTR = std::to_string(day);
                        }

                        date = yearSSTR + "-" + monthSTR + "-" + daySTR;
                        
                        if (date != daystring) {
                            std::string url = "https://api.polygon.io/v1/open-close/" + this->symbol + "/" + date + "?adjusted=true&apiKey=" + this->apikey;
                            std::thread requestThread(&STOCK::makeGetRequestAndPushBack, this, url, std::ref(dataFile));
                            workerThreads.push_back(std::move(requestThread));
                            Sleep(10);
                        }
                    }
                }
            }

            for (std::thread& thread : workerThreads) {
                if (thread.joinable())
                    thread.join();
            }

            sortDailyData();

        }
        else {
            std::cout << "[-] Could not create daily data file --> ";
        }

    }
    else {

        std::cout << "    [+] Initializing Daily Aggreagtes --> ";

        std::ifstream dataFile(this->dailyDataFile);

        if (dataFile.is_open()) {

            std::string dataLine = "";
            std::vector <std::string> data;

            while (getline(dataFile, dataLine)) {
                data.push_back(dataLine);
            }

            dataFile.close();

            if (static_cast<int>(data.size()) < 1200) {
                std::remove(this->dailyDataFile.c_str());
                std::cout << "[-] Missing Data in File\n";
                initDailyDataSince();
                return;
            }

            for (std::string& dataString : data) {
                this->dailyData.push_back(dailydata::getDailyDataStruct(dataString));
            }

            sortDailyData();

        }

    }

    std::cout << "[+] Done\n";

}




void STOCK::sortDailyData() {

    dailydata save;

    for (int x = 0; x < this->dailyData.size(); x++) {
        for (size_t index = 0; index < dailyData.size() - 1; index++) {

            if (std::stoi(dailyData.at(index).from) > std::stoi(dailyData.at(index + 1).from)) {
                save = dailyData.at(index + 1);
                dailyData.at(index + 1) = dailyData.at(index);
                dailyData.at(index) = save;
            }
        }
    }

}