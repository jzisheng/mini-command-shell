#ifndef __SHELL_H_
#define __SHELL_H_
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "args.h"
#include "process.h"

const std::string EXIT_STR = "exit";
/*
  Shell Class:
  This class is mainly responsible for storing all 
  arguments and variables of the shell program,
  and executing processes in the shell
 */
class Shell {
 private:
  std::vector<std::string> paths_v;
  std::vector<std::string> export_v;
  std::map<std::string, std::string> var_m;
  std::string curr_path;
  bool running;

 public:
  Shell() {
    /*
      Main constructor function for initializing 
      PATH environment  of the shell
     */
    char * env_p = std::getenv("PATH");
    std::string s_env_p = (std::string)env_p;
    getEnvPath(s_env_p);
    running = true;
  }

  void getEnvPath(const std::string & paths) {
    /*
      Method takes in a string of path variables
      colon delimited, and generates a paths variable
      vector
     */
    paths_v.clear();
    std::istringstream ss(paths);
    std::string path;
    while (std::getline(ss, path, ':')) {
      paths_v.push_back(path);
    }
  }

  bool setVar(Args & args) {
    /*
      Method for parsing a new variable to store
      in variables map
      If variable exists already, update the existing
      variable
     */
    if (args.getArgsVec().size() == 3) {
      if (args.getArgsVec()[1] == "ECE551PATH") {
        // if the variable being changes is the path update it
        getEnvPath(args.getArgsVec()[2]);
      }
      else {
        std::map<std::string, std::string>::iterator it =
            var_m.find("$" + args.getArgsVec()[1]);
        if (it != var_m.end()) {
          var_m.erase(it);
        }
        var_m.insert(std::pair<std::string, std::string>("$" + args.getArgsVec()[1],
                                                         args.getArgsVec()[2]));
      }
      return true;
    }
    std::cout << "Invalid args for set var" << std::endl;
    return false;
  }

  bool exportVar(Args & args) {
    /*
      Adds a variables for export in execve to a vector
      
      Takes as input reference to args class storing
      all args
     */
    for (size_t i = 0; i < export_v.size(); i++) {
      if (export_v[i].find(args.getArgsVec()[1]) != std::string::npos) {
        // erase var
        export_v.erase(export_v.begin() + i);
      }
    }

    if (args.getArgsVec().size() == 2) {
      // check that valid number of arguments passed
      try {
        std::string val = var_m.at("$" + args.getArgsVec()[1]);
        export_v.push_back(args.getArgsVec()[1] + "=" + val);
        return true;
      }
      catch (const std::out_of_range & oor) {
        std::cout << "Variable not found" << std::endl;
        return false;
      }
    }
    std::cout << "Invalid args for export var" << std::endl;
    return false;
  }

  bool revVar(Args & args) {
    /*
      Given a variable name, reverse the key
      If variable to rev not found, returns false
      returns true if successful

      Takes as input reference to a args class
      storing all arguments
     */
    if (args.getArgsVec().size() == 2) {
      // valid number of arguments
      std::map<std::string, std::string>::iterator it =
          var_m.find("$" + args.getArgsVec()[1]);
      if (it != var_m.end()) {
        std::string rev_str(it->second);
        std::reverse(rev_str.begin(), rev_str.end());
        it->second = rev_str;
        return true;
      }
      return false;
    }
    // invalid number of args
    std::cout << "Invalid args for reverse variable" << std::endl;
    return false;
  }

  void runShell() {
    /*
      Method for initializing the shell
      Responsible for changing directories
      Prints out the cwd, and parses the input lines
      Checks if command is called for set, export, or rev
      otherwise searches path for program
     */
    std::string input;
    char cwd[256];
    getcwd(cwd, 256);
    std::string cwd_str = cwd;

    while (running && !std::cin.eof()) {
      getcwd(cwd, 256);
      cwd_str = cwd;
      std::cout << "ffosh:" << cwd_str << "$ ";
      std::getline(std::cin, input);
      if (input.size() > 0) {
        running = ((input.compare(EXIT_STR) != 0) && !(std::cin.eof()));
        if (!running) {
          break;
        }
        // remove any new lines
        input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
        Args args(input);
        if (args.isValid()) {
          if (args.getArgsVec()[0].compare("set") == 0) {
            if (!setVar(args)) {
              std::cout << "Invalid set command" << std::endl;
            }
          }
          else if (args.getArgsVec()[0].compare("export") == 0) {
            if (!exportVar(args)) {
              std::cout << "Invalid export command" << std::endl;
            }
          }
          else if (args.getArgsVec()[0].compare("rev") == 0) {
            if (!revVar(args)) {
              std::cout << "Invalid rev command" << std::endl;
            }
          }
          else if (args.getArgsVec()[0].compare("cd") == 0) {
            args.parseVariables(var_m);
            int result = chdir(args.getArgsVec()[1].c_str());
            if (result == -1) {
              std::cout << "ffosh: cd: " << args.getArgsVec()[1]
                        << ": No such file or directory" << std::endl;
            }
          }
          else {
            Process p;
            args.parseVariables(var_m);
            p.executeProcess(&args, &paths_v, &export_v);
          }
        }
        else {
          std::cout << "Invalid argument\n";
        }
      }
    }
  }
};

#endif
