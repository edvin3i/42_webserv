#include "../../includes/signals/signals.hpp"

volatile sig_atomic_t g_sig = 0;


void sigHandler(int sig) {
	if (sig == SIGINT) {
		g_sig = 1;
    	std::cout << "\nCTRL+C pressed\nWebserver stopped." << std::endl;
	}
	else if (sig == SIGTERM) {
		g_sig = 1;
		std::cout << "\nWebserver process terminated" << std::endl;
	}
}


void setupSigHandler() {
	struct  sigaction sa = {};
	sa.sa_handler = sigHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		std::cerr << "Error with SIGINT setup!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		std::cerr << "Error with SIGTERM setup!" << std::endl;
		exit(EXIT_FAILURE);
	}
}