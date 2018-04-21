#define MAIN_SRC
#include "DkCommon.h"
#include "DkSample_Triangle.h"

int main() {
	DkSample_Triangle app;
	if (!app.init()) std::cout << "Mission failed." << std::endl;
	else {
		app.render();
		std::cout << "Mission accomplished." << std::endl;
	}
}