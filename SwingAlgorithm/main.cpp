
#pragma warning(suppress: 4244)

#include "stock.h"
#include "swingtrade.h"

int main() {

	int x = 0;
	STOCK* mainPointer = new(STOCK);
	
	// wait for init
	while (!mainPointer->initialized) {
		Sleep(50);
	}
	
	// start algo
	SWINGTRADE swingtrade(mainPointer);

	while (true) {
		Sleep(60000);
	}
	

}

