#define MAIN_SRC
#include "DkCommon.h"
#include "DkApplication.h"

int main() {
	DkApplication& app = DkApplication::getInstance();
	if (!app.init()) std::cout << "Mission failed." << std::endl;
	else {
		app.render();
		std::cout << "Mission accomplished." << std::endl;
	}
}