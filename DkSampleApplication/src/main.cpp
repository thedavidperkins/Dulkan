#define MAIN_SRC
#include "DkCommon.h"
#include "DkSampleApplication.h"

int main() {
	DkSampleApplication app;
	if (!app.init()) std::cout << "Mission failed." << std::endl;
	else {
		app.render();
		std::cout << "Mission accomplished." << std::endl;
	}
}