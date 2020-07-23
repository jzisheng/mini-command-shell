#ifndef __PROCESS_H_
#define __PROCESS_H_
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
#include <sstream>
#include <vector>

#include "args.h"

/*
  Function that checks if a specified file name
  exists 
 */
bool fexists(std::string filename) {
  struct stat fileAt;
  if (stat(filename.c_str(), &fileAt) != 0) {
    return false;
  }
  return S_ISREG(fileAt.st_mode);
}
/*
  Process Class:
  This class contains methods for finding the program
  executable given a paths_v vector of all paths and executes
  the programs in the shell
 */
class Process {
 private:
 public:
  std::string findCommand(Args * args, std::vector<std::string> * paths_v) {
    /*
      Function that iterates through a paths string vector, checking if the 
      program exists, and returning the absolute path of the program 
      of the first appearing program in the vector string paths_v

      Takes in as input vector map of valid maps paths_v, and a reference
      to the args class args
   */
    bool pathFound = false;
    std::string path;

    if (args->getSize() > 0) {
      // if there is a ./ exec from local dir
      if (args->getArgsVec()[0].substr(0, 2) == "./") {
        return args->getArgsVec()[0];
      }

      if (args->getArgsVec()[0][0] == '/') {
        return args->getArgsVec()[0];
      }

      size_t i;
      for (i = 0; i < paths_v->size(); i++) {
        path = ((*paths_v)[i] + "/" + args->getArgsVector()[0]);
        // check if file exists
        if (fexists(path)) {
          // args->getArgsVector()[0] = path;
          pathFound = true;
          break;
        }
      }
    }
    if (pathFound) {
      return path;
    }
    // return empty string if no path found
    return "";
  }

  void initCharArr(std::vector<std::string> vec, char ** args_arr, size_t num_args) {
    /*
      Method when given a vector, char pointer array, and number of arguments,
      mallocs the char array, and loads the std::strings from the vector vec
      into a char array. 
      In this class, used for transforming vectors into char array for use in execve

      Takes in as input the vector string vec, a poitner to the args_arr for the
      vector to be copied into, and the number of arguments num_args
     */
    for (size_t i = 0; i < vec.size(); i++) {
      args_arr[i] = new char[vec[i].size() + 1];
      strncpy(args_arr[i], vec[i].c_str(), vec[i].size());
      args_arr[i][vec[i].size()] = '\0';
    }

    args_arr[num_args] = NULL;
  }

  void destroyCharArr(char ** args_arr, size_t num_args) {
    /*
      Given a char arary and the number of arguments, deallocs
      any malloced space used by the pointer args_arr
      
      Takes in as input a pointer to a pointer args_arr and the number
      of arguments num_args
     */

    if (num_args > 0) {
      for (size_t i = 0; i < num_args; i++) {
        delete[] args_arr[i];
      }
      delete[] args_arr;
    }
    else {
      delete[] args_arr;
    }
  }

  void executeProcess(Args * args,
                      std::vector<std::string> * paths_v,
                      std::vector<std::string> * export_v) {
    /*
      Method for executing spawning a child process and having the parent wait
      for the child process given the args class, the paths of the shell
      and exports any variables to the execve call

      Takes in as input
      Args * args: pointer to arguments class storing all arguments
      vector<string> paths_v: vector string of paths to check for programs
      vector<string export_v: vector string of any variables to be exported
     */
    if (args->isValid()) {
      pid_t child_pid;
      int child_status;
      // forks the main program
      child_pid = fork();
      if (child_pid == -1) {
        std::cout << "Failed to create a child process" << std::endl;
      }
      if (child_pid == 0) {
        // Arguments are valid and is now child process
        std::string cmdPath;
        cmdPath = findCommand(args, paths_v);
        size_t num_args = (size_t)args->getArgsVec().size();
        size_t num_vars = (size_t)export_v->size();
        // initialize args_arr for passing to execve
        char ** args_arr = new char *[num_args + 1];
        initCharArr(args->getArgsVec(), args_arr, num_args);
        // initialize venv_arr for passing exported vars to execve
        char ** venv_arr = new char *[num_vars + 1];
        initCharArr(*export_v, venv_arr, num_vars);
        // execute process only if valid path found
        if (cmdPath.size() > 0) {
          execve((cmdPath.c_str()), args_arr, venv_arr);
        }
        // exits if program fails
        std::cout << args->getArgsVec()[0] << ": execve failed" << std::endl;
        destroyCharArr(args_arr, num_args);
        destroyCharArr(venv_arr, num_args);
        exit(EXIT_FAILURE);
      }
      else {
        // run by parent, wait for the child process to complete
        pid_t tpid = wait(&child_status);
        if (tpid != child_pid) {
          //process terminated
        }
        while (tpid != child_pid) {
          tpid = wait(&child_status);
        }
      }
    }
    else {
      // arguments are not valid and prints error message
      std::cout << "Invalid arguments" << std::endl;
    }
  }
};

#endif
