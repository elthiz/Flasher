#ifndef OUT_H
#define OUT_H

#include <string>

void print(const std::string& message);
void printProgressBar(int currentIteration, const std::string& prefix, int totalIterations);

#endif