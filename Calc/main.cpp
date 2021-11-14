/**
 * @file
 * @brief Main program source file
 * @authors Vorotnikov Andrey
 */

/**
 * @mainpage Introduction
 * @author Vorotnikov Andrey
 *
 * Calculator project.
 */

#include <iostream>
#include "calc.h"

/// @brief Main program function
int main(void) {
  Calculator calc;
  std::string line;
  while (std::getline(std::cin, line)) {
    try {
      std::cout << calc.Evaluate(line) << std::endl;
    }
    catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  }
  return 0;
}
