#ifndef __ARGS_H_
#define __ARGS_H_

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

/*
  Args Class:
  Class for parsing std::string into
  arguments for execve in Shell Class
 */
class Args {
 private:
  std::vector<std::string> args_v;
  bool valid_args;  // = false;
  size_t args_n;    // = 0;
 public:
  Args() {
    valid_args = false;
    args_n = 0;
  }
  Args(std::string s) {
    valid_args = false;
    args_n = 0;
    readInput(s);
  };

  std::vector<std::string> getArgsVec() { return args_v; }

  std::string parseQuotesArg(std::string str, size_t * idx) {
    /*
      Method for parsing a quotes argument. Takes as input
      a string, and starting index of quote
     */
    valid_args = false;
    std::string arg;
    (*idx)++;  // skip over first quotation mark
    while (*idx < str.length()) {
      if (str[*idx] == '\\') {
        (*idx)++;
        arg.push_back(str[*idx]);
        (*idx)++;
      }
      else {
        if (str[*idx] == '\"') {
          valid_args = true;
          break;
        }
        arg.push_back(str[*idx]);
        (*idx)++;
      }
    }
    return arg;
  }

  std::string parseArg(std::string str, size_t * idx) {
    /*
      Method for parsing a normal argument. Takes as input
      a string, and starting index of argument
     */
    std::string arg;
    while (*idx < str.length()) {
      if (str[*idx] == ' ') {
        break;
      }
      arg.push_back(str[*idx]);
      (*idx)++;
    }
    valid_args = true;
    return arg;
  }

  void readInput(std::string str) {
    /*
      Method for parsing input std::string into
      arguments for execve
      First makes sure that the string is not empty,
      then iterates through white space parsing
      both normal arguments, backslashes, and quote strings
      Takes in as input a string to be parsed
     */
    size_t idx = 0;

    // non empty string
    while (idx < str.length()) {
      while (str[idx] == ' ' && idx < str.length()) {
        idx++;
      }

      if (str[idx] == '\"') {
        // Iterated character is a quotation mark, parse quoted
        // string
        std::string result = parseQuotesArg(str, &idx);
        if (result.length() > 0) {
          args_v.push_back(result);
          args_n++;
          idx++;
        }
      }
      else {
        // Iterated character is a normal argument, parse it
        std::string result = parseArg(str, &idx);
        if (result.length() > 0) {
          args_v.push_back(result);
          args_n++;
        }
      }
    }
  }
  void parseVariables(std::map<std::string, std::string> & var_m) {
    /*
      Method for parsing any arguments in vector with the variable values
      Iterates through arguments, and checks that if the $ call does not have
      alphanumeric chars before or after the variable string
     */
    for (size_t i = 0; i < args_v.size(); i++) {
      std::map<std::string, std::string>::iterator it = var_m.begin();
      while (it != var_m.end()) {
        // iterate through the variables map, and see if any $variable
        // appears in args vector string
        std::string var_key = it->first;
        std::string var_val = it->second;
        size_t idx = args_v[i].find(var_key);
        if (idx < args_v[i].size() && idx >= 0) {
          if (idx + var_key.size() < args_v[i].size()) {
            // check if last character is alpha numeric if yes, break
            if (isalnum(args_v[i][idx + var_key.size()])) {
              break;
            }
          }
          if (idx - 1 > 0) {
            // check if first character prior to $ is alpha numeric if yes, break
            if (isalnum(args_v[i][idx - 1]) && args_v[i][idx - 1] != ' ') {
              break;
            }
          }
          args_v[i].replace(idx, var_key.size(), var_val);
        }
        it++;
      }
    }
  }
  // returns the number of arguments
  size_t getSize() { return args_v.size(); }
  // returns the argument string vector
  std::vector<std::string> getArgsVector() { return args_v; }
  // returns whether the arguments vector is valid
  bool isValid() { return valid_args; }
};

#endif
