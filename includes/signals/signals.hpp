#ifndef SIGNALS_HPP
#define SIGNALS_HPP

#include <csignal>
#include <cstdlib>
#include <iostream>

extern volatile sig_atomic_t g_sig;

void sigHandler(int sig);
void setupSigHandler();



#endif