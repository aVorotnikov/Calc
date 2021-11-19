/**
 * @file
 * @brief Token decalaration file
 * @authors Vorotnikov Andrey
 */

#pragma once

#ifndef __TOKEN_H_INCLUDED
#define __TOKEN_H_INCLUDED

#include "operation.h"

/// @brief Token representation type
struct Token {
  /// @brief Token type enumeration
  enum class Type {
    operation,  ///< operation token
    value       ///< number token
  };
  Type type;                        ///< Type
  double num;                       ///< Number
  std::string operName;             ///< Operation name
  std::shared_ptr<Operation> oper;  ///< Pointer to operation struct
};

#endif // !__TOKEN_H_INCLUDED

