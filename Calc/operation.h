/**
 * @file
 * @brief Operation decalaration file
 * @authors Vorotnikov Andrey
 */

#pragma once

#ifndef __OPERATIONS_H_INCLUDED
#define __OPERATIONS_H_INCLUDED

#include <string>
#include <vector>
#include <functional>

/// @brief Operation realisation representation type
using FuncRealisation = std::function<double(std::vector<double> const &)>;

/// @brief Operation representation type
struct Operation {
  /// Operation type enumeratiob
  enum class Type {
    infix,    ///< infix operation
    postfix,  ///< postfix operation
    binary    ///< binary operation
  };
  int prior;             ///< Priority
  std::string token;     ///< Name
  Type type;             ///< Type
  FuncRealisation func;  ///< Realisation
};

#endif // !__OPERATIONS_H_INCLUDED
