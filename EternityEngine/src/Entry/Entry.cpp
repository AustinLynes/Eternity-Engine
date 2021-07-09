
#include "Application.h"
#include <iostream>

extern EternityEngine::Application* EternityEngine::CreateApplication();


int main() {

	auto app = EternityEngine::CreateApplication();
	
	app->Initilize();
	
	try {
		app->Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}