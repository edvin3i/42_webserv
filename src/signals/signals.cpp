#include "../../includes/signals/signals.hpp"

volatile sig_atomic_t g_sig = 0;


void sigIntHandler(int sig) {
	if (sig == SIGINT) {
		g_sig = 1;
    	std::cout << "\nCTRL+C pressed" << std::endl;
	}
}


void setupSigHandler() {
	struct  sigaction sa = {};
	sa.sa_handler = sigIntHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		std::cerr << "Error with sigaction setup!" << std::endl;
		exit(EXIT_FAILURE);
	}
}