
#pragma once

#pragma warning( push )
#pragma warning( disable : 4715)

#ifndef AGGREGATE_H
#define AGGREGATE_H


struct aggregate {

	double high;
	double low;
	double open;
	double close;
	bool isEmpty;

	static inline aggregate getAggregateStruct(std::string& dataLine);
};

aggregate aggregate::getAggregateStruct(std::string& dataLine) {

    aggregate data;

    std::string value = "";
    int valueType = 0;
    for (char& character : dataLine) {
        if (character == ',') {
            switch (valueType) {

            case 0:
            {
                data.open = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 1:
            {
                data.close = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 2:
            {
                data.high = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 3:
            {
                data.low = std::stod(value);
                value = "";
                return data;
                valueType++;

            }break;

            }
        }
        else {
            value += character;
        }
    }

}


#pragma warning( pop )

#endif // !AGGREGATES_H
