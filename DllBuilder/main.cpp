/**
 * @file
 * @brief Main program source file
 * @authors Vorotnikov Andrey
 */

 /**
  * @mainpage Introduction
  * @author Vorotnikov Andrey
  *
  * Dll creator for calculator.
  */

#include <list>
#include "operation.h"

extern "C" __declspec(dllexport) std::list<Operation>* GetOperations(void);

/// @brief Get operation list for calculator
/// @return Pointer to operation list
std::list<Operation>* GetOperations(void) {
  return new std::list<Operation>(
    {
      {40, "^", Operation::Type::binary, [](std::vector<double> const &args) -> double {
          if (args[0] < 0 || (args[0] == 0 && args[1] < 0))
            throw std::exception("Incorrect pow arguments");
          return pow(args[0], args[1]);
        }
      },
      {20, "sin", Operation::Type::infix, [](std::vector<double> const &args) -> double {return sin(args[0]); }},
    });
}

#if 0
/// @brief Get operation list for calculator
/// @return Pointer to operation list
std::list<Operation> *GetOperations(void) {
  return new std::list<Operation>(
    {
      {40, "i", Operation::Type::postfix, [](std::vector<double> const &args) -> double {
          return static_cast<double>(static_cast<int>(args[0]));
        }
      }
    });
}
#endif // 0
