/**
 * @file
 * @brief Calculator decalaration file
 * @authors Vorotnikov Andrey
 */

#pragma once

#ifndef __CALC_H_INCLUDED
#define __CALC_H_INCLUDED

#include <list>
#include <queue>
#include <vector>
#include <string>
#include <functional>
#include <Windows.h>

#include "token.h"

/// @brief Calculator representation type
class Calculator {
private:
  static const std::list<Operation> defaultOperations;  ///< Default calculator operations
  std::list<Operation> operations;                      ///< Supported operations
  std::list<HMODULE> dlls;                              ///< List of loaded dynamic libraries

  /// @brief Create token list from string
  /// @param[in] str String to create token list
  /// @return token list
  std::list<Token> Scan(std::string const &str);

  /// @brief Create token queue in reverse polish notation
  /// @param[in] tokens List of tokens
  /// @return token queue in RPN
  std::queue<Token> Parse(std::list<Token> const &tokens);

public:
  /// @brief Constructor
  /// @param[in] path path to load dynamic libraries with operations
  Calculator(std::string const &path = "plugins");

  /// @brief Destructor
  ~Calculator(void);

  /// @brief Evaluate expression value
  /// @param[in] str String with expression
  /// @return expression value
  double Evaluate(std::string const &str);
};

#endif // !__CALC_H_INCLUDED
