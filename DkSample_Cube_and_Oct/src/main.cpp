#define MAIN_SRC
#include "DkCommon.h"
#include "DkSample_Cube_and_Oct.h"

int main() {
	DkSample_Cube_and_Oct app;
	if (!app.init()) std::cout << "Mission failed." << std::endl;
	else {
		app.render();
		std::cout << "Mission accomplished." << std::endl;
	}
}