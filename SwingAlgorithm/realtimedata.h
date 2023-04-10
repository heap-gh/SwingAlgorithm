
#pragma once

#ifndef REALTIMEDATA_H
#define REALTIMEDATA_H

#include <string>
#include <vector>

struct realtimedata {


    long long int volume;
    long long int accumulated_volume;
    double vwap;
    double open;
    double close;
    double high;
    double low;
    double aggregate_vwap;
    double average_size;
    long long int start_timestamp;
    long long int end_timestamp;
    std::string otc;
    bool isEmpty = false;

    static inline realtimedata getRealTimeDataStruct(std::string data);

};


realtimedata realtimedata::getRealTimeDataStruct(std::string dataLine) {

    std::vector <std::string> values = { "volume=", "accumulated_volume=", "vwap=", "open=", "close=", "high=", "low=", "aggregate_vwap=", "average_size=", "start_timestamp=", "end_timestamp=" };

    int count = 0;

    realtimedata dataStruct;
    for (std::string value : values) {
        size_t x = dataLine.find(value);
        std::string unit = "";
        while (dataLine[x] != '=') {
            x++;
        }
        x++;
        while (dataLine[x] != ',') {
            unit += dataLine[x];
            x++;
        }

        switch (count) {

        case 0:
            dataStruct.volume = std::stoll(unit);
            count++;
            break;

        case 1:
            dataStruct.accumulated_volume = std::stoll(unit);
            count++;
            break;

        case 2:
            dataStruct.vwap = std::stod(unit);
            count++;
            break;

        case 3:
            dataStruct.open = std::stod(unit);
            count++;
            break;

        case 4:
            dataStruct.close = std::stod(unit);
            count++;
            break;

        case 5:
            dataStruct.high = std::stod(unit);
            count++;
            break;

        case 6:
            dataStruct.low = std::stod(unit);
            count++;
            break;

        case 7:
            dataStruct.aggregate_vwap = std::stod(unit);
            count++;
            break;

        case 8:
            dataStruct.average_size = std::stod(unit);
            count++;
            break;

        case 9:
            dataStruct.start_timestamp = std::stoll(unit);
            count++;
            break;

        case 10:
            dataStruct.end_timestamp = std::stoll(unit);
            break;
        }
    }

    return dataStruct;
}

#endif // !REALTIMEDATA_H
