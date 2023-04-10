#pragma once
#pragma warning(suppress: 4267)

#ifndef PREVIOUSDAY_H
#define PREVIOUSDAY_H

#include <string>

struct previousDay {

	long long int volume;
	double volumeWeighted;
	double open;
	double close;
	double high;
	double low;
	int numberTransactions;

	static inline previousDay getPreviousDayData(std::string url);

};


previousDay previousDay::getPreviousDayData(std::string url) {

	previousDay ddata;
	std::string data = cpr::Get(cpr::Url{ url }).text;
	std::string value = "";

	// volume
	size_t index = data.find("\"v\"");
	if (index != std::string::npos) {
		index += 2;
		while (data.at(index) != ',' && data.at(index) != '}') {

			if (std::isdigit(data.at(index)) || data.at(index) == '+' || data.at(index) == 'e' || data.at(index) == '.') {
				value += data.at(index);
			}

			index++;
		}

		std::string actualValue = "";
		int iter = 0;
		try {
			while ((value.at(iter) != 'e' && value.at(iter) != '}' && value.at(iter) != ',') && (std::isdigit(value.at(iter)) || value.at(iter) == '.')) {
				actualValue += value.at(iter);
				iter++;
			}
		}
		catch (std::exception) {

		}

		size_t scientificIndex = value.find('+');
		if (scientificIndex != std::string::npos) {

			scientificIndex += 1;

			std::string scientificValue = "";
			try {
				while (std::isdigit(value.at(scientificIndex))) {
					scientificValue += value.at(scientificIndex);
					scientificIndex++;
				}
			}
			catch (std::exception) {

			}

			int scientificValueINT = std::stoi(scientificValue) + 1;
			std::string volumeValue = "";

			for (int x = 0; x < scientificValueINT; x++) {

				try {
					if (std::isdigit(actualValue.at(x))) {
						volumeValue += actualValue.at(x);
					}
					else {
						scientificValueINT += 1;
					}
				}
				catch (std::out_of_range) {
					volumeValue += '0';
				}
			}
			ddata.volume = std::stoll(volumeValue);

		}
		else {
			ddata.volume = std::stoll(value);
		}

	}
	value = "";

	// vw
	index = data.find("\"vw\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.volumeWeighted = std::stod(value);
		value = "";
	}


	// o
	index = data.find("\"o\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.open = std::stod(value);
		value = "";
	}

	// c
	index = data.find("\"c\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.close = std::stod(value);
		value = "";
	}

	// h
	index = data.find("\"h\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.high = std::stod(value);
		value = "";
	}


	// l
	index = data.find("\"l\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.low = std::stod(value);
		value = "";
	}

	index = data.find("\"l\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.low = std::stod(value);
		value = "";
	}


	index = data.find("\"n\"");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index))) {
				value += data.at(index);
			}
			index++;
		}
		ddata.numberTransactions = std::stoi(value);
		
	}

	return ddata;
}


#endif // !PREVIOUSDAY_H
