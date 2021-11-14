/**
 * @file
 * @brief Calculator implementation file
 * @authors Vorotnikov Andrey
 */

#include <exception>
#include <stack>
#include <filesystem>
#include "calc.h"

/// @brief Default calculator operations
const std::list<Operation> Calculator::defaultOperations = {
  {20, "-", Operation::Type::infix, [](std::vector<double> const &args) -> double {return -args[0]; }},
  {30, "*", Operation::Type::binary, [](std::vector<double> const &args) -> double {return args[0] * args[1]; }},
  {30, "/", Operation::Type::binary, [](std::vector<double> const &args) -> double {
      if (args[1] == 0)
        throw std::exception("Division by zero");
      return args[0] / args[1];
    }
  },
  {10, "+", Operation::Type::binary, [](std::vector<double> const &args) -> double {return args[0] + args[1]; }},
  {10, "-", Operation::Type::binary, [](std::vector<double> const &args) -> double {return args[0] - args[1]; }},
};

namespace {
  /// @brief Function to import calculator operations
  using GetOperationsFromDLL = std::list<Operation> *(void);
}

/// @brief Constructor
/// @param[in] path path to load dynamic libraries with operations
Calculator::Calculator(std::string const &path) : operations(defaultOperations) {
  for (auto &p : std::filesystem::directory_iterator(path)) {
    HMODULE hDll = LoadLibrary(p.path().string().c_str());
    if (hDll == NULL)
      continue;
    GetOperationsFromDLL *func = (GetOperationsFromDLL *)GetProcAddress(hDll, "GetOperations");
    if (func == (GetOperationsFromDLL *)NULL) {
      FreeLibrary(hDll);
      continue;
    }
    auto opers = func();
    if (opers == (std::list<Operation>*)NULL) {
      FreeLibrary(hDll);
      continue;
    }
    operations.insert(operations.end(), opers->begin(), opers->end());
    delete opers;
    dlls.push_back(hDll);
  }
}

/// @brief Destructor
Calculator::~Calculator(void) {
  operations.clear();
  for (auto &hDll : dlls)
    FreeLibrary(hDll);
}

/// @brief Create token list from string
/// @param[in] str String to create token list
/// @return token list
std::list<Token> Calculator::Scan(std::string const &str) {
  std::list<Token> res;
  Token t;

  auto &it = str.begin();
  while (it != str.end()) {
    if (isspace(*it)) {
      it++;
      continue;
    }
    Token tok;
    tok.oper = nullptr;
    if (*it == '.' || isdigit(*it)) {
      tok.type = Token::Type::value;
      double num = 0;
      while (it != str.end() && isdigit(*it)) {
        num = 10 * num + (*it - '0');
        it++;
      }
      if (it != str.end()  && *it == '.') {
        it++;
        double muler = 1;
        while (it != str.end() && isdigit(*it)) {
          muler /= 10;
          num += muler * (*it - '0');
          it++;
        }
      }
      tok.num = num;
      res.push_back(tok);
      continue;
    }
    std::string tok_name;
    tok.type = Token::Type::operation;
    if (isalpha(*it)) {
      while (it != str.end() && isalpha(*it)) {
        tok_name += *it;
        it++;
      }
      tok.operName = tok_name;
      res.push_back(tok);
      continue;
    }
    tok.operName = *it;
    res.push_back(tok);
    it++;
  }

  return res;
}

namespace {
  /// @brief Priority >= value function for operations
  /// @param[in] Op1 First param
  /// @param[in] Op2 Second param
  /// @return Priority >= value
  bool MoreOrEqualPrior(Token const &Op1, Token const &Op2) {
    if (Op1.operName == "(")
      if (Op2.operName != "(")
        return false;
      else
        return true;

    if (Op1.operName == ")")
      if (Op2.operName != "(" && Op2.operName != ")")
        return false;
      else
        return true;

    if (Op2.oper == nullptr)
      return true;
    return Op1.oper->prior >= Op2.oper->prior;
  }

  /// @brief Drop operations from stack by priority
  /// @param[in] numStack Stack with numbers
  /// @param[in] opStack Stack with operations
  /// @param[in] Op Operation with priority to drop
  void DropOpers(std::stack<Token> &numStack, std::stack<Token> &opStack, Token const &Op) {
    while (!opStack.empty() && MoreOrEqualPrior(opStack.top(), Op)) {
      numStack.push(opStack.top());
      opStack.pop();
    }
  }
}

/// @brief Create token queue in reverse polish notation
/// @param[in] tokens List of tokens
/// @return token queue in RPN
std::queue<Token> Calculator::Parse(std::list<Token> const &tokens) {
  std::queue<Token> res;
  std::stack<Token> numStack, opStack;

  enum class State {
    prefix,
    suffix,
    done,
    end
  };
  State state = State::prefix;
  Token tok;
  auto it = tokens.begin();

  while (state != State::end) {
    if (state == State::prefix || state == State::suffix)
      if (it == tokens.end())
        if (state == State::suffix)
          state = State::done;
        else
          throw std::exception("Unexpected end of expression");
      else {
        tok = *it;
        it++;
      }

    switch (state) {
      case State::prefix:
        if (tok.type == Token::Type::value) {
          numStack.push(tok);
          state = State::suffix;
        }
        else {
          if (tok.operName != "(") {
            auto opIter = std::find_if(operations.begin(),
              operations.end(),
              [&tok](Operation const &op) {
                if (op.type == Operation::Type::infix && op.token == tok.operName)
                  return true;
                return false;
              });
            if (opIter == operations.end())
              throw std::exception((std::string("Unknown infix operation ") + tok.operName).c_str());
            tok.oper = &*opIter;
          }
          opStack.push(tok);
        }
        break;
      case State::suffix:
        if (tok.type == Token::Type::value || tok.operName == "(")
          throw std::exception("Expect operation");

        if (tok.operName == ")") {
          DropOpers(numStack, opStack, tok);
          if (!opStack.empty() && opStack.top().operName == "(")
            opStack.pop();
          else
            throw std::exception("Missing '('");
        }
        else {
          Operation::Type opType;
          if (it != tokens.end() &&
            (it->type == Token::Type::value || (it->type == Token::Type::operation && it->operName != "(")))
            opType = Operation::Type::binary;
          else
            opType = Operation::Type::postfix;
          auto prediction = [&tok, &opType](Operation const &op) {
            if (op.type == opType && op.token == tok.operName)
              return true;
            return false;
          };
          auto opIter = std::find_if(operations.begin(), operations.end(), prediction);
          if (opIter == operations.end() && opType == Operation::Type::postfix) {
            opType = Operation::Type::binary;
            opIter = std::find_if(operations.begin(), operations.end(), prediction);
          }
          if (opIter == operations.end())
            throw std::exception((std::string("Unknown operation ") + tok.operName).c_str());
          tok.oper = &*opIter;
          DropOpers(numStack, opStack, tok);
          opStack.push(tok);
          if (opType == Operation::Type::binary)
            state = State::prefix;
        }
        break;
      case State::done: {
        Token closeBracket;
        closeBracket.type = Token::Type::operation;
        closeBracket.oper = nullptr;
        closeBracket.operName = ")";
        DropOpers(numStack, opStack, closeBracket);
        if (numStack.empty())
          throw std::exception("Missing ')'");
        while (!numStack.empty()) {
          opStack.push(numStack.top());
          numStack.pop();
        }
        while (!opStack.empty()) {
          res.push(opStack.top());
          opStack.pop();
        }
        state = State::end;
        break;
      }
    }
  }

  return res;
}

/// @brief Evaluate expression value
/// @param[in] str String with expression
/// @return expression value
double Calculator::Evaluate(std::string const &str) {
  auto tokenQ = Parse(Scan(str));
  std::stack<double> numStack;
  std::vector<double> args(2);

  while (!tokenQ.empty()) {
    auto tok = tokenQ.front();
    tokenQ.pop();
    if (tok.type == Token::Type::value)
      numStack.push(tok.num);
    else {
      if (tok.oper->type == Operation::Type::binary) {
        args[1] = numStack.top();
        numStack.pop();
        args[0] = numStack.top();
        numStack.pop();
      }
      else {
        args[0] = numStack.top();
        numStack.pop();
      }
      numStack.push(tok.oper->func(args));
    }
  }
  return numStack.top();
}
