#include "Engine/gola_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

/// <summary>
/// 程序入口
/// </summary>
int main() {
	std::cout.setf(std::ios::unitbuf);
	gola::GolaApp app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << "An error occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}